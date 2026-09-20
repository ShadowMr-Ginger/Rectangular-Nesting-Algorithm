# 矩形件套料算法（中文说明）

**在线演示：<https://jianqiaoxu.xyz/rectangular-packing>**

在统一尺寸的大矩形钢板上套排小矩形零件，最小化原板用量、最大化整体成材率的套料算法。

基准算例：1000 个随机零件、100 × 50 原板，算法产出 **98 张原板、整体成材率 96.6%**，
桌面电脑上运算耗时约 **0.35 秒**（基准数据见仓库根目录 `parts.csv`）。

> 本文件是中文版说明，英文版见仓库根目录 `README.md`。

## 目录结构

```
source/                  MATLAB Coder 生成的 C 代码——套料算法（工作副本）
head/                    source/ 对应的头文件
Nest_and_reNest.m        算法入口的 MATLAB 可读参考实现
ver6.sln / ver6.vcxproj  原始离线版 Visual Studio 工程（控制台程序 + DXF 输出）
example.cpp              离线版程序入口（读 dataA*.csv，输出结果）
CSV/                     离线版用的 CSV 读写组件
DXFOperate/              离线版用的 DXF 图纸输出组件
parts.csv                1000 件基准算例数据（id,length,width）
往期版本/                 算法往期版本的冻结快照
online-platform/         在线演示平台（Next.js + C# + 原生 DLL）
中文说明/                 本文件夹（中文说明）
```

## 算法原理

算法入口为 `Nest_and_reNest`（`source/Nest_and_reNest.c`），由三个阶段组成：

### 1. 启发式逐件排样 —— `nest_and_calc_Ver5`

逐件把零件放到当前原板上。对每个零件，算法在当前布局的“角点”候选位置中，
用**紧密度**目标（`tightnessCalc`，由经验系数 `S_coef` 加权）评估：
优先选择投影浪费空间最小的位置，使布局持续向右延伸并保持紧凑。
当剩余零件都放不上当前板时，封板并开启新板。

### 2. 末板重套料 —— `Nest_and_reNest`

最后一张板通常用不满。算法取出末板上的全部零件，通过**步长二分**缩短板长
（`reNestStepLength`，一旦需要两张板或有零件放不下就把步长减半），
在更短的板上重新排样，最大化剩余的板料。

### 3. 末板 skyline 紧凑重排（2026-09-20 改进）

重套料只缩短板长，板内仍是“向右延伸”的排布，右上方常常空着，
最右端却离零件很远。末板余料按**一刀切口径**估值：在最右端零件右缘竖直切一刀，
切线右侧的整板料全部计为有效余料。

新增后处理步骤（`source/last_plate_refine.c`）用 **skyline（skyline/bottom-left）**
紧凑重排末板零件：把零件放在最低、最左的天际线段上（允许旋转 90°），
尝试四种排序（最大边/面积/宽度降序 + 原始顺序）取最优。
仅当一刀切余料严格变大时才写回结果，主流程与耗时不受影响。

基准算例（`parts.csv`，100 × 50 板）：末板最右端从 **36.2 降到 29.8**，
一刀切余料从 **63.8 提升到 70.2**（多出 6.4 个长度单位、约 320 平方单位可用余料），
额外耗时仅约 2 毫秒。

同样的改进已同步到 `Nest_and_reNest.m`（局部函数 `refineLastPlate` / `skylineRun`）。

### 成材率口径

算法输出的整体成材率为：

```
成材率 = 1 − 末板余料长度 × 板宽 / (板数 × 板长 × 板宽)
```

即满板全部计入，只扣除末板按一刀切口径计算的余料。

末板自身的成材率（性能表与查看器中逐板展示）按同一口径计算：

```
末板成材率 =（零件总面积 + 余料长度 × 板宽）/（板长 × 板宽）
```

即一刀切余料计入有效材料，零件之间的间隙计为损耗。

**术语约定：** 只有末板一刀切右侧的整宽条带称为「余料」；其余一切未用区域
（零件间隙、满板两侧窄条）统称「边角料」。查看器中末板右侧的余料区以蓝色
斜线阴影高亮，信息栏分别显示「边角料面积」与「余料面积」（余料仅末板有）。

### 零件编号修复（2026-09-20）

末板经子问题重套料后内部会重排零件，行号不再对应零件。
原代码按行顺序写回保存的编号，导致末板零件编号错位。
修复后按子布局自身的序号列做间接映射（`真实编号 = 保存编号[局部序号]`），
C 源码与 MATLAB 参考实现同步修复，并已用“编号 ↔ 尺寸（含旋转）”逐件校验通过。

## 离线版程序的编译与运行

需要 Windows + Visual Studio（MSVC）。

