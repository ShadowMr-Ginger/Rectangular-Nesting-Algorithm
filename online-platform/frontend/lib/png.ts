import type { Sheet } from "./types";

export const MAX_CANVAS_SIZE = 1600;

export function renderSheetToCanvas(
  sheet: Sheet,
  plateLength: number,
  plateWidth: number
): HTMLCanvasElement {
  const scale = MAX_CANVAS_SIZE / Math.max(plateLength, plateWidth);
  const pad = Math.max(8, Math.min(plateLength, plateWidth) * scale * 0.03);
  const w = Math.max(1, Math.round(plateLength * scale + pad * 2));
  const h = Math.max(1, Math.round(plateWidth * scale + pad * 2));

  const canvas = document.createElement("canvas");
  canvas.width = w;
  canvas.height = h;
  const ctx = canvas.getContext("2d");
  if (!ctx) return canvas;

  // 白底
  ctx.fillStyle = "#ffffff";
  ctx.fillRect(0, 0, w, h);

  const ox = pad;
  const oy = pad;

  // 板框
  ctx.strokeStyle = "#1f2937";
  ctx.lineWidth = Math.max(2, scale * 0.6);
  ctx.strokeRect(ox, oy, plateLength * scale, plateWidth * scale);

  // 零件
  ctx.strokeStyle = "#2563eb";
  ctx.lineWidth = Math.max(1, scale * 0.25);
  ctx.fillStyle = "rgba(37, 99, 235, 0.08)";
  const fontSize = Math.max(8, Math.min(plateLength, plateWidth) * scale * 0.07);
  ctx.font = `${fontSize}px "Segoe UI", "Microsoft YaHei", sans-serif`;
  ctx.fillStyle = "#1e40af";
  ctx.textAlign = "center";
  ctx.textBaseline = "middle";

  for (const p of sheet.parts) {
    const px = ox + p.x * scale;
    // 算法 y 从左下角起算（y 向上），Canvas/SVG 的 y 从顶部向下，需翻转
    const py = oy + (plateWidth - p.y - p.width) * scale;
    const pw = p.length * scale;
    const ph = p.width * scale;
    ctx.fillRect(px, py, pw, ph);
    ctx.strokeRect(px, py, pw, ph);
    if (pw > fontSize * 1.6 && ph > fontSize * 1.2) {
      ctx.fillText(String(p.partId), px + pw / 2, py + ph / 2);
    }
  }

  return canvas;
}

export function canvasToPngBlob(canvas: HTMLCanvasElement): Promise<Blob> {
  return new Promise((resolve, reject) => {
    canvas.toBlob((blob) => {
      if (blob) resolve(blob);
      else reject(new Error("Canvas 导出 PNG 失败。"));
    }, "image/png");
  });
}
