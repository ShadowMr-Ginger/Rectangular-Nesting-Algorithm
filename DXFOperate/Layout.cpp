#include "Layout.h"

Layout::Layout(Plate plate, list<NestedPart> nestedParts) 
{
	this->plate = plate;
	this->nestedPart = nestedParts;
}


Layout::Layout(Plate plate, NestedPart nestedPart) 
{
	this->plate = plate;
	this->nestedPart.push_back(nestedPart);
}
