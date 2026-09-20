export type Locale = "zh" | "en";

/** 默认跟随浏览器：中文（zh*）→ 中文，其余语言 → 英文 */
export function detectLocale(): Locale {
  if (typeof navigator !== "undefined" && typeof navigator.language === "string") {
    return navigator.language.toLowerCase().startsWith("zh") ? "zh" : "en";
  }
  return "zh";
}

const LOCALE_STORAGE_KEY = "nesting-locale";

/** 初始化语言：优先使用用户在站点内手动选择并保存的语言，否则跟随浏览器 */
export function initialLocale(): Locale {
  if (typeof window !== "undefined") {
    const saved = window.localStorage.getItem(LOCALE_STORAGE_KEY);
    if (saved === "zh" || saved === "en") return saved;
  }
  return detectLocale();
}

/** 保存用户在站点内的语言选择（跨页面共享） */
export function saveLocale(locale: Locale): void {
  if (typeof window !== "undefined") {
    try {
      window.localStorage.setItem(LOCALE_STORAGE_KEY, locale);
    } catch {
      /* 存储不可用时忽略 */
    }
  }
}

export interface Strings {
  appTitle: string;
  appSubtitle: string;
  btnGuide: string;
  errorPrefix: string;
  paramCheckFailed: string;
  params: string;
  labelPlateLength: string;
  labelPlateWidth: string;
  labelPartCount: string;
  labelEdgeDistance: string;
  errPlateLength: string;
  errPlateWidth: string;
  errPartCount: string;
  errEdgeNaN: string;
  errEdgeTooLarge: string;
  hintLocked: (l: number, w: number, e: number) => string;
  hintUnlocked: string;
  actions: string;
  btnGenerate: string;
  btnGenerating: string;
  btnExportParts: string;
  btnRun: string;
  btnRunning: string;
  importBoxTitle: string;
  importBoxHint: string;
  importBoxActive: string;
  partsList: string;
  partsPreview: (total: number, max: number) => string;
  colIndex: string;
  colLength: string;
  colWidth: string;
  colLine: string;
  colReason: string;
  importWarnings: (n: number) => string;
  timerTitle: string;
  timerRunning: string;
  timerIdle: string;
  perfTitle: string;
  perfServerTime: string;
  perfClientTime: string;
  unitMs: string;
  unitSec: string;
  perfSheets: string;
  perfUtilization: string;
  perfTotalParts: string;
  perfPlaced: string;
  perfUnplaced: string;
  perfRemaining: string;
  unplacedTitle: string;
  viewerTitle: string;
  placeholderTitle: string;
  placeholderHint: string;
  sheetTitle: (cur: number, total: number) => string;
  sheetUtilization: string;
  sheetPartCount: string;
  sheetRemainingArea: string;
  sheetOffcutArea: string;
  surplusLabel: string;
  prevPage: string;
  nextPage: string;
  jump: string;
  pagerStatus: (cur: number, total: number) => string;
  pageError: (n: number) => string;
  exportTitle: string;
  exportCsv: string;
  exportDxf: string;
  exportPng: string;
  exporting: string;
  errApiGeneric: string;
  errNoPartsFromBackend: string;
  errRun: string;
  errExport: string;
  importNeedPlate: string;
  importRejectReason: (plateL: number, plateW: number) => string;
  importEmpty: string;
  importSuccess: (ok: number, skipped: number) => string;
  importWarnTooFew: (line: number) => string;
  importWarnNotNumeric: (line: number, a: string, b: string) => string;
  importWarnNotPositive: (line: number, a: number, b: number) => string;
  netErr: string;
  httpErr: (status: number) => string;
  langZh: string;
  langEn: string;
}

