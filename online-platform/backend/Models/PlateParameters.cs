using System.ComponentModel.DataAnnotations;

namespace Backend.Models;

/// <summary>板材与边缘距离的公共参数（generate / run 共用校验）。</summary>
public class PlateParameters
{
    public double? PlateLength { get; set; }
    public double? PlateWidth { get; set; }
    public double? EdgeDistance { get; set; }

    public const double DefaultPlateLength = 100.0;
    public const double DefaultPlateWidth = 50.0;
    public const int DefaultCount = 1000;
    public const double DefaultEdgeDistance = 0.2;

    /// <summary>板材长/宽上限。</summary>
    public const double PlateMax = 100000.0;

    /// <summary>
    /// 校验板材长宽与边缘距离。返回错误消息列表（空 = 合法）。
    /// 应用默认值后再校验，保证后端独立调用也安全。
    /// </summary>
    public List<string> ValidateAndApplyDefaults()
    {
        PlateLength ??= DefaultPlateLength;
        PlateWidth ??= DefaultPlateWidth;
        EdgeDistance ??= DefaultEdgeDistance;

        var errors = new List<string>();

        if (!IsValidPlateSize(PlateLength.Value))
            errors.Add($"板材长度必须是有限数且 0 < 长度 ≤ {PlateMax}，当前值：{PlateLength.Value}。");
        if (!IsValidPlateSize(PlateWidth.Value))
            errors.Add($"板材宽度必须是有限数且 0 < 宽度 ≤ {PlateMax}，当前值：{PlateWidth.Value}。");

        if (!double.IsFinite(EdgeDistance.Value) || EdgeDistance.Value < 0)
        {
            errors.Add($"边缘距离必须是非负有限数，当前值：{EdgeDistance.Value}。");
        }
        else if (errors.Count == 0 && 2 * EdgeDistance.Value >= Math.Min(PlateLength.Value, PlateWidth.Value))
        {
            errors.Add($"边缘距离过大：需满足 2 × 边缘距离 < min(板材长, 板材宽)，当前 2 × {EdgeDistance.Value} ≥ {Math.Min(PlateLength.Value, PlateWidth.Value)}。");
        }

        return errors;
    }

    private static bool IsValidPlateSize(double v) => double.IsFinite(v) && v > 0 && v <= PlateMax;
}
