using System.Diagnostics;
using Backend.Models;
using Backend.Native;
using Microsoft.AspNetCore.Mvc;

namespace Backend.Controllers;

[ApiController]
[Route("api/nesting")]
public class NestingController : ControllerBase
{
    /// <summary>
    /// layouts 为逐行连续存储的 N×6（行 j 从 j*6 开始，由 wrapper 从 MATLAB
    /// 列主序转换而来，见 native/wrapper.cpp）。
    /// 每行：col0=板号（1-based，未套上则 ≤ 0）；col1=零件在输入列表中的 1-based
    /// 序号（即真实零件编号，行号是放置顺序，与输入序号不对应）；
    /// col2=x；col3=y；col4=x 方向长；col5=y 方向长。
    /// </summary>
    private const int LayoutCols = 6;

    /// <summary>sheetDetails 逐行连续存储（每行 4 列），行=板，列：0=板号 / 1=余料长 / 2=余料宽 / 3=该板成材率。</summary>
    private const int SheetCols = 4;

    [HttpPost("run")]
    public ActionResult<RunNestingResponse> Run([FromBody] RunNestingRequest request)
    {
        // ---- 1. 板材参数校验（与 parts/generate 同一套规则）----
        var errors = request.ValidateAndApplyDefaults();
        if (errors.Count > 0)
            return BadRequest(new ValidationErrorResponse { Errors = errors });

        double plateLength = request.PlateLength!.Value;
        double plateWidth = request.PlateWidth!.Value;
        double edge = request.EdgeDistance!.Value;

        // ---- 2. parts 校验 ----
        var parts = request.Parts ?? new List<PartItem>();
        if (parts.Count is < GeneratePartsRequest.CountMin or > GeneratePartsRequest.CountMax)
        {
            return BadRequest(new ValidationErrorResponse
            {
                Errors = { $"零件数量必须是 {GeneratePartsRequest.CountMin}..{GeneratePartsRequest.CountMax}，当前值：{parts.Count}。" }
            });
        }

        foreach (var p in parts)
        {
            if (!double.IsFinite(p.Length) || p.Length <= 0 || !double.IsFinite(p.Width) || p.Width <= 0)
            {
                return BadRequest(new ValidationErrorResponse
                {
                    Errors = { $"零件尺寸必须是正有限数（id={p.Id}，长={p.Length}，宽={p.Width}）。" }
                });
            }
        }

        // ---- 3. 放得下检查：长宽（含边缘距离）都不得超过原板 ----
        var unplaceable = new List<UnplaceablePart>();
        for (int i = 0; i < parts.Count; i++)
        {
            var p = parts[i];
            if (p.Length + 2 * edge > plateLength || p.Width + 2 * edge > plateWidth)
                unplaceable.Add(new UnplaceablePart { Index = i + 1, Id = p.Id, Length = p.Length, Width = p.Width });
        }
        if (unplaceable.Count > 0)
            return BadRequest(new UnplaceableResponse { Unplaceable = unplaceable });

        // ---- 4. DLL 可用性检查（缺失时 503，避免 P/Invoke 直接抛 DllNotFoundException）----
        if (!NestingLibrary.IsAvailable)
        {
            return StatusCode(StatusCodes.Status503ServiceUnavailable, new
            {
                message = "套料引擎 nesting.dll 不可用。该 DLL 由并行任务构建，预期位于 online-platform/native/x64/Release/nesting.dll，请确认已构建后重新启动本服务。"
            });
        }

        int n = parts.Count;

        // ---- 5. 算法派生参数（依据见 README.md）----
        // 零件间距：原板的 0.2%，毫米级板材下约为 0.1mm 量级；不低于 0.01 防止算法内除零/退化。
        double intervalParts = Math.Max(0.01, Math.Round(Math.Min(plateLength, plateWidth) * 0.002, 3));
        // 紧密度系数：取所有零件最小边中的最小值，匹配最细长零件的尺度。
        double tightnessCoef = parts.Min(p => Math.Min(p.Length, p.Width));
        // 板边缘距离：直接使用用户指定的边缘距离。
        double intervalPlate = edge;
        // 经验常数（与离线版一致）。
        const double sCoef = 0.1;
        // 重套料步长：随板长缩放，保证常见板长（如 6000mm）下步长约 100，且不低于 0.5。
        double reNestStep = Math.Max(0.5, plateLength / 61.0);

        var partLengths = parts.Select(p => p.Length).ToArray();
        var partWidths = parts.Select(p => p.Width).ToArray();

        // 预分配输出缓冲（列主序）。
        var layouts = new double[n * LayoutCols];
        var sheetDetails = new double[n * SheetCols]; // 板数 ≤ 零件数，按最大可能分配
        var partIsNested = new double[n];

        int ret;
        double numPlates, lastPlateSurplus, utilization;
        var stopwatch = Stopwatch.StartNew();
        lock (NestingLibrary.SyncRoot) // DLL 非线程安全，全局串行化
        {
            ret = NativeMethods.nesting_run(
                partLengths, partWidths, n,
                plateLength, plateWidth,
                tightnessCoef, intervalParts, intervalPlate,
                sCoef, reNestStep,
                layouts, sheetDetails, partIsNested,
                out numPlates, out lastPlateSurplus, out utilization);
        }
        stopwatch.Stop();

        return ret switch
        {
            0 => Ok(BuildResponse(parts, layouts, sheetDetails,
                     numPlates, lastPlateSurplus, utilization, stopwatch.Elapsed.TotalMilliseconds)),
            1 => BadRequest(new ValidationErrorResponse
            {
                Errors = { "套料引擎报告参数非法（nesting_run 返回 1）。" }
            }),
            2 => StatusCode(StatusCodes.Status500InternalServerError, new
            {
                message = "套料引擎内部异常（nesting_run 返回 2）。"
            }),
            _ => StatusCode(StatusCodes.Status500InternalServerError, new
            {
                message = $"套料引擎返回未知状态码：{ret}。"
            })
        };
    }

