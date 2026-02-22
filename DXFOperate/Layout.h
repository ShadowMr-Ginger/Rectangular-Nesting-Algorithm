#pragma once
#include <list>
#include "NestedPart.h"
#include "Plate.h"

//using namespace std;

class Layout
{
public:
	Plate plate;
	list<NestedPart> nestedPart;

	Layout() = default;
	Layout(Plate plate, list<NestedPart> nestedParts);
	Layout(Plate plate, NestedPart nestedPart);
};

