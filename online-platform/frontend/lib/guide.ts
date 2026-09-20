export interface GuideStep {
  title: string;
  text: string;
}

export interface GuideParam {
  name: string;
  desc: string;
}

export interface GuideColumn {
  name: string;
  desc: string;
}

export interface GuideContent {
  title: string;
  back: string;
  introTitle: string;
  introText: string;
  workflowTitle: string;
  steps: GuideStep[];
  paramsTitle: string;
  params: GuideParam[];
  derivedNoteTitle: string;
  derivedNote: string;
  csvTitle: string;
  csvIntro: string;
  csvColumnsTitle: string;
  csvColumns: GuideColumn[];
  csvNotes: string[];
  layoutTitle: string;
  coordText: string;
  edgeText: string;
  lastPlateTitle: string;
  lastPlateText: string;
  viewerTitle: string;
  viewerText: string;
  exportTitle: string;
  exportText: string;
}

const zh: GuideContent = {
  title: "使用说明",
  back: "← 返回套料平台",
  introTitle: "平台简介",
  introText:
    "本平台用于演示矩形件套料算法：在统一尺寸的大矩形钢板（原板）上排布大量小矩形零件，" +
    "目标是最小化原板用量、最大化整体成材率。左侧输入参数并生成零件，右侧查看每张原板的套料方案图，" +
    "并可把全部方案导出为 CSV / DXF / PNG 压缩包。",
  workflowTitle: "使用流程",
  steps: [
    {
      title: "① 设置参数",
      text: "填写板材长、板材宽、矩形件数量、边缘距离（默认 100 × 50，1000 件，边距 0.2），" +
        "点击“生成随机矩形件”。参数不合理时会逐条提示错误，不会生成。",
    },
    {
      title: "② 生成并锁定板材",
      text: "点击生成按钮后，板材长/宽与边缘距离被锁定（防止方案中途换板），" +
        "同时随机生成所有零件的长宽——小尺寸零件偏多、大尺寸零件偏少，且都保证能放入原板。" +
        "再次点击该按钮才会更新板材参数并重新生成。",
    },
    {
      title: "③ 导出 / 导入零件列表",
      text: "生成后可点击“导出零件列表”得到 CSV 文件，可用 Excel 等工具修改后，" +
        "通过拖拽或点击导入框重新导入。导入时会逐行检查：凡在原板中放不下的零件" +
        "（计入边缘距离：长 + 2×边距 > 板长，或宽 + 2×边距 > 板宽）会被跳过并给出警告。",
    },
    {
      title: "④ 运行算法",
      text: "点击“运行算法”后计时框开始计时；运算完成后，下方性能表展示运算耗时、" +
        "整体成材率、原板张数、套上/未套上零件数、末板余料长度等指标。",
    },
    {
      title: "⑤ 查看套料方案",
      text: "右侧面板按原板分页展示套料方案图，可用“上一页 / 下一页”翻页，" +
        "也可直接输入页码跳转；每页显示该板成材率、零件数与余料面积。",
    },
    {
      title: "⑥ 导出套料方案",
      text: "可将全部原板的套料方案打包导出：每个 zip 内每张板一个文件，支持 CSV、DXF（CAD 图纸）、PNG（图片）三种格式。",
    },
  ],
  paramsTitle: "参数说明",
  params: [
    { name: "板材长", desc: "原板沿 X 方向的长度，单位与零件一致（如 mm）。须为 0 < 长 ≤ 100000 的有限数字。" },
    { name: "板材宽", desc: "原板沿 Y 方向的宽度。须为 0 < 宽 ≤ 100000 的有限数字。" },
    { name: "矩形件数量", desc: "要套料的零件个数，1 ~ 100000 的整数。数量越大运算时间越长。" },
    { name: "边缘距离", desc: "零件与原板四边的最小间距。须 ≥ 0 且满足 2 × 边距 < min(板长, 板宽)。" },
  ],
  derivedNoteTitle: "派生参数（由平台自动计算，无需填写）",
  derivedNote:
    "零件间距取原板短边的 0.2%（毫米级板材约为 0.1mm，下限 0.01）；紧密度系数取所有零件最小边中的最小值；" +
    "重套料步长随板长缩放（板长 / 61，下限 0.5）；S 系数为经验常数 0.1。",
  csvTitle: "零件列表 CSV 文件格式",
  csvIntro:
    "导出与导入使用同一格式：带 UTF-8 BOM 的 CSV，第一行为表头，其后每行一个零件。",
  csvColumnsTitle: "列说明",
  csvColumns: [
    { name: "id", desc: "零件编号，正整数。导出时自动生成；导入时原样保留，用于在结果中标识零件。" },
    { name: "length", desc: "零件长（X 向尺寸），正数。" },
    { name: "width", desc: "零件宽（Y 向尺寸），正数。" },
  ],
  csvNotes: [
    "导入时三列缺一不可；length / width 必须为正数，否则该行被跳过并警告。",
    "零件允许在套料时旋转 90°，因此长宽写反不影响能否放入，但建议按实际填写。",
    "修改 CSV 后请保存为 CSV（逗号分隔）格式再导入。",
  ],
  layoutTitle: "套料方案图说明",
  coordText:
    "方案图以原板左下角为原点，X 轴沿板长向右，Y 轴沿板宽向上；每个矩形为一件零件，" +
    "图中标注了零件编号。零件之间留有间距，四边与板边保持边缘距离。",
  edgeText:
    "蓝色细线框为原板边界，灰色区域为边缘距离示意，橙色矩形为零件。",
  lastPlateTitle: "末板余料口径（一刀切）",
  lastPlateText:
    "最后一张原板往往用不满。算法先通过“末板重套料”尽量缩短该板的占用长度，" +
    "再对方案做紧凑重排，直接最小化最右端边界：在最右端零件的右缘竖直切一刀，" +
    "切线右侧的整板料全部计为有效余料（灰色阴影区）。因此末板成材率按此口径单独计算。",
  viewerTitle: "翻页查看",
  viewerText:
    "用了 N 张原板就有 N 页方案；“第 k / N 张”显示在方案图上方；支持按钮翻页与页码跳转。",
  exportTitle: "批量导出",
  exportText:
    "导出结果为 zip 压缩包，内部每张板一个文件：CSV 含该板全部零件的坐标与尺寸；" +
    "DXF 可在 AutoCAD 等软件中打开；PNG 为方案图图片。文件名含板号，便于对照。",
};

