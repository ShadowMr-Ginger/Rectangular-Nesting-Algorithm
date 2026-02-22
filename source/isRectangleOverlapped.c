/*
 * File: isRectangleOverlapped.c
 *
 * MATLAB Coder version            : 5.4
 * C/C++ source code generated on  : 01-Sep-2023 10:52:31
 */

/* Include Files */
#include "isRectangleOverlapped.h"
#include "rt_nonfinite.h"

/* Function Definitions */
/*
 * 判断两个矩形是否相互相交
 *  rec1\rec2格式：[左上角顶点x，左上角顶点y,右下角顶点x，右下角顶点y]
 *
 * Arguments    : const double rec1[4]
 *                const double rec2[4]
 * Return Type  : double
 */
double isRectangleOverlapped(const double rec1[4], const double rec2[4])
{
  double overlapped;
  if ((rec1[0] + 0.0001 > rec2[2]) || (rec1[2] - 0.0001 < rec2[0]) ||
      (rec1[3] + 0.0001 > rec2[1]) || (rec1[1] - 0.0001 < rec2[3])) {
    overlapped = 0.0;
  } else {
    overlapped = 1.0;
  }
  return overlapped;
}

/*
 * File trailer for isRectangleOverlapped.c
 *
 * [EOF]
 */
