#pragma once
#include <string>

using namespace std;


class Plate
{
private:

public:
	//原片材质, 原片序号, 产品id, 产品x坐标, 产品y坐标, 产品x方向长度, 产品y方向长度
	int Code;
	string Material;
	double Length;
	double Height;

	//Plate() = default;
	Plate() = default;
	Plate(string material, int Code, double Length, double Height);
};


