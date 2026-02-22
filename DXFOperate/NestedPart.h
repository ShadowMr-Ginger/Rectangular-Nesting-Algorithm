#pragma once
#include "Part.h"

class NestedPart :
    public Part
{
public:
    double PartPositionX;
    double PartPositionY;

    NestedPart() = default;
    NestedPart(Part part);
    NestedPart(Part part,double partPositionX,double partPositionY);
};