const zh: Strings = {
  appTitle: "矩形件套料算法 · 在线运算演示平台",
  appSubtitle: "生成矩形件 → 运行套料算法 → 查看方案 / 导出结果",
  btnGuide: "使用说明",
  errorPrefix: "错误：",
  paramCheckFailed: "参数校验未通过，请逐条修正：",
  params: "参数设置",
  labelPlateLength: "板材长",
  labelPlateWidth: "板材宽",
  labelPartCount: "矩形件数量",
  labelEdgeDistance: "边缘距离",
  errPlateLength: "板材长须为 0 < 长 ≤ 100000 的有限数字。",
  errPlateWidth: "板材宽须为 0 < 宽 ≤ 100000 的有限数字。",
  errPartCount: "矩形件数量须为 1 ~ 100000 的整数。",
  errEdgeNaN: "边缘距离须为 ≥ 0 的有限数字。",
  errEdgeTooLarge: "边缘距离不合理：须满足 2 × 边缘距离 < min(板材长, 板材宽)。",
  hintLocked: (l, w, e) =>
    `板材参数已锁定：${l} × ${w}，边距 ${e}。重新点击“生成随机矩形件”可更新。`,
  hintUnlocked: "点击“生成随机矩形件”后将锁定板材长/宽与边缘距离。",
  actions: "操作",
  btnGenerate: "生成随机矩形件",
  btnGenerating: "运算中…",
  btnExportParts: "导出零件列表",
  btnRun: "运行算法",
  btnRunning: "算法运算中…",
  importBoxTitle: "导入零件列表",
  importBoxHint: "点击选择文件，或将 CSV 文件拖拽到此处",
  importBoxActive: "松开鼠标以上传",
  partsList: "零件列表",
  partsPreview: (total, max) => `共 ${total} 件（最多预览前 ${max} 行）`,
  colIndex: "序号",
  colLength: "长",
  colWidth: "宽",
  colLine: "行号",
  colReason: "原因",
  importWarnings: (n) => `导入警告（${n}）`,
  timerTitle: "运算计时",
  timerRunning: "计时中（mm:ss.厘秒）…",
  timerIdle: "点击“运行算法”后开始计时",
  perfTitle: "性能结果",
  perfServerTime: "运算耗时（服务端）",
  perfClientTime: "整体耗时（前端）",
  unitMs: "毫秒",
  unitSec: "秒",
  perfSheets: "原板张数",
  perfUtilization: "整体成材率",
  perfTotalParts: "零件总数",
  perfPlaced: "成功套上数",
  perfUnplaced: "未套上数",
  perfRemaining: "末板余料长度",
  unplacedTitle: "未套上零件明细",
  viewerTitle: "套料方案查看器",
  placeholderTitle: "尚未生成套料方案",
  placeholderHint: "请先在左侧生成零件并点击“运行算法”",
  sheetTitle: (cur, total) => `第 ${cur} / ${total} 张`,
  sheetUtilization: "该板成材率：",
  sheetPartCount: "零件数：",
  sheetRemainingArea: "余料面积：",
  sheetOffcutArea: "边角料面积：",
  surplusLabel: "余料",
  prevPage: "上一页",
  nextPage: "下一页",
  jump: "跳转",
  pagerStatus: (cur, total) => `${cur} / ${total}`,
  pageError: (n) => `页码须在 1 ~ ${n} 之间，已自动调整。`,
  exportTitle: "导出全部板材（zip，每张板一个文件）",
  exportCsv: "导出 CSV (zip)",
  exportDxf: "导出 DXF (zip)",
  exportPng: "导出 PNG (zip)",
  exporting: "导出中…",
  errApiGeneric: "生成零件失败，请稍后重试。",
  errNoPartsFromBackend: "后端未返回零件数据，请检查接口返回格式。",
  errRun: "运算失败，请稍后重试。",
  errExport: "导出失败，请稍后重试。",
  importNeedPlate: "请先点击“生成随机矩形件”锁定板材参数，再导入零件列表。",
  importRejectReason: (plateL, plateW) =>
    `尺寸超出板材可用范围（须满足 长+2×边距 ≤ ${plateL}，宽+2×边距 ≤ ${plateW}）`,
  importEmpty: "没有可导入的零件（全部放不下或文件为空），零件列表未更新。",
  importSuccess: (ok, skipped) =>
    `导入成功 ${ok} 件` + (skipped > 0 ? `，${skipped} 件因放不下被跳过（见下方警告）。` : "。"),
  importWarnTooFew: (line) => `第 ${line} 行：列数不足（至少需要长度、宽度两列），已跳过。`,
  importWarnNotNumeric: (line, a, b) =>
    `第 ${line} 行：无法解析为数字（“${a}”,“${b}”），已跳过。`,
  importWarnNotPositive: (line, a, b) =>
    `第 ${line} 行：长度/宽度必须为正数（${a}, ${b}），已跳过。`,
  netErr: "无法连接后端服务，请确认 API（http://localhost:5088）已启动。",
  httpErr: (status) => `请求失败（HTTP ${status}），请稍后重试。`,
  langZh: "中文",
  langEn: "English",
};

