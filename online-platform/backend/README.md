# 后端（ASP.NET Core 8 Web API）

矩形件套料算法在线运算平台的后端。前端为 Next.js，套料引擎为原生 DLL `nesting.dll`（C 接口，cdecl，由并行任务构建，非线程安全）。

## 运行

```bash
cd online-platform/backend
dotnet run
```

服务监听 **http://localhost:5088**（`launchSettings.json` 与 `Program.cs` 中的 `UseUrls` 均已固定，直接 `dotnet run` 即可）。

构建：

```bash
dotnet build
```

`nesting.dll` 尚不存在时也能编译通过（仅产生 Warning）。

## 依赖说明

- 目标框架 `net8.0`，SDK 风格 `Microsoft.NET.Sdk.Web`，Controller 风格 API。
- **无第三方 NuGet 依赖**，仅框架自带包。
- `nesting.dll` 预期位于 `online-platform/native/x64/Release/nesting.dll`。
  csproj 中以 `Content` + `CopyToOutputDirectory=PreserveNewest` 拷贝到输出目录，
  且带 `Condition="Exists(...)"`——DLL 尚未构建时跳过拷贝、编译照常。
- 运行时若 DLL 缺失，`POST /api/nesting/run` 返回 **503** 与中文错误说明。

## CORS

允许任意来源（`AllowAnyOrigin/AllowAnyMethod/AllowAnyHeader`），便于前端开发服务器任意端口接入。

## API

所有接口均为 JSON，camelCase。

### POST /api/parts/generate — 生成随机矩形件

请求：

```json
{ "plateLength": 100, "plateWidth": 50, "count": 1000, "edgeDistance": 0.2 }
```

`plateLength` / `plateWidth` / `count` / `edgeDistance` 均可省略，默认值为 `100 / 50 / 1000 / 0.2`（前端也做默认值，后端同样兜底校验）。

校验规则（非法返回 400，`{ message, errors: string[] }`，中文消息）：

- 板材长、宽：有限数且 `0 < v ≤ 100000`
- 数量：整数 `1..100000`
- 边缘距离：`≥ 0` 且 `2 × edge < min(长, 宽)`

生成规则（小件偏多大件偏少）：

- `maxL = plateLength - 2·edge`，`maxW = plateWidth - 2·edge`；`minSize = max(1.0, min(长,宽) × 0.02)`
- 每件取 `(0,1)` 均匀随机 `u1, u2`，`len = clamp(maxL · u1^2.5, minSize, maxL)`，`wid = clamp(maxW · u2^2.5, minSize, maxW)`
- 指数 `2.5` 使尺寸分布向小件集中；约 30% 概率交换长宽（不超出 `maxL/maxW`）
- `id` 从 1 递增

响应：

```json
{ "parts": [{ "id": 1, "length": 12.3, "width": 4.5 }], "plateLength": 100, "plateWidth": 50, "edgeDistance": 0.2 }
```

### POST /api/nesting/run — 运行套料算法

请求：

```json
{ "plateLength": 100, "plateWidth": 50, "edgeDistance": 0.2,
  "parts": [{ "id": 1, "length": 12.3, "width": 4.5 }] }
```

处理流程：

1. 板材参数校验（同 generate）。
2. `parts` 数量 `1..100000`，每件长宽为正有限数。
3. **放得下检查**：每件须满足 `length + 2·edge ≤ plateLength` 且 `width + 2·edge ≤ plateWidth`。
   存在放不下的件时返回 400：

   ```json
   { "message": "存在无法放入原板的矩形件",
     "unplaceable": [{ "index": 3, "id": 7, "length": 120.0, "width": 5.0 }] }
   ```

   `index` 为该件在 `parts` 中的序号（1-based）。
4. DLL 缺失返回 503。
5. 调用 `nesting_run`（全局 lock 串行化；Stopwatch 只计原生调用耗时），
   返回码：`0` 成功；`1` 参数非法（400）；`2` 内部异常（500）；其他（500）。

响应 200：

```json
{
  "runtimeMs": 42.5,
  "numPlates": 12,
  "utilization": 0.87,
  "lastPlateSurplusLength": 300.2,
  "partsTotal": 1000,
  "partsNestedCount": 998,
  "unplaced": [{ "id": 55, "length": 10.1, "width": 3.2 }],
  "sheets": [{
    "sheetNo": 1, "utilization": 0.92, "surplusLength": 120.0, "surplusWidth": 50.0,
    "parts": [{ "id": 1, "x": 0.2, "y": 0.2, "length": 12.3, "width": 4.5 }]
  }]
}
```

- `layouts` 为 `N×6`（行号 `j` 对应 `parts[j]`），由 wrapper 以**逐行连续**方式回拷（行 `j` 从 `j*6` 开始）：`col0`=板号（1-based，未套上则 ≤0）、`col1`=板内序号（**不是零件 id，不使用**）、`col2..5`=x / y / x 方向长 / y 方向长。
- `sheetDetails` 同样逐行连续（每行 4 列，行=板）：`col0`=板号、`col1`=余料长、`col2`=余料宽、`col3`=该板成材率。
- 未套上判定：`col0 ≤ 0` 或 `partIsNested[j] < 1`。
- 板的 `parts[].id` 取 `parts[j].id`（原始 id）。
- 板按板号升序返回。

## 算法派生参数说明

| 参数 | 取值 | 依据 |
| --- | --- | --- |
| `intervalParts`（零件间距） | `max(0.01, round(min(长,宽) × 0.002, 3))` | 原板短边的 0.2%，毫米级板材下约 0.1mm 量级，符合切割工艺间隙；下限 0.01 防止算法内退化 |
| `tightnessCoef`（紧密度系数） | 所有零件 `min(length, width)` 的最小值 | 以最小零件边长为紧密度基准，避免细长款被过度放大间隔 |
| `intervalPlate`（板边缘距离） | `edgeDistance` | 直接使用用户指定的边缘距离 |
| `sCoef` | `0.1` | 与离线版一致的经验常数 |
| `reNestStep`（重套料步长） | `max(0.5, plateLength / 61)` | 随板长线性缩放：常见板长 6000 时步长约 100，板长 30 时步长约 0.5；下限 0.5 保证数值稳定 |
