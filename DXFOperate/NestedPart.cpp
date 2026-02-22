#include "NestedPart.h"

NestedPart::NestedPart(Part part) 
{
	this->Code = part.Code;
	this->Material = part.Material;

	this->Length = part.Length;
	this->Height = part.Height;
}

NestedPart::NestedPart(Part part, double partPositionX, double partPositionY) 
{
	this->Code = part.Code;

	this->Material = part.Material;

	this->Length = part.Length;
	this->Height = part.Height;

	this->PartPositionX = partPositionX;
	this->PartPositionY = partPositionY;
}
