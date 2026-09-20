# Rectangular Nesting Algorithm

**Online demo: <https://jianqiaoxu.xyz/rectangular-packing>**

A nesting (stock-cutting) algorithm that packs small rectangular parts onto identical
large rectangular steel plates, minimizing the number of plates used and maximizing
the overall material utilization (成材率).

Given 1 000 random parts and 100 × 50 plates, the algorithm produces **98 plates at
96.6 % overall utilization** in about **0.35 s** on a desktop PC (see `parts.csv`
for the benchmark data set).

## Repository layout

```
source/                  MATLAB Coder generated C code — the nesting algorithm (working copy)
head/                    Headers for source/
Nest_and_reNest.m        Human-readable MATLAB reference of the entry point
ver6.sln / ver6.vcxproj  Original offline Visual Studio project (console app + DXF output)
example.cpp              Original offline program entry (reads dataA*.csv, writes results)
CSV/                     CSV reading/writing helpers (used by the offline app)
DXFOperate/              DXF writing helpers (used by the offline app)
parts.csv                1 000-part benchmark data set (id,length,width)
往期版本/                 Frozen snapshots of previous algorithm versions
online-platform/         Full-stack online demo platform (Next.js + C# + native DLL)
中文说明/                 Chinese documentation (README in Chinese)
```

## How the algorithm works

The entry point is `Nest_and_reNest` (`source/Nest_and_reNest.c`), which combines
two stages:

### 1. Greedy nesting with a tightness heuristic — `nest_and_calc_Ver5`

Parts are placed one at a time onto the current plate. For each part, candidate
positions (the "corner vertices" of the current layout) are evaluated with a
**tightness** objective (`tightnessCalc`, weighted by the empirical coefficient
`S_coef`): the algorithm prefers the position whose projection leaves the least
wasted space, which pushes the layout to extend to the right and stay compact.
The plate is closed and a new one is opened when no remaining part fits.

### 2. Last-plate re-nesting — `Nest_and_reNest`

The last plate is usually only partially filled. The algorithm extracts the parts
on the last plate, then **shrinks the plate length by bisection**
(`reNestStepLength`, halved whenever two plates would be needed or a part no longer
fits) and re-nests those parts on the shorter plate, maximizing the leftover strip.

### 3. Last-plate skyline compaction *(improvement, 2026-09-20)*

Re-nesting only shortens the plate; inside the plate the layout still extends
rightward and can leave the upper-right corner empty while the rightmost edge
stays far from the parts. The remainder of the last plate is valued by a
**one-cut rule**: a vertical cut just right of the rightmost part, with everything
to the right of the cut counting as usable surplus material.

A new post-processing step (`source/last_plate_refine.c`) re-packs the last-plate
parts with a **skyline (bottom-left) compaction**: parts are placed on the lowest,
leftmost skyline segment (90° rotation allowed), trying four orderings (max edge /
area / width descending, and original order) and keeping the best. The result is
written back only when it strictly improves the one-cut surplus, so the main flow
and its running time are untouched.

On the benchmark (`parts.csv`, 100 × 50 plates): the rightmost occupied edge of the
last plate moved from **36.2 to 29.8**, the one-cut surplus from **63.8 to 70.2**
(+6.4 units of plate length, ~320 units² of reusable material), at the cost of
~2 ms.

The same improvement is mirrored in `Nest_and_reNest.m` (local functions
`refineLastPlate` / `skylineRun`).

### Utilization

The overall utilization reported by the algorithm is

```
utilization = 1 − lastPlateSurplusLength × plateWidth / (numPlates × plateLength × plateWidth)
```

i.e. all full plates count as fully used and only the last plate's surplus (valued
by the one-cut rule) is deducted. The last plate's own utilization (shown in the
per-sheet table and viewer) is computed by the same one-cut rule:

```
lastPlateUtilization = (total part area + surplusLength × plateWidth) / (plateLength × plateWidth)
```

i.e. the one-cut remainder counts as effective material; only the gaps between
parts count as waste.

### Part-identity fix *(2026-09-20)*

The last plate is re-nested through a sub-call that reorders the parts internally,
so layout rows can no longer be matched to parts by row order. The original code
wrote the saved part numbers back row by row, which scrambled the part IDs of the
last plate. The fix maps each row through the sub-layout's own index column
(`realId = savedIds[localIndex]`), in both the C source and the MATLAB reference.
Verified with a rotation-aware id ↔ dimensions check on every part of the last plate.

## Building and running the original offline program

Requires Visual Studio (MSVC) on Windows.

