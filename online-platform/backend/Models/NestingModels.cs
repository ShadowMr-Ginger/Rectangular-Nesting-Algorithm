using Backend.Models;

namespace Backend.Models;

public class RunNestingRequest : PlateParameters
{
    public List<PartItem>? Parts { get; set; }
}

public class UnplaceablePart
{
    public int Index { get; set; }
    public int Id { get; set; }
    public double Length { get; set; }
    public double Width { get; set; }
}

public class UnplaceableResponse
{
    public string Message { get; set; } = "存在无法放入原板的矩形件";
    public List<UnplaceablePart> Unplaceable { get; set; } = new();
}

public class NestedSheetPart
{
    public int Id { get; set; }
    public double X { get; set; }
    public double Y { get; set; }
    public double Length { get; set; }
    public double Width { get; set; }
}

public class NestedSheet
{
    public int SheetNo { get; set; }
    public double Utilization { get; set; }
    public double SurplusLength { get; set; }
    public double SurplusWidth { get; set; }
    public List<NestedSheetPart> Parts { get; set; } = new();
}

public class RunNestingResponse
{
    public double RuntimeMs { get; set; }
    public double NumPlates { get; set; }
    public double Utilization { get; set; }
    public double LastPlateSurplusLength { get; set; }
    public int PartsTotal { get; set; }
    public int PartsNestedCount { get; set; }
    public List<PartItem> Unplaced { get; set; } = new();
    public List<NestedSheet> Sheets { get; set; } = new();
}
