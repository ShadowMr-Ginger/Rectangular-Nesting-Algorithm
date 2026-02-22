#include "RectangleNestTask.h"

int main() {
	RectangleNestTask task1 = RectangleNestTask::RectangleNestTask();
	task1.load_parts_from_CSV("dataA1.csv");
	task1.plateLength = 2440;
	task1.plateWidth = 1220;
	//可调参数如下。以下三个参数的取值会直接影响最终成材率的大小
	double tightnessCoef = 50; //紧密度系数。取值最好大于方形件的最短边长
	double S_coef = 0.1;       //板面积权重系数。该值会影响到方形件排列的顺序。
	//该值越大，面积越大的方形件优先排列；该值越小，紧密度越大的方形件优先排列。
	double reNestStepLength = 40; //重套料的初始步长，影响最后一块板材的余料面积。步长越小计算时间越长，但成材率可能会更高。

	//零件间距和板间距
	double interval_distance_parts = 5;
	double interval_distance_plate = 5;

	task1.writeINI(tightnessCoef = 50,
		S_coef = 0.1,
		reNestStepLength = 40,
		interval_distance_parts = 5,
		interval_distance_plate = 5);
	task1.PerformNestingTask();
	task1.OutputCSV();
	task1.OutputDXF();
	//task1.
	


	return 0;
}