```bash
# Build the console application (ver6.exe)
MSBuild.exe ver6.vcxproj -p:Configuration=Release -p:Platform=x64

# or open ver6.sln in Visual Studio and build
```

`example.cpp` shows the programmatic usage: load parts from `dataA1.csv`
(`dataA1..A6.csv` are sample data sets), set the plate size and the algorithm
parameters, run, and export layouts (CSV + DXF via `DXFOperate`). Parameters can
also be persisted through `parametersSetting.ini` (`RectangleNestTask::writeINI`):

| Parameter | Meaning |
|---|---|
| `tightnessCoef` | Tightness scale; should be comparable to the part size |
| `S_coef` | Weight of the tightness term in placement scoring (empirical, 0.1) |
| `reNestStepLength` | Initial last-plate shrink step (bisection start) |
| `interval_distance_parts` | Gap between parts |
| `interval_distance_plate` | Edge distance (parts to plate edges) |

## Online demo platform

`online-platform/` is a full-stack demo: a **Next.js 16 + TypeScript** frontend,
a **C# / ASP.NET Core 8** backend, and the algorithm compiled as a native
**`nesting.dll`** (P/Invoke bridge in `native/wrapper.cpp`). The UI is bilingual
(Chinese/English, defaults to the browser language).

### Prerequisites

- .NET 8 SDK
- Node.js 18+ (Next.js 16)
- Visual Studio MSBuild (to build the native DLL)

### Build & run

```bash
# 1. Build the algorithm DLL (one time; rebuilt automatically on demand)
MSBuild.exe online-platform/native/nesting_dll.vcxproj -p:Configuration=Release -p:Platform=x64

# 2. Backend (port 5088)
cd online-platform/backend
dotnet run

# 3. Frontend (separate terminal; http://localhost:3000, or the next free port)
cd online-platform/frontend
npm run dev
```

The frontend proxies `/api/*` to the backend. In production, `npm run build` +
`npm run start` serves the optimized build.

### Usage flow

1. Enter **plate length, plate width, part count, edge distance**
   (defaults 100 / 50 / 1000 / 0.2) and click **Generate Random Parts**.
   Invalid parameters are reported one by one. Generation locks the plate
   dimensions; click again to regenerate. Parts are drawn with many small parts
   and few large ones, all guaranteed to fit.
2. **Export Parts List** produces a CSV (`id,length,width`) that can be edited and
   re-imported via drag & drop. On import, every row is checked against the plate
   (`length + 2×edge ≤ plateLength`, `width + 2×edge ≤ plateWidth`); parts that do
   not fit are skipped with warnings.
3. Click **Run Algorithm** — the timer starts immediately; the performance table
   then shows server computation time, total client time, number of plates, overall
   utilization, nested / not-nested counts and the last-plate remainder length.
4. The viewer on the right paginates the layouts (prev / next / page-number jump)
   and exports all plates as a zip archive — one file per plate — in **CSV**,
   **DXF** (CAD) or **PNG** format.
5. A built-in **User Guide** page (`/guide`, also linked in the header) explains
   every parameter, the CSV format and how to read the layout diagrams.

### API

| Method | Endpoint | Description |
|---|---|---|
| POST | `/api/parts/generate` | Validate parameters and generate random parts |
| POST | `/api/nesting/run` | Run the nesting algorithm, return performance + layouts |

`run` response: `runtimeMs`, `numPlates`, `utilization` (0–1),
`lastPlateSurplusLength`, `partsTotal`, `partsNestedCount`, `unplaced[]`,
`sheets[]` (`sheetNo`, `utilization`, `surplusLength`, `surplusWidth`,
`parts[{id,x,y,length,width}]`). Coordinates have their origin at the plate's
bottom-left with Y pointing up (DXF convention; the SVG/PNG previews flip Y).

Derived parameters (set automatically by the backend):

| Parameter | Value |
|---|---|
| Part gap `intervalParts` | `max(0.01, round(min(L,W) × 0.002, 3))` |
| Tightness `tightnessCoef` | min over parts of `min(length, width)` |
| Edge distance `intervalPlate` | user input |
| `S_coef` | 0.1 |
| Re-nest step `reNestStep` | `max(0.5, plateLength / 61)` |

## Verification

`online-platform/native/test/` contains native test harnesses:

- `smoke_test.exe` — end-to-end check: every part nested, no overlaps, all within
  plate bounds.
- `repro_parts.exe parts.csv` — runs the benchmark data set and prints plates /
  utilization / runtime plus an SVG of the last plate.

## Author

**Jianqiao Xu** (ShadowMr-Ginger) — <xujianqiao020319@163.com>

Demo deployment: <https://jianqiaoxu.xyz/rectangular-packing>
