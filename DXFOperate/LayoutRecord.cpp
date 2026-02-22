#include "LayoutRecord.h"

//LayoutRecord::LayoutRecord()
//{
//}

LayoutRecord::LayoutRecord(string material, int plateCode, int partCode, double partPositionX, double partPositionY, double partLength, double partHeight)
{
	this->Material = material;
	this->plate.Code = plateCode;

	this->part.Code = partCode;
	this->part.Length = partLength;
	this->part.Height = partHeight;
	this->part.PartPositionX = partPositionX;
	this->part.PartPositionY = partPositionY;
}

LayoutRecord::LayoutRecord(Plate plate, NestedPart part, double partPositionX, double partPositionY)
{
	this->Material = plate.Material;
	this->plate = plate;
	this->part = part;
	this->part.PartPositionX = partPositionX;
	this->part.PartPositionY = partPositionY;
}