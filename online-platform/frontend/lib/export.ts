import JSZip from "jszip";
import type { NestingResult } from "./types";
import { sheetToDxf } from "./dxf";
import { renderSheetToCanvas, canvasToPngBlob } from "./png";

export type ExportKind = "csv" | "dxf" | "png";

function sheetToCsv(sheet: NestingResult["sheets"][number]): string {
  const rows = sheet.parts.map(
    (p) => `${sheet.sheetNo},${p.partId},${p.x},${p.y},${p.length},${p.width}`
  );
  return "sheetNo,partId,x,y,length,width\n" + rows.join("\n") + "\n";
}

export async function exportSheetsZip(
  kind: ExportKind,
  result: NestingResult,
  plateLength: number,
  plateWidth: number
): Promise<void> {
  const zip = new JSZip();
  const ext = kind === "csv" ? "csv" : kind === "dxf" ? "dxf" : "png";

  for (const sheet of result.sheets) {
    const name = `Sheet${sheet.sheetNo}.${ext}`;
    if (kind === "csv") {
      zip.file(name, sheetToCsv(sheet));
    } else if (kind === "dxf") {
      zip.file(name, sheetToDxf(sheet, plateLength, plateWidth));
    } else {
      const canvas = renderSheetToCanvas(sheet, plateLength, plateWidth);
      const blob = await canvasToPngBlob(canvas);
      zip.file(name, blob);
    }
  }

  const blob = await zip.generateAsync({ type: "blob" });
  const url = URL.createObjectURL(blob);
  const a = document.createElement("a");
  a.href = url;
  a.download = `sheets-${kind}.zip`;
  a.click();
  URL.revokeObjectURL(url);
}
