import type { NestingResult, Sheet, SheetPart, UnplacedPart } from "./types";

function num(v: unknown, fallback: number): number {
  const n = Number(v);
  return Number.isFinite(n) ? n : fallback;
}

function strId(v: unknown, index: number): number {
  const n = Number(v);
  return Number.isFinite(n) ? n : index + 1;
}

// 后端成材率返回 0~1 小数，前端统一展示为 0~100 百分数
function toPercent(v: unknown): number {
  const n = Number(v);
  if (!Number.isFinite(n)) return NaN;
  return n > 0 && n <= 1 ? n * 100 : n;
}

export function normalizeGenerateResponse(data: unknown): { id: number; length: number; width: number }[] {
  const raw = Array.isArray(data)
    ? data
    : data && typeof data === "object"
      ? (data as Record<string, unknown>).parts
      : null;
  if (!Array.isArray(raw)) return [];
  return raw.map((item, i) => {
    const o = (item ?? {}) as Record<string, unknown>;
    return {
      id: strId(o.id ?? o.partId, i),
      length: num(o.length, 0),
      width: num(o.width, 0),
    };
  });
}

export function normalizeRunResponse(data: unknown): NestingResult {
  const o = (data ?? {}) as Record<string, unknown>;

  const rawSheets = Array.isArray(o.sheets) ? o.sheets : [];
  const sheets: Sheet[] = rawSheets.map((s, i) => {
    const so = (s ?? {}) as Record<string, unknown>;
    const rawParts = Array.isArray(so.parts)
      ? so.parts
      : Array.isArray(so.placed)
        ? so.placed
        : [];
    const parts: SheetPart[] = rawParts.map((p, j) => {
      const po = (p ?? {}) as Record<string, unknown>;
      return {
        partId: strId(po.partId ?? po.id, j),
        x: num(po.x, 0),
        y: num(po.y, 0),
        length: num(po.length, 0),
        width: num(po.width, 0),
      };
    });
    return {
      sheetNo: num(so.sheetNo, i + 1),
      utilization: toPercent(so.utilization),
      surplusLength: num(so.surplusLength, 0),
      surplusWidth: num(so.surplusWidth, 0),
      parts,
    };
  });

  const rawUnplaced = Array.isArray(o.unplaced) ? o.unplaced : [];
  const unplaced: UnplacedPart[] = rawUnplaced.map((p, i) => {
    const po = (p ?? {}) as Record<string, unknown>;
    return {
      id: strId(po.id ?? po.partId, i),
      length: num(po.length, 0),
      width: num(po.width, 0),
    };
  });

  const placedCount = num(o.placedCount ?? o.placed ?? o.partsNestedCount, 0);
  const totalParts = num(o.totalParts ?? o.partsTotal, sheets.reduce((acc, s) => acc + s.parts.length, 0) + unplaced.length);
  const sheetCount = num(o.sheetCount ?? o.plateCount ?? o.numPlates, sheets.length);

  return {
    runtimeMs: num(o.runtimeMs ?? o.elapsedMs, 0),
    utilization: toPercent(o.utilization),
    sheetCount,
    totalParts,
    placedCount: placedCount || sheets.reduce((acc, s) => acc + s.parts.length, 0),
    unplacedCount: unplaced.length || num(o.unplacedCount, 0),
    unplaced,
    remainingLength:
      o.remainingLength ?? o.remainingLengthOfLastSheet ?? o.lastPlateSurplusLength != null
        ? num(o.remainingLength ?? o.remainingLengthOfLastSheet ?? o.lastPlateSurplusLength, NaN)
        : null,
    sheets,
  };
}
