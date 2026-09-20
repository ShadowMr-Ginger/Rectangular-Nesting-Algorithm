export interface Part {
  id: number;
  length: number;
  width: number;
}

export interface SheetPart {
  partId: number;
  x: number;
  y: number;
  length: number;
  width: number;
}

export interface Sheet {
  sheetNo: number;
  utilization: number; // 0~100
  parts: SheetPart[];
}

export interface UnplacedPart {
  id: number;
  length: number;
  width: number;
}

export interface NestingResult {
  runtimeMs: number;
  utilization: number; // 0~100
  sheetCount: number;
  totalParts: number;
  placedCount: number;
  unplacedCount: number;
  unplaced: UnplacedPart[];
  remainingLength: number | null; // 末板余料长度
  sheets: Sheet[];
}

export interface PlateParams {
  plateLength: number;
  plateWidth: number;
  edgeDistance: number;
}

export interface ImportWarning {
  line: number;
  length: number;
  width: number;
  reason: string;
}
