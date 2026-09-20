import type { Sheet } from "./types";

function pair(code: number, value: string | number): string {
  return `${code}\n${value}\n`;
}

function lineEntity(x1: number, y1: number, x2: number, y2: number, layer: string): string {
  return (
    pair(0, "LINE") +
    pair(8, layer) +
    pair(10, x1) +
    pair(20, y1) +
    pair(11, x2) +
    pair(21, y2)
  );
}

function rectEntities(
  x: number,
  y: number,
  length: number,
  width: number,
  layer: string
): string {
  const x2 = x + length;
  const y2 = y + width;
  return (
    lineEntity(x, y, x2, y, layer) +
    lineEntity(x2, y, x2, y2, layer) +
    lineEntity(x2, y2, x, y2, layer) +
    lineEntity(x, y2, x, y, layer)
  );
}

export function sheetToDxf(
  sheet: Sheet,
  plateLength: number,
  plateWidth: number
): string {
  const header =
    pair(0, "SECTION") +
    pair(2, "HEADER") +
    pair(9, "$ACADVER") +
    pair(1, "AC1009") +
    pair(0, "ENDSEC");

  const tables =
    pair(0, "SECTION") +
    pair(2, "TABLES") +
    pair(0, "TABLE") +
    pair(2, "LAYER") +
    pair(70, 2) +
    // PLATE 图层：白色
    pair(0, "LAYER") +
    pair(2, "PLATE") +
    pair(70, 0) +
    pair(62, 7) +
    pair(6, "CONTINUOUS") +
    // PARTS 图层：蓝色
    pair(0, "LAYER") +
    pair(2, "PARTS") +
    pair(70, 0) +
    pair(62, 5) +
    pair(6, "CONTINUOUS") +
    pair(0, "ENDTAB") +
    pair(0, "ENDSEC");

  const blocks = pair(0, "SECTION") + pair(2, "BLOCKS") + pair(0, "ENDSEC");

  let entities = "";
  // 板框
  entities += rectEntities(0, 0, plateLength, plateWidth, "PLATE");
  // 左上角 TEXT：利用率
  const textHeight = Math.min(plateLength, plateWidth) / 30;
  entities +=
    pair(0, "TEXT") +
    pair(8, "PLATE") +
    pair(10, plateLength * 0.02) +
    pair(20, plateWidth - textHeight * 1.5) +
    pair(40, textHeight) +
    pair(1, `Utilization: ${sheet.utilization.toFixed(2)}%`);
  // 零件
  for (const p of sheet.parts) {
    entities += rectEntities(p.x, p.y, p.length, p.width, "PARTS");
  }

  return (
    header +
    tables +
    blocks +
    pair(0, "SECTION") +
    pair(2, "ENTITIES") +
    entities +
    pair(0, "ENDSEC") +
    pair(0, "EOF")
  );
}
