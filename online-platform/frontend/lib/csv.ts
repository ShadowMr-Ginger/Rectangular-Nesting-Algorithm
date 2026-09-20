import type { Part } from "./types";

export function partsToCsv(parts: Part[]): string {
  const rows = parts.map((p) => `${p.id},${p.length},${p.width}`);
  return "id,length,width\n" + rows.join("\n") + "\n";
}

export function downloadTextFile(filename: string, content: string, bom = false): void {
  const blob = new Blob([bom ? "\uFEFF" + content : content], {
    type: "text/plain;charset=utf-8",
  });
  const url = URL.createObjectURL(blob);
  const a = document.createElement("a");
  a.href = url;
  a.download = filename;
  a.click();
  URL.revokeObjectURL(url);
}

function stripQuotes(field: string): string {
  const t = field.trim();
  if (t.length >= 2 && t.startsWith('"') && t.endsWith('"')) {
    return t.slice(1, -1).trim();
  }
  return t;
}

export type ParseWarningKind = "tooFewColumns" | "notNumeric" | "notPositive";

export interface ParseWarning {
  line: number;
  kind: ParseWarningKind;
  values: [string, string] | [number, number] | null;
}

export interface ParsedImport {
  rows: Array<{ length: number; width: number }>;
  warnings: ParseWarning[];
}

export function parsePartsCsv(text: string): ParsedImport {
  const warnings: ParseWarning[] = [];
  const rows: Array<{ length: number; width: number }> = [];
  const lines = text.split(/\r?\n/);

  for (let i = 0; i < lines.length; i++) {
    const raw = lines[i].trim();
    if (!raw) continue;
    const fields = raw.split(/[,，;]/).map(stripQuotes);
    if (fields.length < 2) {
      warnings.push({ line: i + 1, kind: "tooFewColumns", values: null });
      continue;
    }
    // 兼容表头行：前两个字段不是数字则视为表头
    const cols = fields.length >= 3 ? fields.slice(-2) : fields.slice(0, 2);
    const length = Number(cols[0]);
    const width = Number(cols[1]);
    if (!Number.isFinite(length) || !Number.isFinite(width)) {
      if (rows.length === 0 && warnings.length === 0) {
        continue; // 表头行
      }
      warnings.push({
        line: i + 1,
        kind: "notNumeric",
        values: [cols[0], cols[1]],
      });
      continue;
    }
    if (length <= 0 || width <= 0) {
      warnings.push({ line: i + 1, kind: "notPositive", values: [length, width] });
      continue;
    }
    rows.push({ length, width });
  }

  return { rows, warnings };
}
