# 矩形件套料算法 · 在线运算演示平台

在统一大矩形钢板上套小矩形零件、最大化成材率的算法演示平台。

- **算法**：仓库根目录 `source/`（MATLAB Coder 生成，入口 `Nest_and_reNest` = 单次排样 `nest_and_calc_Ver5` + 末板重套料），编译为原生 `nesting.dll`，成材率由算法自身计算。
- **后端**：`backend/`（C# / ASP.NET Core 8），通过 P/Invoke 调用算法 DLL，端口 **5088**。
- **前端**：`frontend/`（Next.js 16 + TypeScript），`npm run dev` 默认端口 3000（被占用时自动顺延，如 3001），`/api/*` 经 rewrites 代理到后端。界面支持中/英双语：默认跟随浏览器语言（中文 → 中文，其他 → 英文），右上角可手动切换。

> 算法版本的冻结快照见仓库根目录 `往期版本/Ver6-2026-09-20/`。根目录 `source/`、`head/`
> 为工作副本，已包含末板 skyline 紧凑重排与末板零件编号映射修复（详见根目录 README）。

## 启动方式

需要先启动后端，再启动前端。

```bash
# 1. 后端（端口 5088）
cd online-platform/backend
dotnet run

# 2. 前端（另开终端；默认 http://localhost:3000，被占用则看终端输出）
cd online-platform/frontend
npm run dev
```

打开终端里显示的地址（如 http://localhost:3000 ）即可使用。

## 使用流程

1. 输入参数：板材长、板材宽、矩形件数量、边缘距离（默认 **100 / 50 / 1000 / 0.2**），点击 **生成随机矩形件**。
   - 参数不合理会逐条提示；生成后板材长/宽锁定，重新点击生成才更新。
   - 随机件分布为「小件偏多大件偏少」，且保证能放入板内（计入边缘距离）。
2. **导出零件列表** 得到 parts.csv；可手工修改后用 **导入零件列表** 导入。
   - 导入时逐件检查 `长 + 2×边缘距离 ≤ 板材长` 且 `宽 + 2×边缘距离 ≤ 板材宽`，放不下的件会列出警告。
3. 点击 **运行算法**：计时框开始计时；完成后下方性能表展示运算耗时、整体耗时、原板张数、整体成材率、套上/未套上明细、末板余料。
4. 右侧查看器逐张展示套料图（上一页/下一页/页码跳转），可分别导出 **CSV / DXF / PNG** 三种 zip，每个文件即一张板的套料方案。

## 算法派生参数（后端自动设定，见 `backend/README.md`）

| 参数 | 取值 |
|---|---|
| 零件间距 intervalParts | `max(0.01, round(min(长,宽)×0.002, 3))` |
| 紧密度系数 tightnessCoef | 所有零件 `min(长,宽)` 的最小值 |
| 板边缘距离 intervalPlate | 用户输入的边缘距离 |
| 面积系数 S_coef | 0.1 |
| 末板重套料步长 reNestStep | `max(0.5, 板长/61)` |

## 重新构建算法 DLL

```bash
MSBuild.exe online-platform/native/nesting_dll.vcxproj -p:Configuration=Release -p:Platform=x64
# 产物：online-platform/native/x64/Release/nesting.dll（后端构建时自动拷到输出目录）
```

冒烟测试：`online-platform/native/test/`（smoke_test.exe，验证所有零件套上、无重叠、不出板界）。

## API 一览

| 方法 | 路径 | 说明 |
|---|---|---|
| POST | `/api/parts/generate` | 校验参数并生成随机矩形件 |
| POST | `/api/nesting/run` | 运行套料算法，返回性能与每板布局 |

`run` 的响应：`runtimeMs`、`numPlates`、`utilization`（0~1）、`lastPlateSurplusLength`、`partsTotal`、`partsNestedCount`、`unplaced[]`、`sheets[]`（每板 `sheetNo/utilization/surplusLength/surplusWidth/parts[{id,x,y,length,width}]`）。坐标系为左下角原点、Y 向上（与 DXF 一致；前端 SVG/PNG 已做 Y 翻转）。
