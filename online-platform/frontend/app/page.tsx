"use client";

import { useEffect, useMemo, useRef, useState } from "react";
import Link from "next/link";
import { apiPost } from "@/lib/api";
import { downloadTextFile, parsePartsCsv, partsToCsv } from "@/lib/csv";
import { exportSheetsZip, type ExportKind } from "@/lib/export";
import { getStrings, initialLocale, saveLocale, type Locale } from "@/lib/i18n";
import { normalizeGenerateResponse, normalizeRunResponse } from "@/lib/result";
import type { ImportWarning, NestingResult, Part, PlateParams } from "@/lib/types";

const MAX_PREVIEW_ROWS = 200;

function formatTimer(ms: number): string {
  const totalCs = Math.floor(ms / 10);
  const cs = totalCs % 100;
  const totalSec = Math.floor(totalCs / 100);
  const sec = totalSec % 60;
  const min = Math.floor(totalSec / 60);
  const pad = (n: number) => String(n).padStart(2, "0");
  return `${pad(min)}:${pad(sec)}.${pad(cs)}`;
}

function parseInput(raw: string): number {
  if (raw.trim() === "") return NaN;
  return Number(raw);
}

export default function Home() {
  // i18n：默认跟随浏览器，中文 → zh，其他 → en；手动选择后跨页面保存
  const [locale, setLocaleState] = useState<Locale>(initialLocale);
  const setLocale = (l: Locale) => {
    saveLocale(l);
    setLocaleState(l);
  };
  const s = getStrings(locale);

  useEffect(() => {
    document.documentElement.lang = locale === "zh" ? "zh-CN" : "en";
  }, [locale]);

  // 参数输入（字符串，便于输入控制）
  const [plateLengthInput, setPlateLengthInput] = useState("100");
  const [plateWidthInput, setPlateWidthInput] = useState("50");
  const [countInput, setCountInput] = useState("1000");
  const [edgeInput, setEdgeInput] = useState("0.2");

  // 校验错误（生成按钮）
  const [paramErrors, setParamErrors] = useState<string[]>([]);
  // API / 全局错误
  const [apiError, setApiError] = useState<string | null>(null);

  // 锁定后的板材参数（生成成功后设置）
  const [plate, setPlate] = useState<PlateParams | null>(null);
  const [parts, setParts] = useState<Part[] | null>(null);

  // 导入相关
  const [importWarnings, setImportWarnings] = useState<ImportWarning[]>([]);
  const [importInfo, setImportInfo] = useState<string | null>(null);
  const [dragActive, setDragActive] = useState(false);
  const fileInputRef = useRef<HTMLInputElement>(null);

  // 运算相关
  const [running, setRunning] = useState(false);
  const [elapsedMs, setElapsedMs] = useState(0);
  const [timing, setTiming] = useState(false);
  const [clientSeconds, setClientSeconds] = useState<number | null>(null);
  const [result, setResult] = useState<NestingResult | null>(null);
  const timerRef = useRef<number | null>(null);
  const startRef = useRef(0);

  // 翻页
  const [pageInput, setPageInput] = useState("1");
  const [pageError, setPageError] = useState<string | null>(null);

  // 导出
  const [exporting, setExporting] = useState<ExportKind | null>(null);

  useEffect(() => {
    return () => {
      if (timerRef.current !== null) window.clearInterval(timerRef.current);
    };
  }, []);

  const sheetCount = result?.sheets.length ?? 0;
  const currentPage = Math.min(Math.max(1, Number(pageInput) || 1), Math.max(1, sheetCount));
  const currentSheet = result?.sheets[currentPage - 1] ?? null;

  const canGenerate = !running;
  const canRun = !running && parts !== null && parts.length > 0 && plate !== null;
  const hasParts = parts !== null && parts.length > 0;

  function startTimer() {
    startRef.current = performance.now();
    setElapsedMs(0);
    setTiming(true);
    if (timerRef.current !== null) window.clearInterval(timerRef.current);
    timerRef.current = window.setInterval(() => {
      setElapsedMs(performance.now() - startRef.current);
    }, 100);
  }

  function stopTimer() {
    if (timerRef.current !== null) {
      window.clearInterval(timerRef.current);
      timerRef.current = null;
    }
    setTiming(false);
  }

  function validateParams(): {
    errors: string[];
    values: { plateLength: number; plateWidth: number; count: number; edgeDistance: number } | null;
  } {
    const errors: string[] = [];
    const plateLength = parseInput(plateLengthInput);
    const plateWidth = parseInput(plateWidthInput);
    const count = parseInput(countInput);
    const edgeDistance = parseInput(edgeInput);

    if (!Number.isFinite(plateLength) || plateLength <= 0 || plateLength > 100000) {
      errors.push(s.errPlateLength);
    }
    if (!Number.isFinite(plateWidth) || plateWidth <= 0 || plateWidth > 100000) {
      errors.push(s.errPlateWidth);
    }
    if (!Number.isInteger(count) || count < 1 || count > 100000) {
      errors.push(s.errPartCount);
    }
    if (!Number.isFinite(edgeDistance) || edgeDistance < 0) {
      errors.push(s.errEdgeNaN);
    }
    if (
      errors.length === 0 &&
      2 * edgeDistance >= Math.min(plateLength, plateWidth)
    ) {
      errors.push(s.errEdgeTooLarge);
    }
    if (errors.length > 0) return { errors, values: null };
    return { errors, values: { plateLength, plateWidth, count, edgeDistance } };
  }

  async function handleGenerate() {
    setApiError(null);
    setImportInfo(null);
    const { errors, values } = validateParams();
    setParamErrors(errors);
    if (!values) return;

    try {
      const data = await apiPost<unknown>("/api/parts/generate", values, s);
      const generated = normalizeGenerateResponse(data);
      if (generated.length === 0) {
        setApiError(s.errNoPartsFromBackend);
        return;
      }
      setParts(generated);
      setPlate({
        plateLength: values.plateLength,
        plateWidth: values.plateWidth,
        edgeDistance: values.edgeDistance,
      });
      setResult(null);
      setClientSeconds(null);
      setElapsedMs(0);
      setImportWarnings([]);
      setPageInput("1");
      setPageError(null);
    } catch (e) {
      setApiError(e instanceof Error ? e.message : s.errApiGeneric);
    }
  }

  function handleExportParts() {
    if (!parts || parts.length === 0) return;
    downloadTextFile("parts.csv", partsToCsv(parts), true);
  }

  async function handleImportFile(file: File) {
    setApiError(null);
    setImportInfo(null);
    if (!plate) {
      setImportWarnings([]);
      setImportInfo(s.importNeedPlate);
      return;
    }
    const text = await file.text();
    const { rows, warnings } = parsePartsCsv(text);

    const accepted: Part[] = [];
    const rejected: ImportWarning[] = [];
    rows.forEach((r, i) => {
      if (r.length + 2 * plate.edgeDistance <= plate.plateLength &&
          r.width + 2 * plate.edgeDistance <= plate.plateWidth) {
        accepted.push({ id: accepted.length + 1, length: r.length, width: r.width });
      } else {
        rejected.push({
          line: i + 1,
          length: r.length,
          width: r.width,
          reason: s.importRejectReason(plate.plateLength, plate.plateWidth),
        });
      }
    });

    const parseWarnings: ImportWarning[] = warnings.map((w) => {
      let reason: string;
      if (w.kind === "tooFewColumns") reason = s.importWarnTooFew(w.line);
      else if (w.kind === "notNumeric" && w.values)
        reason = s.importWarnNotNumeric(w.line, String(w.values[0]), String(w.values[1]));
      else if (w.values)
        reason = s.importWarnNotPositive(w.line, Number(w.values[0]), Number(w.values[1]));
      else reason = "";
      return { line: w.line, length: 0, width: 0, reason };
    });

    setImportWarnings([...parseWarnings, ...rejected]);
    if (accepted.length === 0) {
      setImportInfo(s.importEmpty);
      return;
    }
    setParts(accepted);
    setResult(null);
    setClientSeconds(null);
    setElapsedMs(0);
    setPageInput("1");
    setImportInfo(s.importSuccess(accepted.length, rejected.length));
  }

  async function handleRun() {
    if (!canRun || !plate || !parts) return;
    setApiError(null);
    setRunning(true);
    startTimer();
    try {
      const data = await apiPost<unknown>(
        "/api/nesting/run",
        {
          plateLength: plate.plateLength,
          plateWidth: plate.plateWidth,
          edgeDistance: plate.edgeDistance,
          parts,
        },
        s
      );
      setClientSeconds((performance.now() - startRef.current) / 1000);
      setResult(normalizeRunResponse(data));
      setPageInput("1");
      setPageError(null);
    } catch (e) {
      setApiError(e instanceof Error ? e.message : s.errRun);
    } finally {
      stopTimer();
      setRunning(false);
    }
  }

  function handleJump() {
    if (!result || sheetCount === 0) return;
    const n = Number(pageInput);
    if (!Number.isInteger(n) || n < 1 || n > sheetCount) {
      setPageError(s.pageError(sheetCount));
    } else {
      setPageError(null);
    }
    setPageInput(String(Math.min(Math.max(1, Number.isInteger(n) ? n : 1), sheetCount)));
  }

  async function handleExport(kind: ExportKind) {
    if (!result || !plate || result.sheets.length === 0) return;
    setExporting(kind);
    setApiError(null);
    try {
      await exportSheetsZip(kind, result, plate.plateLength, plate.plateWidth);
    } catch (e) {
      setApiError(e instanceof Error ? e.message : s.errExport);
    } finally {
      setExporting(null);
    }
  }

  const sheetRemainingArea = useMemo(() => {
    if (!currentSheet || !plate) return null;
    const total = plate.plateLength * plate.plateWidth;
    const used = currentSheet.parts.reduce((acc, p) => acc + p.length * p.width, 0);
    return total - used;
  }, [currentSheet, plate]);

  const fontScale = plate ? Math.min(plate.plateLength, plate.plateWidth) / 24 : 1;

  return (
    <div className="page">
      <header className="page-header">
        <div>
          <h1>{s.appTitle}</h1>
          <p>{s.appSubtitle}</p>
        </div>
        <div className="header-actions">
          <Link href="/guide" className="btn btn-sm btn-guide">
            {s.btnGuide}
          </Link>
          <div className="lang-switch" role="group" aria-label="Language / 语言">
          <button
            className={locale === "zh" ? "active" : ""}
            onClick={() => setLocale("zh")}
          >
            {s.langZh}
          </button>
          <button
            className={locale === "en" ? "active" : ""}
            onClick={() => setLocale("en")}
          >
            {s.langEn}
          </button>
          </div>
        </div>
      </header>

      {(apiError || paramErrors.length > 0 || importInfo) && (
        <div className="alert-stack">
          {apiError && (
            <div className="alert alert-error" role="alert">
              <strong>{s.errorPrefix}</strong>
              {apiError}
            </div>
          )}
          {paramErrors.length > 0 && (
            <div className="alert alert-error" role="alert">
              <strong>{s.paramCheckFailed}</strong>
              <ul>
                {paramErrors.map((e, i) => (
                  <li key={i}>{e}</li>
                ))}
              </ul>
            </div>
          )}
          {importInfo && <div className="alert alert-info">{importInfo}</div>}
        </div>
      )}

      <div className="layout">
        {/* ===================== 左栏 ===================== */}
        <aside className="sidebar">
          <section className="card">
            <h2>{s.params}</h2>
            <div className="form-grid">
              <label>
                <span>{s.labelPlateLength}</span>
                <input
                  type="number"
                  value={plateLengthInput}
                  disabled={!!plate}
                  onChange={(e) => setPlateLengthInput(e.target.value)}
                />
              </label>
              <label>
                <span>{s.labelPlateWidth}</span>
                <input
                  type="number"
                  value={plateWidthInput}
                  disabled={!!plate}
                  onChange={(e) => setPlateWidthInput(e.target.value)}
                />
              </label>
              <label>
                <span>{s.labelPartCount}</span>
                <input
                  type="number"
                  value={countInput}
                  onChange={(e) => setCountInput(e.target.value)}
                />
              </label>
              <label>
                <span>{s.labelEdgeDistance}</span>
                <input
                  type="number"
                  value={edgeInput}
                  onChange={(e) => {
                    setEdgeInput(e.target.value);
                    const v = Number(e.target.value);
                    if (plate && Number.isFinite(v) && v >= 0) {
                      setPlate({ ...plate, edgeDistance: v });
                    }
                  }}
                />
              </label>
            </div>
            <p className="hint">
              {plate
                ? s.hintLocked(plate.plateLength, plate.plateWidth, plate.edgeDistance)
                : s.hintUnlocked}
            </p>
          </section>

          <section className="card">
            <h2>{s.actions}</h2>
            <div className="button-stack">
              <button className="btn btn-primary" onClick={handleGenerate} disabled={!canGenerate}>
                {running ? s.btnGenerating : s.btnGenerate}
              </button>
              <button
                className={`btn btn-export${hasParts ? " ready" : ""}`}
                onClick={handleExportParts}
                disabled={!hasParts}
              >
                {s.btnExportParts}
              </button>

              <div
                className={`dropzone${dragActive ? " active" : ""}${running ? " disabled" : ""}`}
                role="button"
                tabIndex={running ? -1 : 0}
                onClick={() => !running && fileInputRef.current?.click()}
                onKeyDown={(e) => {
                  if (!running && (e.key === "Enter" || e.key === " ")) {
                    e.preventDefault();
                    fileInputRef.current?.click();
                  }
                }}
                onDragOver={(e) => {
                  e.preventDefault();
                  if (!running) setDragActive(true);
                }}
                onDragLeave={() => setDragActive(false)}
                onDrop={(e) => {
                  e.preventDefault();
                  setDragActive(false);
                  if (running) return;
                  const f = e.dataTransfer.files?.[0];
                  if (f) handleImportFile(f);
                }}
              >
                <input
                  ref={fileInputRef}
                  type="file"
                  accept=".csv,text/csv"
                  hidden
                  onChange={(e) => {
                    const f = e.target.files?.[0];
                    if (f) handleImportFile(f);
                    e.target.value = "";
                  }}
                />
                <div className="dropzone-icon" aria-hidden>
                  ⭱
                </div>
                <div className="dropzone-text">{s.importBoxTitle}</div>
                <div className="dropzone-hint">
                  {dragActive ? s.importBoxActive : s.importBoxHint}
                </div>
              </div>

              <button className="btn btn-accent" onClick={handleRun} disabled={!canRun}>
                {running ? s.btnRunning : s.btnRun}
              </button>
            </div>
          </section>

          {parts && (
            <section className="card">
              <h2>{s.partsList}</h2>
              <p className="hint">{s.partsPreview(parts.length, MAX_PREVIEW_ROWS)}</p>
              <div className="table-scroll">
                <table className="data-table">
                  <thead>
                    <tr>
                      <th>{s.colIndex}</th>
                      <th>{s.colLength}</th>
                      <th>{s.colWidth}</th>
                    </tr>
                  </thead>
                  <tbody>
                    {parts.slice(0, MAX_PREVIEW_ROWS).map((p) => (
                      <tr key={p.id}>
                        <td>{p.id}</td>
                        <td>{p.length}</td>
                        <td>{p.width}</td>
                      </tr>
                    ))}
                  </tbody>
                </table>
              </div>
            </section>
          )}

          {importWarnings.length > 0 && (
            <section className="card card-warning">
              <h2>{s.importWarnings(importWarnings.length)}</h2>
              <div className="table-scroll">
                <table className="data-table">
                  <thead>
                    <tr>
                      <th>{s.colLine}</th>
                      <th>{s.colLength}</th>
                      <th>{s.colWidth}</th>
                      <th>{s.colReason}</th>
                    </tr>
                  </thead>
                  <tbody>
                    {importWarnings.map((w, i) => (
                      <tr key={i}>
                        <td>{w.line || "-"}</td>
                        <td>{w.length || "-"}</td>
                        <td>{w.width || "-"}</td>
                        <td className="reason">{w.reason}</td>
                      </tr>
                    ))}
                  </tbody>
                </table>
              </div>
            </section>
          )}

          <section className="card card-timer">
            <h2>{s.timerTitle}</h2>
            <div className={`timer-display ${timing ? "timer-active" : ""}`}>
              {formatTimer(elapsedMs)}
            </div>
            <p className="hint">{timing ? s.timerRunning : s.timerIdle}</p>
          </section>

          {result && (
            <section className="card">
              <h2>{s.perfTitle}</h2>
              <table className="perf-table">
                <tbody>
                  <tr>
                    <td>{s.perfServerTime}</td>
                    <td>
                      {result.runtimeMs.toFixed(1)} {s.unitMs}
                    </td>
                  </tr>
                  <tr>
                    <td>{s.perfClientTime}</td>
                    <td>
                      {clientSeconds !== null
                        ? `${clientSeconds.toFixed(2)} ${s.unitSec}`
                        : "-"}
                    </td>
                  </tr>
                  <tr>
                    <td>{s.perfSheets}</td>
                    <td>{result.sheetCount}</td>
                  </tr>
                  <tr>
                    <td>{s.perfUtilization}</td>
                    <td className="highlight">
                      {Number.isFinite(result.utilization)
                        ? `${result.utilization.toFixed(2)}%`
                        : "-"}
                    </td>
                  </tr>
                  <tr>
                    <td>{s.perfTotalParts}</td>
                    <td>{result.totalParts}</td>
                  </tr>
                  <tr>
                    <td>{s.perfPlaced}</td>
                    <td>{result.placedCount}</td>
                  </tr>
                  <tr>
                    <td>{s.perfUnplaced}</td>
                    <td className={result.unplacedCount > 0 ? "warn-text" : ""}>
                      {result.unplacedCount}
                    </td>
                  </tr>
                  {result.remainingLength !== null && Number.isFinite(result.remainingLength) && (
                    <tr>
                      <td>{s.perfRemaining}</td>
                      <td>{result.remainingLength}</td>
                    </tr>
                  )}
                </tbody>
              </table>
              {result.unplaced.length > 0 && (
                <>
                  <h3 className="sub-title">{s.unplacedTitle}</h3>
                  <div className="table-scroll">
                    <table className="data-table">
                      <thead>
                        <tr>
                          <th>id</th>
                          <th>{s.colLength}</th>
                          <th>{s.colWidth}</th>
                        </tr>
                      </thead>
                      <tbody>
                        {result.unplaced.map((p) => (
                          <tr key={p.id}>
                            <td>{p.id}</td>
                            <td>{p.length}</td>
                            <td>{p.width}</td>
                          </tr>
                        ))}
                      </tbody>
                    </table>
                  </div>
                </>
              )}
            </section>
          )}
        </aside>

        {/* ===================== 右栏 ===================== */}
        <main className="viewer card">
          <h2>{s.viewerTitle}</h2>
          {!result || !plate ? (
            <div className="placeholder">
              <div className="placeholder-icon">▦</div>
              <p>{s.placeholderTitle}</p>
              <p className="hint">{s.placeholderHint}</p>
            </div>
          ) : (
            <>
              <div className="sheet-header">
                <span className="sheet-title">{s.sheetTitle(currentPage, sheetCount)}</span>
              </div>
              <div className="svg-wrap">
                {currentSheet && (
                  <svg
                    viewBox={`0 0 ${plate.plateLength} ${plate.plateWidth}`}
                    preserveAspectRatio="xMidYMid meet"
                    role="img"
                  >
                    <rect
                      x={0}
                      y={0}
                      width={plate.plateLength}
                      height={plate.plateWidth}
                      className="plate-rect"
                    />
                    {currentSheet.parts.map((p) => (
                      <g key={p.partId}>
                        <rect
                          x={p.x}
                          y={plate.plateWidth - p.y - p.width}
                          width={p.length}
                          height={p.width}
                          className="part-rect"
                        />
                        {p.length > fontScale * 2.2 && p.width > fontScale * 1.4 && (
                          <text
                            x={p.x + p.length / 2}
                            y={plate.plateWidth - p.y - p.width / 2}
                            fontSize={fontScale * 0.9}
                            className="part-label"
                          >
                            {p.partId}
                          </text>
                        )}
                      </g>
                    ))}
                  </svg>
                )}
              </div>
              <div className="sheet-info">
                <span>
                  {s.sheetUtilization}
                  <strong>
                    {Number.isFinite(currentSheet?.utilization)
                      ? `${currentSheet!.utilization.toFixed(2)}%`
                      : "-"}
                  </strong>
                </span>
                <span>
                  {s.sheetPartCount}
                  <strong>{currentSheet?.parts.length ?? 0}</strong>
                </span>
                <span>
                  {s.sheetRemainingArea}
                  <strong>
                    {sheetRemainingArea !== null ? sheetRemainingArea.toFixed(2) : "-"}
                  </strong>
                </span>
              </div>

              <div className="pager">
                <button
                  className="btn btn-sm"
                  disabled={currentPage <= 1}
                  onClick={() => setPageInput(String(currentPage - 1))}
                >
                  {s.prevPage}
                </button>
                <span className="pager-status">
                  {s.pagerStatus(currentPage, sheetCount)}
                </span>
                <button
                  className="btn btn-sm"
                  disabled={currentPage >= sheetCount}
                  onClick={() => setPageInput(String(currentPage + 1))}
                >
                  {s.nextPage}
                </button>
                <span className="pager-jump">
                  <input
                    type="number"
                    min={1}
                    max={sheetCount}
                    value={pageInput}
                    onChange={(e) => setPageInput(e.target.value)}
                    onKeyDown={(e) => e.key === "Enter" && handleJump()}
                  />
                  <button className="btn btn-sm" onClick={handleJump}>
                    {s.jump}
                  </button>
                </span>
                {pageError && <span className="warn-text">{pageError}</span>}
              </div>

              <div className="export-zone">
                <h3 className="sub-title">{s.exportTitle}</h3>
                <div className="button-row">
                  <button
                    className="btn"
                    disabled={exporting !== null}
                    onClick={() => handleExport("csv")}
                  >
                    {exporting === "csv" ? s.exporting : s.exportCsv}
                  </button>
                  <button
                    className="btn"
                    disabled={exporting !== null}
                    onClick={() => handleExport("dxf")}
                  >
                    {exporting === "dxf" ? s.exporting : s.exportDxf}
                  </button>
                  <button
                    className="btn"
                    disabled={exporting !== null}
                    onClick={() => handleExport("png")}
                  >
                    {exporting === "png" ? s.exporting : s.exportPng}
                  </button>
                </div>
              </div>
            </>
          )}
        </main>
      </div>
    </div>
  );
}
