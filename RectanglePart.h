#pragma once
#ifndef RECTANGLEPART_H_
#define RECTANGLEPART_H_
#endif
#include <string>

using namespace std;


class _declspec(dllexport) RectanglePart;
class RectanglePart
{
public:
	string materialType;
	int partNumber;
	double partLength;
	double partWidth;
	RectanglePart() = default;
	RectanglePart(string materialType, int partNumber, double partLength, double partWidth);
};