const en: Strings = {
  appTitle: "Rectangle Nesting Algorithm · Online Demo Platform",
  appSubtitle: "Generate parts → Run nesting → View layouts / Export results",
  btnGuide: "User Guide",
  errorPrefix: "Error: ",
  paramCheckFailed: "Parameter validation failed. Please fix the following:",
  params: "Parameters",
  labelPlateLength: "Plate length",
  labelPlateWidth: "Plate width",
  labelPartCount: "Part count",
  labelEdgeDistance: "Edge distance",
  errPlateLength: "Plate length must be a finite number with 0 < length ≤ 100000.",
  errPlateWidth: "Plate width must be a finite number with 0 < width ≤ 100000.",
  errPartCount: "Part count must be an integer between 1 and 100000.",
  errEdgeNaN: "Edge distance must be a finite number ≥ 0.",
  errEdgeTooLarge:
    "Edge distance is unreasonable: it must satisfy 2 × edge distance < min(plate length, plate width).",
  hintLocked: (l, w, e) =>
    `Plate parameters locked: ${l} × ${w}, edge distance ${e}. Click "Generate Random Parts" again to update.`,
  hintUnlocked:
    'Plate length/width and edge distance will be locked after clicking "Generate Random Parts".',
  actions: "Actions",
  btnGenerate: "Generate Random Parts",
  btnGenerating: "Working…",
  btnExportParts: "Export Parts List",
  btnRun: "Run Algorithm",
  btnRunning: "Running…",
  importBoxTitle: "Import Parts List",
  importBoxHint: "Click to choose a file, or drag & drop a CSV file here",
  importBoxActive: "Release to upload",
  partsList: "Parts List",
  partsPreview: (total, max) => `${total} parts in total (previewing first ${max} rows)`,
  colIndex: "#",
  colLength: "Length",
  colWidth: "Width",
  colLine: "Line",
  colReason: "Reason",
  importWarnings: (n) => `Import Warnings (${n})`,
  timerTitle: "Elapsed Time",
  timerRunning: "Timing (mm:ss.cs)…",
  timerIdle: 'Click "Run Algorithm" to start timing',
  perfTitle: "Performance",
  perfServerTime: "Computation time (server)",
  perfClientTime: "Total time (frontend)",
  unitMs: "ms",
  unitSec: "s",
  perfSheets: "Sheets used",
  perfUtilization: "Overall utilization",
  perfTotalParts: "Total parts",
  perfPlaced: "Parts nested",
  perfUnplaced: "Parts not nested",
  perfRemaining: "Remainder of last sheet",
  unplacedTitle: "Parts Not Nested",
  viewerTitle: "Nesting Layout Viewer",
  placeholderTitle: "No nesting layout yet",
  placeholderHint: 'Generate parts on the left and click "Run Algorithm"',
  sheetTitle: (cur, total) => `Sheet ${cur} / ${total}`,
  sheetUtilization: "Sheet utilization: ",
  sheetPartCount: "Parts: ",
  sheetRemainingArea: "Remaining area: ",
  sheetOffcutArea: "Offcut area: ",
  surplusLabel: "Remainder",
  prevPage: "Prev",
  nextPage: "Next",
  jump: "Go",
  pagerStatus: (cur, total) => `${cur} / ${total}`,
  pageError: (n) => `Page number must be between 1 and ${n}; adjusted automatically.`,
  exportTitle: "Export all sheets (zip, one file per sheet)",
  exportCsv: "Export CSV (zip)",
  exportDxf: "Export DXF (zip)",
  exportPng: "Export PNG (zip)",
  exporting: "Exporting…",
  errApiGeneric: "Failed to generate parts. Please try again later.",
  errNoPartsFromBackend:
    "The backend returned no part data. Please check the API response format.",
  errRun: "Computation failed. Please try again later.",
  errExport: "Export failed. Please try again later.",
  importNeedPlate:
    'Please click "Generate Random Parts" first to lock the plate parameters before importing.',
  importRejectReason: (plateL, plateW) =>
    `Exceeds usable plate size (require length + 2 × edge ≤ ${plateL}, width + 2 × edge ≤ ${plateW})`,
  importEmpty:
    "No importable parts (all too large or the file is empty). The parts list was not updated.",
  importSuccess: (ok, skipped) =>
    `Imported ${ok} parts` +
    (skipped > 0 ? `; ${skipped} skipped because they do not fit (see warnings below).` : "."),
  importWarnTooFew: (line) =>
    `Line ${line}: not enough columns (at least length and width are required); skipped.`,
  importWarnNotNumeric: (line, a, b) =>
    `Line ${line}: cannot parse as numbers ("${a}", "${b}"); skipped.`,
  importWarnNotPositive: (line, a, b) =>
    `Line ${line}: length/width must be positive (${a}, ${b}); skipped.`,
  netErr:
    "Cannot connect to the backend. Please make sure the API (http://localhost:5088) is running.",
  httpErr: (status) => `Request failed (HTTP ${status}). Please try again later.`,
  langZh: "中文",
  langEn: "English",
};

export function getStrings(locale: Locale): Strings {
  return locale === "zh" ? zh : en;
}
