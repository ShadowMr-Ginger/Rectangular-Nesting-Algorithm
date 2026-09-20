"use client";

import { useEffect, useState } from "react";
import Link from "next/link";
import { getStrings, initialLocale, saveLocale, type Locale } from "@/lib/i18n";
import { getGuideContent } from "@/lib/guide";

/** 参数与边缘距离示意图：蓝框=原板，灰带=边缘距离，橙块=零件，左下角为原点 */
function PlateDiagram() {
  return (
    <svg viewBox="0 0 150 86" className="guide-svg" role="img" aria-label="plate diagram">
      {/* 边缘距离示意（ exaggerated ） */}
      <rect x="8" y="6" width="132" height="62" fill="none" stroke="#94a3b8" strokeWidth="7" opacity="0.45" />
      {/* 原板边界 */}
      <rect x="4" y="2" width="140" height="70" fill="#f8fafc" stroke="#2563eb" strokeWidth="1.5" />
      {/* 零件 */}
      <rect x="14" y="12" width="44" height="20" className="guide-part" />
      <rect x="62" y="12" width="28" height="20" className="guide-part" />
      <rect x="14" y="36" width="30" height="28" className="guide-part" />
      <rect x="48" y="36" width="54" height="14" className="guide-part" />
      <text x="36" y="24" className="guide-svg-label">1</text>
      <text x="76" y="24" className="guide-svg-label">2</text>
      <text x="29" y="53" className="guide-svg-label">3</text>
      <text x="75" y="45" className="guide-svg-label">4</text>
      {/* 坐标轴 */}
      <line x1="4" y1="78" x2="144" y2="78" stroke="#334155" strokeWidth="1" markerEnd="url(#arrowGuide)" />
      <line x1="4" y1="78" x2="4" y2="4" stroke="#334155" strokeWidth="1" markerEnd="url(#arrowGuideUp)" />
      <text x="146" y="81" className="guide-svg-axis">X</text>
      <text x="7" y="12" className="guide-svg-axis">Y</text>
      <text x="7" y="76" className="guide-svg-axis">O</text>
      <defs>
        <marker id="arrowGuide" markerWidth="8" markerHeight="8" refX="6" refY="3" orient="auto">
          <path d="M0,0 L6,3 L0,6 Z" fill="#334155" />
        </marker>
        <marker id="arrowGuideUp" markerWidth="8" markerHeight="8" refX="6" refY="3" orient="auto">
          <path d="M0,0 L6,3 L0,6 Z" fill="#334155" />
        </marker>
      </defs>
    </svg>
  );
}

/** 末板一刀切示意图：虚线为切割线，右侧灰色为有效余料 */
function LastPlateDiagram() {
  return (
    <svg viewBox="0 0 150 78" className="guide-svg" role="img" aria-label="last plate one-cut diagram">
      <rect x="4" y="2" width="140" height="70" fill="#f8fafc" stroke="#2563eb" strokeWidth="1.5" />
      {/* 零件堆在左侧 */}
      <rect x="12" y="10" width="36" height="24" className="guide-part" />
      <rect x="52" y="10" width="22" height="24" className="guide-part" />
      <rect x="12" y="38" width="26" height="28" className="guide-part" />
      <rect x="42" y="38" width="40" height="16" className="guide-part" />
      {/* 切割线 */}
      <line x1="88" y1="2" x2="88" y2="72" stroke="#dc2626" strokeWidth="1.5" strokeDasharray="5 4" />
      {/* 余料区 */}
      <rect x="88" y="2" width="56" height="70" fill="#64748b" opacity="0.18" />
      <text x="100" y="38" className="guide-svg-axis" fill="#475569">⇠</text>
    </svg>
  );
}

export default function GuidePage() {
  const [locale, setLocaleState] = useState<Locale>(initialLocale);
  const setLocale = (l: Locale) => {
    saveLocale(l);
    setLocaleState(l);
  };
  const s = getStrings(locale);
  const g = getGuideContent(locale);

  useEffect(() => {
    document.documentElement.lang = locale === "zh" ? "zh-CN" : "en";
  }, [locale]);

  return (
    <div className="page guide-page">
      <header className="page-header">
        <div>
          <h1>{g.title}</h1>
          <p>{s.appTitle}</p>
        </div>
        <div className="header-actions">
          <Link href="/" className="btn btn-sm btn-guide">
            {g.back}
          </Link>
          <div className="lang-switch" role="group" aria-label="Language / 语言">
            <button className={locale === "zh" ? "active" : ""} onClick={() => setLocale("zh")}>
              {s.langZh}
            </button>
            <button className={locale === "en" ? "active" : ""} onClick={() => setLocale("en")}>
              {s.langEn}
            </button>
          </div>
        </div>
      </header>

      <main className="guide-body">
        <section className="guide-section">
          <h2>{g.introTitle}</h2>
          <p>{g.introText}</p>
        </section>

        <section className="guide-section">
          <h2>{g.workflowTitle}</h2>
          <ol className="guide-steps">
            {g.steps.map((st) => (
              <li key={st.title}>
                <h3>{st.title}</h3>
                <p>{st.text}</p>
              </li>
            ))}
          </ol>
        </section>

        <section className="guide-section">
          <h2>{g.paramsTitle}</h2>
          <table className="guide-table">
            <tbody>
              {g.params.map((p) => (
                <tr key={p.name}>
                  <th scope="row">{p.name}</th>
                  <td>{p.desc}</td>
                </tr>
              ))}
            </tbody>
          </table>
          <h3 className="guide-sub">{g.derivedNoteTitle}</h3>
          <p>{g.derivedNote}</p>
        </section>

        <section className="guide-section">
          <h2>{g.csvTitle}</h2>
          <p>{g.csvIntro}</p>
          <pre className="guide-code">{`id,length,width
1,12.5,6.0
2,8.0,4.25
3,15.0,3.5`}</pre>
          <h3 className="guide-sub">{g.csvColumnsTitle}</h3>
          <table className="guide-table">
            <tbody>
              {g.csvColumns.map((c) => (
                <tr key={c.name}>
                  <th scope="row">
                    <code>{c.name}</code>
                  </th>
                  <td>{c.desc}</td>
                </tr>
              ))}
            </tbody>
          </table>
          <ul className="guide-notes">
            {g.csvNotes.map((n) => (
              <li key={n}>{n}</li>
            ))}
          </ul>
        </section>

        <section className="guide-section">
          <h2>{g.layoutTitle}</h2>
          <p>{g.coordText}</p>
          <figure className="guide-figure">
            <PlateDiagram />
            <figcaption>{g.edgeText}</figcaption>
          </figure>
          <h3 className="guide-sub">{g.lastPlateTitle}</h3>
          <p>{g.lastPlateText}</p>
          <figure className="guide-figure">
            <LastPlateDiagram />
          </figure>
          <h3 className="guide-sub">{g.viewerTitle}</h3>
          <p>{g.viewerText}</p>
          <h3 className="guide-sub">{g.exportTitle}</h3>
          <p>{g.exportText}</p>
        </section>
      </main>
    </div>
  );
}
