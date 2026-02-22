/*
 * File: gapIsTooNarrow.c
 *
 * MATLAB Coder version            : 5.4
 * C/C++ source code generated on  : 01-Sep-2023 10:52:31
 */

/* Include Files */
#include "gapIsTooNarrow.h"
#include "Nest_and_reNest_types.h"
#include "rt_nonfinite.h"

/* Function Definitions */
/*
 * 检查顶点所在空间是否过于狭窄
 *  输入参数：
 *  vertex 顶点[x,y]
 *  currentNestedPartsList 目前已套料的矩形列表
 *  currentNestedParts_pointer 上述变量的指针
 *  widthLimit,lengthLimit 长宽限制
 *
 * Arguments    : const double vertex[2]
 *                const emxArray_real_T *currentNestedPartsList
 *                double currentNestedParts_pointer
 *                double lengthLimit
 *                double widthLimit
 *                double plateLength
 *                double platewidth
 * Return Type  : double
 */
double gapIsTooNarrow(const double vertex[2],
                      const emxArray_real_T *currentNestedPartsList,
                      double currentNestedParts_pointer, double lengthLimit,
                      double widthLimit, double plateLength, double platewidth)
{
  const double *currentNestedPartsList_data;
  double narrow;
  currentNestedPartsList_data = currentNestedPartsList->data;
  narrow = 0.0;
  if (plateLength - vertex[0] < lengthLimit) {
    narrow = 1.0;
  } else if (platewidth - vertex[1] < widthLimit) {
    narrow = 1.0;
  } else {
    int i;
    bool exitg1;
    i = 0;
    exitg1 = false;
    while ((!exitg1) && (i <= (int)currentNestedParts_pointer - 1)) {
      /* 检查上边界距离 */
      /* 条件1：矩形下边在上方，且距离过小 */
      if ((currentNestedPartsList_data[i +
                                       currentNestedPartsList->size[0] * 3] +
               0.0001 >
           vertex[1]) &&
          (currentNestedPartsList_data[i +
                                       currentNestedPartsList->size[0] * 3] -
               vertex[1] <
           widthLimit) &&
          (currentNestedPartsList_data[i] - 0.0001 < vertex[0]) &&
          (currentNestedPartsList_data[i +
                                       currentNestedPartsList->size[0] * 2] -
               0.0001 >
           vertex[0])) {
        /* 条件2：左右包含 */
        narrow = 1.0;
        exitg1 = true;

        /* 检查右边界距离 */
        /* 条件1：矩形左边在右方，且距离过小 */
      } else if ((currentNestedPartsList_data[i] + 0.0001 > vertex[0]) &&
                 (currentNestedPartsList_data[i] - vertex[0] < lengthLimit) &&
                 (currentNestedPartsList_data
                          [i + currentNestedPartsList->size[0] * 3] -
                      0.0001 <
                  vertex[1]) &&
                 (currentNestedPartsList_data[i +
                                              currentNestedPartsList->size[0]] -
                      0.0001 >
                  vertex[1])) {
        /* 条件2：上下包含 */
        narrow = 1.0;
        exitg1 = true;
      } else {
        i++;
      }
    }
  }
  return narrow;
}

/*
 * File trailer for gapIsTooNarrow.c
 *
 * [EOF]
 */
