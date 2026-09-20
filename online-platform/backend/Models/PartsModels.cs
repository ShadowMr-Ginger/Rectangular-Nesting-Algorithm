namespace Backend.Models;

public class PartItem
{
    public int Id { get; set; }
    public double Length { get; set; }
    public double Width { get; set; }
}

public class GeneratePartsRequest : PlateParameters
{
    public int? Count { get; set; }

    public const int CountMin = 1;
    public const int CountMax = 100000;
}

public class GeneratePartsResponse
{
    public List<PartItem> Parts { get; set; } = new();
    public double PlateLength { get; set; }
    public double PlateWidth { get; set; }
    public double EdgeDistance { get; set; }
}

/// <summary>400 校验错误的统一响应。</summary>
public class ValidationErrorResponse
{
    public string Message { get; set; } = "请求参数不合法。";
    public List<string> Errors { get; set; } = new();
}
