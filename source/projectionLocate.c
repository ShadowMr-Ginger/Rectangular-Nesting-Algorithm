/*
 * File: projectionLocate.c
 *
 * MATLAB Coder version            : 5.4
 * C/C++ source code generated on  : 01-Sep-2023 10:52:31
 */

/* Include Files */
#include "projectionLocate.h"
#include "Nest_and_reNest_types.h"
#include "rt_nonfinite.h"

/* Function Definitions */
/*
 * 投影点坐标计算函数
 *  输入参数：
 *  originPoint 起始点坐标[x,y]
 *  projectionMode 投影模式（取值1左投，2下投）
 *  CurrentNestedPartsList 当前已套料矩形列表
 *  currentNestedParts_pointer 上述变量的指针
 *  函数主体：
 *
 * Arguments    : const double originPoint[2]
 *                double projectionMode
 *                const emxArray_real_T *currentNestedPartsList
 *                double currentNestedParts_pointer
 *                double *x
 *                double *y
 * Return Type  : void
 */
void projectionLocate(const double originPoint[2], double projectionMode,
                      const emxArray_real_T *currentNestedPartsList,
                      double currentNestedParts_pointer, double *x, double *y)
{
  const double *currentNestedPartsList_data;
  double dist;
  int b_i;
  currentNestedPartsList_data = currentNestedPartsList->data;
  dist = rtInf;
  *x = 0.0;
  *y = 0.0;
  if (projectionMode == 1.0) {
    int i;
    /*  左投 */
    *y = originPoint[1];
    i = (int)currentNestedParts_pointer;
    for (b_i = 0; b_i < i; b_i++) {
      double d;
      /* 条件1 左右分离 且距离更近 */
      d = currentNestedPartsList_data[b_i +
                                      currentNestedPartsList->size[0] * 2];
      if (d + 0.0001 < originPoint[0]) {
        double d1;
        d1 = originPoint[0] - d;
        if ((d1 < dist) &&
            (currentNestedPartsList_data[b_i +
                                         currentNestedPartsList->size[0]] -
                 0.0001 >
             originPoint[1]) &&
            (currentNestedPartsList_data[b_i +
                                         currentNestedPartsList->size[0] * 3] -
                 0.0001 <
             originPoint[1])) {
          /*  条件2 上下包含 */
          dist = d1;
          *x = d;
        }
      }
    }
  } else {
    int i;
    /*  下投 */
    *x = originPoint[0];
    i = (int)currentNestedParts_pointer;
    for (b_i = 0; b_i < i; b_i++) {
      double d;
      /* 条件1 上下分离 且距离更近 */
      d = currentNestedPartsList_data[b_i + currentNestedPartsList->size[0]];
      if (d + 0.0001 < originPoint[1]) {
        double d1;
        d1 = originPoint[1] - d;
        if ((d1 < dist) &&
            (currentNestedPartsList_data[b_i +
                                         currentNestedPartsList->size[0] * 2] -
                 0.0001 >
             originPoint[0]) &&
            (currentNestedPartsList_data[b_i] - 0.0001 < originPoint[0])) {
          /*  条件2 左右包含 */
          dist = d1;
          *y = d;
        }
      }
    }
  }
}

/*
 * File trailer for projectionLocate.c
 *
 * [EOF]
 */