    private static RunNestingResponse BuildResponse(
        List<PartItem> parts,
        double[] layouts, double[] sheetDetails,
        double numPlates, double lastPlateSurplus, double utilization, double runtimeMs)
    {
        int n = parts.Count;

        // ---- 按板号分组。layouts 行号是放置顺序，零件身份以 col1（输入 1-based
        // 序号）为准；不能假定行号 j 对应 parts[j]，也不能用 partIsNested
        // （Nest_and_reNest 内部将其复用为临时缓冲，输出并非可靠的逐零件标志）。----
        var sheets = new Dictionary<int, NestedSheet>();
        var placed = new bool[n];
        int nestedCount = 0;

        for (int j = 0; j < n; j++)
        {
            int baseIdx = j * LayoutCols;
            double sheetNoRaw = layouts[baseIdx]; // col0
            if (sheetNoRaw <= 0)
                continue;

            int inputIdx = (int)Math.Round(layouts[baseIdx + 1]) - 1; // col1
            if (inputIdx < 0 || inputIdx >= n || placed[inputIdx])
                continue;
            placed[inputIdx] = true;
            nestedCount++;

            int sheetNo = (int)Math.Round(sheetNoRaw);
            if (!sheets.TryGetValue(sheetNo, out var sheet))
            {
                sheet = new NestedSheet { SheetNo = sheetNo };
                sheets.Add(sheetNo, sheet);
            }

            sheet.Parts.Add(new NestedSheetPart
            {
                Id = parts[inputIdx].Id,
                X = layouts[baseIdx + 2],      // col2
                Y = layouts[baseIdx + 3],     // col3
                Length = layouts[baseIdx + 4], // col4
                Width = layouts[baseIdx + 5]   // col5
            });
        }

        var unplaced = new List<PartItem>();
        for (int i = 0; i < n; i++)
        {
            if (!placed[i])
                unplaced.Add(parts[i]);
        }

        // ---- sheetDetails 同样逐行连续存储（每行 4 列）：行=板，col0=板号 / col1=余料长 / col2=余料宽 / col3=成材率 ----
        foreach (var sheet in sheets.Values)
        {
            int baseIdx = (sheet.SheetNo - 1) * SheetCols;
            sheet.SurplusLength = sheetDetails[baseIdx + 1];
            sheet.SurplusWidth = sheetDetails[baseIdx + 2];
            sheet.Utilization = sheetDetails[baseIdx + 3];
        }

        return new RunNestingResponse
        {
            RuntimeMs = runtimeMs,
            NumPlates = numPlates,
            Utilization = utilization,
            LastPlateSurplusLength = lastPlateSurplus,
            PartsTotal = n,
            PartsNestedCount = nestedCount,
            Unplaced = unplaced,
            Sheets = sheets.Values.OrderBy(s => s.SheetNo).ToList()
        };
    }
}
