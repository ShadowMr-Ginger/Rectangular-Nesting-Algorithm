#pragma once

#ifndef RECTANGLENESTTASK_H_
#define RECTANGLENESTTASK_H_
#endif

#include <list>;
#include "RectanglePart.h"
#include "rtwtypes.h"
#include <stddef.h>
#include <stdlib.h>
#include <Windows.h>
#include "CSV.hpp"
#include "Nest_and_reNest.h"
#include "Nest_and_reNest_emxAPI.h"
#include "Nest_and_reNest_terminate.h"
#include "Nest_and_reNest_types.h"
#include "rt_nonfinite.h"
#include <iostream>
#include "Layout.h"
#include "LayoutRecord.h"
#include "dl_creationadapter.h"
#include "dl_dxf.h"
#include "LayoutsToDXF.h"
#include "iniparser.hpp"


class _declspec(dllexport) RectangleNestTask;
class RectangleNestTask
{
public:
	// 排样前的属性
	list<RectanglePart> partList_toNest;
	double plateLength;
	double plateWidth;
	int num_product;

	// 排样后的属性
	emxArray_real_T* layouts;
	emxArray_real_T* sheetDetails;
	emxArray_real_T* partIsNested;
	double num_plate;
	double lastPlateSurplusLength;
	double utilization;

	//算法参数（从ini文件读入）
	double tightnessCoef;
	double S_coef;
	double reNestStepLength;
	double interval_distance_parts;
	double interval_distance_plate;

	//构造函数
	RectangleNestTask() = default;
	RectangleNestTask(list<RectanglePart> partList_toNest, double plateLength, double plateWidth);

	//方法
	void addPart(string materialType, int partNumber, double partLength, double partWidth);
	void addPart(RectanglePart rectanglePart);
	void load_parts_from_CSV(string filename);
	void PerformNestingTask();
	void OutputCSV();
	void OutputDXF();
	void writeINI(double tightnessCoef,
		double S_coef,
		double reNestStepLength,
		double interval_distance_parts,
		double interval_distance_plate);
	void readINI();

};

