#include "Part.h"

Part::Part() 
{
	//this->Code = Code;
	this->Length = 0;
	this->Height = 0;
}

Part::Part(string material, int Code, double Length, double Height)
{
	this->Material = material;
	this->Code = Code;
	this->Length = Length;
	this->Height = Height;
}