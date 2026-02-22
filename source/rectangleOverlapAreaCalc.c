/*
 * File: rectangleOverlapAreaCalc.c
 *
 * MATLAB Coder version            : 5.4
 * C/C++ source code generated on  : 01-Sep-2023 10:52:31
 */

/* Include Files */
#include "rectangleOverlapAreaCalc.h"
#include "rt_nonfinite.h"
#include "sort.h"
#include <math.h>

/* Function Definitions */
/*
 * 计算矩形重叠面积
 *  输入：
 *     rec1  [矩形1的左上角x，左上角y,右下角x，右下角y]
 *     rec2  [矩形2的左上角x，左上角y,右下角x，右下角y]
 *  输出：重叠面积
 *
 * Arguments    : const double rec1[4]
 *                const double rec2[4]
 * Return Type  : double
 */
double rectangleOverlapAreaCalc(const double rec1[4], const double rec2[4])
{
  double overlapArea;
  if (rec1[0] + 0.0001 > rec2[2]) {
    overlapArea = 0.0;
  } else if (rec1[2] - 0.0001 < rec2[0]) {
    overlapArea = 0.0;
  } else if (rec1[3] + 0.0001 > rec2[1]) {
    overlapArea = 0.0;
  } else if (rec1[1] - 0.0001 < rec2[3]) {
    overlapArea = 0.0;
  } else {
    double xList[4];
    double yList[4];
    xList[0] = rec1[0];
    xList[1] = rec1[2];
    xList[2] = rec2[0];
    xList[3] = rec2[2];
    sort(xList);
    yList[0] = rec1[1];
    yList[1] = rec1[3];
    yList[2] = rec2[1];
    yList[3] = rec2[3];
    sort(yList);
    overlapArea = fabs(xList[2] - xList[1]) * fabs(yList[2] - yList[1]);
  }
  return overlapArea;
}

/*
 * File trailer for rectangleOverlapAreaCalc.c
 *
 * [EOF]
 */