```bash
# 编译控制台程序（ver6.exe）
MSBuild.exe ver6.vcxproj -p:Configuration=Release -p:Platform=x64

# 或用 Visual Studio 打开 ver6.sln 直接生成
```

`example.cpp` 展示了编程用法：从 `dataA1.csv` 读入零件
（`dataA1..A6.csv` 为示例数据），设置板材尺寸与算法参数，运行并导出套料结果
（CSV + DXF，经 `DXFOperate`）。参数也可经 `parametersSetting.ini` 持久化
（`RectangleNestTask::writeINI`）：

| 参数 | 含义 |
|---|---|
| `tightnessCoef` | 紧密度系数，取值应与零件尺寸同量级 |
| `S_coef` | 紧密度项在评分中的权重（经验值 0.1） |
| `reNestStepLength` | 末板压缩初始步长（二分起点） |
| `interval_distance_parts` | 零件间距 |
| `interval_distance_plate` | 边缘距离（零件到板边） |

## 在线演示平台

`online-platform/` 是全栈演示：**Next.js 16 + TypeScript** 前端、
**C# / ASP.NET Core 8** 后端、算法编译为原生 **`nesting.dll`**
（P/Invoke 桥接见 `native/wrapper.cpp`）。界面中英双语，
默认跟随浏览器语言（中文 → 中文，其他 → 英文），可手动切换。

### 环境要求

- .NET 8 SDK
- Node.js 18+（Next.js 16）
- Visual Studio MSBuild（编译原生 DLL）

### 编译与启动

```bash
# 1. 编译算法 DLL（一次性；算法改动后需重新编译）
MSBuild.exe online-platform/native/nesting_dll.vcxproj -p:Configuration=Release -p:Platform=x64

# 2. 后端（端口 5088）
cd online-platform/backend
dotnet run

# 3. 前端（另开终端；http://localhost:3000，被占用则自动顺延）
cd online-platform/frontend
npm run dev
```

前端把 `/api/*` 代理到后端。生产环境用 `npm run build` + `npm run start`。

### 使用流程

1. 输入**板材长、板材宽、矩形件数量、边缘距离**（默认 100 / 50 / 1000 / 0.2），
   点击“生成随机矩形件”。参数不合理会逐条提示；生成后锁定板材长宽，
   重新点击才会更新。随机件“小件偏多大件偏少”，且保证能放入板内。
2. “导出零件列表”得到 CSV（`id,length,width`），可修改后经拖拽/点击导入。
   导入时逐行检查 `长 + 2×边距 ≤ 板长`、`宽 + 2×边距 ≤ 板宽`，放不下的件跳过并警告。
3. 点击“运行算法”，计时框开始计时；完成后性能表展示运算耗时、整体耗时、
   原板张数、整体成材率、套上/未套上数量、末板余料长度。
4. 右侧查看器按原板分页展示套料图（上一页/下一页/页码跳转），
   可将全部方案导出为 zip（每张板一个文件），支持 **CSV / DXF（CAD）/ PNG** 三种格式。
5. 页面顶部“使用说明”按钮进入 `/guide` 图文指南，介绍全部参数、CSV 格式与套料图读法。

### API 一览

| 方法 | 路径 | 说明 |
|---|---|---|
| POST | `/api/parts/generate` | 校验参数并生成随机矩形件 |
| POST | `/api/nesting/run` | 运行套料算法，返回性能与每板布局 |

`run` 响应字段：`runtimeMs`、`numPlates`、`utilization`（0~1）、
`lastPlateSurplusLength`、`partsTotal`、`partsNestedCount`、`unplaced[]`、
`sheets[]`（每板 `sheetNo`、`utilization`、`surplusLength`、`surplusWidth`、
`parts[{id,x,y,length,width}]`）。坐标系为左下角原点、Y 向上（与 DXF 一致；
前端 SVG/PNG 预览做了 Y 翻转）。

派生参数（后端自动设定）：

| 参数 | 取值 |
|---|---|
| 零件间距 `intervalParts` | `max(0.01, round(min(长,宽) × 0.002, 3))` |
| 紧密度系数 `tightnessCoef` | 所有零件 `min(长,宽)` 的最小值 |
| 边缘距离 `intervalPlate` | 用户输入 |
| `S_coef` | 0.1 |
| 重套料步长 `reNestStep` | `max(0.5, 板长 / 61)` |

## 验证工具

`online-platform/native/test/` 内含原生测试工具：

- `smoke_test.exe` —— 端到端冒烟：全部零件套上、无重叠、不出板界。
- `repro_parts.exe parts.csv` —— 运行基准算例，输出板数/成材率/耗时及末板 SVG。

## 作者

**Jianqiao Xu**（ShadowMr-Ginger）—— <xujianqiao020319@163.com>

演示部署：<https://jianqiaoxu.xyz/rectangular-packing>
