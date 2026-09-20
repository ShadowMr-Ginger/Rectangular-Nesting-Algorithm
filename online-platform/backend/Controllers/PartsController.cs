using Backend.Models;
using Microsoft.AspNetCore.Mvc;

namespace Backend.Controllers;

[ApiController]
[Route("api/parts")]
public class PartsController : ControllerBase
{
    private static readonly Random Rng = new();

    /// <summary>
    /// 生成随机矩形件。分布上小件偏多、大件偏少：
    /// 尺寸 = maxSize × u^2.5（u 为 (0,1) 均匀随机），指数 2.5 使概率密度向小尺寸集中。
    /// </summary>
    [HttpPost("generate")]
    public ActionResult<GeneratePartsResponse> Generate([FromBody] GeneratePartsRequest request)
    {
        var errors = request.ValidateAndApplyDefaults();

        int count = request.Count ?? GeneratePartsRequest.DefaultCount;
        if (count < GeneratePartsRequest.CountMin || count > GeneratePartsRequest.CountMax)
            errors.Add($"数量必须是 {GeneratePartsRequest.CountMin}..{GeneratePartsRequest.CountMax} 之间的整数，当前值：{count}。");

        if (errors.Count > 0)
            return BadRequest(new ValidationErrorResponse { Errors = errors });

        double plateLength = request.PlateLength!.Value;
        double plateWidth = request.PlateWidth!.Value;
        double edge = request.EdgeDistance!.Value;

        double maxL = plateLength - 2 * edge;
        double maxW = plateWidth - 2 * edge;
        double minSize = Math.Max(1.0, Math.Min(plateLength, plateWidth) * 0.02);

        var parts = new List<PartItem>(count);
        for (int i = 1; i <= count; i++)
        {
            // min + (max-min)*u^2.5：小件偏多大件偏少，且避免大量件被钳到 minSize
            double len = minSize + (maxL - minSize) * Math.Pow(NextOpenUnit(), 2.5);
            double wid = minSize + (maxW - minSize) * Math.Pow(NextOpenUnit(), 2.5);

            // 约 30% 概率交换长宽；仅当交换后不超出 maxL/maxW 才进行，
            // 多数件仍保持长 ≥ 宽更自然的取向。
            if (Rng.NextDouble() < 0.3 && wid <= maxL && len <= maxW)
                (len, wid) = (wid, len);

            parts.Add(new PartItem { Id = i, Length = len, Width = wid });
        }

        return new GeneratePartsResponse
        {
            Parts = parts,
            PlateLength = plateLength,
            PlateWidth = plateWidth,
            EdgeDistance = edge
        };
    }

    /// <summary>取 (0,1) 开区间均匀随机数，避免 pow(0, 2.5) 边界。</summary>
    private static double NextOpenUnit()
    {
        double u;
        do { u = Rng.NextDouble(); } while (u <= 0.0 || u >= 1.0);
        return u;
    }
}