const en: GuideContent = {
  title: "User Guide",
  back: "← Back to the nesting platform",
  introTitle: "Overview",
  introText:
    "This platform demonstrates a rectangular nesting algorithm: packing many small rectangular " +
    "parts onto identical large rectangular sheets (plates) to minimize the number of plates used " +
    "and maximize overall material utilization. Enter parameters and generate parts on the left, " +
    "inspect the layout of every plate on the right, and export all layouts as a CSV / DXF / PNG zip archive.",
  workflowTitle: "Workflow",
  steps: [
    {
      title: "1. Set parameters",
      text: 'Fill in plate length, plate width, part count and edge distance (defaults: 100 × 50, 1000 parts, edge 0.2), then click "Generate Random Parts". Invalid parameters are reported item by item and nothing is generated.',
    },
    {
      title: "2. Generate & lock the plate",
      text: 'After clicking the button, plate length/width and edge distance are locked (so a layout never mixes plates); all part dimensions are generated randomly — small parts are more frequent than large ones — and every part is guaranteed to fit the plate. Click the button again to update the plate and regenerate.',
    },
    {
      title: "3. Export / import the parts list",
      text: 'Once generated, click "Export Parts List" to get a CSV file. You can edit it in Excel etc. and import it back via drag & drop or the file picker. On import every row is checked: parts that cannot fit on the plate (accounting for the edge distance, i.e. length + 2 × edge > plate length or width + 2 × edge > plate width) are skipped with a warning.',
    },
    {
      title: "4. Run the algorithm",
      text: 'Click "Run Algorithm" to start the timer. When finished, the performance table shows computation time, overall utilization, number of plates, nested / not-nested counts and the remainder length of the last plate.',
    },
    {
      title: "5. Inspect layouts",
      text: "The right-hand panel paginates the layout sheet by sheet. Use Prev / Next or type a page number to jump. Each page shows the sheet utilization, part count and remaining area.",
    },
    {
      title: "6. Export layouts",
      text: "Export all plates as a zip archive — one file per plate — in CSV, DXF (CAD drawing) or PNG (image) format.",
    },
  ],
  paramsTitle: "Parameters",
  params: [
    { name: "Plate length", desc: "Plate size along the X axis, in the same unit as the parts (e.g. mm). Must satisfy 0 < length ≤ 100000." },
    { name: "Plate width", desc: "Plate size along the Y axis. Must satisfy 0 < width ≤ 100000." },
    { name: "Part count", desc: "Number of parts to nest, an integer between 1 and 100000. Larger counts take longer." },
    { name: "Edge distance", desc: "Minimum gap between parts and the plate edges. Must be ≥ 0 and satisfy 2 × edge < min(plate length, plate width)." },
  ],
  derivedNoteTitle: "Derived parameters (computed automatically by the platform)",
  derivedNote:
    "Part-to-part gap = 0.2% of the shorter plate side (about 0.1 mm for millimetre-scale plates, floor 0.01); the tightness coefficient is the smallest of all parts' shorter sides; the re-nesting step scales with plate length (plate length / 61, floor 0.5); the S coefficient is an empirical constant 0.1.",
  csvTitle: "Parts-list CSV format",
  csvIntro:
    "Export and import share the same format: a UTF-8 BOM CSV with a header row, one part per line.",
  csvColumnsTitle: "Columns",
  csvColumns: [
    { name: "id", desc: "Part ID, a positive integer. Generated automatically on export; preserved as-is on import and used to identify parts in the results." },
    { name: "length", desc: "Part length (X dimension), a positive number." },
    { name: "width", desc: "Part width (Y dimension), a positive number." },
  ],
  csvNotes: [
    "All three columns are required on import; non-positive or non-numeric dimensions cause the row to be skipped with a warning.",
    "Parts may be rotated 90° during nesting, so swapping length/width does not affect feasibility, but writing actual values is recommended.",
    "After editing, save the file as CSV (comma-separated) before importing.",
  ],
  layoutTitle: "Reading the layout diagram",
  coordText:
    "The origin is the plate's bottom-left corner; the X axis runs along the plate length to the right and the Y axis along the plate width upward. Each rectangle is one part, labelled with its ID. Parts are separated by gaps and kept at the edge distance from all four plate edges.",
  edgeText:
    "The thin blue frame is the plate boundary, the grey bands indicate the edge distance, and the orange rectangles are parts.",
  lastPlateTitle: "Last-plate remainder (one-cut rule)",
  lastPlateText:
    "The last plate is usually not full. The algorithm first shrinks its occupied length by re-nesting, then compacts the layout to minimize the rightmost extent: a vertical cut is made just right of the rightmost part, and all plate material to the right of the cut line counts as usable remainder (grey shaded area). The last plate's utilization is computed with this rule.",
  viewerTitle: "Pagination",
  viewerText:
    'N plates produce N pages. The "Sheet k / N" caption sits above the diagram. Button paging and direct page-number jumping are both supported.',
  exportTitle: "Batch export",
  exportText:
    "The export is a zip archive with one file per plate: CSV contains every part's coordinates and dimensions on that plate; DXF opens in AutoCAD etc.; PNG is an image of the layout. File names include the plate number for cross-reference.",
};

export function getGuideContent(locale: "zh" | "en"): GuideContent {
  return locale === "zh" ? zh : en;
}
