#pragma once
#include <string>

using namespace std;

class LayoutRecord
{
private:

public:
	//原片材质, 原片序号, 产品id, 产品x坐标, 产品y坐标, 产品x方向长度, 产品y方向长度
	string Material;
	int PlateCode;
	int PartCode;
	double PartPositionX;
	double PartPositionY;
	double PartLength;
	double PartHeight;

	LayoutRecord(string material,int plateCode,int partCode,double partPostionX,double partPostionY,double partLength,double partHeight);
};

