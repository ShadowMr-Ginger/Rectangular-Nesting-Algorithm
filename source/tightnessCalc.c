/*
 * File: tightnessCalc.c
 *
 * MATLAB Coder version            : 5.4
 * C/C++ source code generated on  : 01-Sep-2023 10:52:31
 */

/* Include Files */
#include "tightnessCalc.h"
#include "Nest_and_reNest_types.h"
#include "rectangleOverlapAreaCalc.h"
#include "rt_nonfinite.h"

/* Function Definitions */
/*
 * 紧密度计算函数
 *  输入参数解释：
 *  rectangle_current
 * 当前矩形,格式[左上角顶点x,左上角顶点y,右下角顶点x,右下角顶点y];
 *  currentNestedPartsList 当前排料的矩形件列表
 *  currentNestedParts_pointer 上述变量的指针
 *  tightnessCoef  紧密度系数，用于计算紧密度大小时辐射矩形的距离
 *  主体内容
 *
 * Arguments    : const double rectangleCurrent[4]
 *                const emxArray_real_T *currentNestedPartsList
 *                double currentNestedParts_pointer
 *                double tightnessCoef
 *                double plateLength
 *                double plateWidth
 *                double S_coef
 * Return Type  : double
 */
double tightnessCalc(const double rectangleCurrent[4],
                     const emxArray_real_T *currentNestedPartsList,
                     double currentNestedParts_pointer, double tightnessCoef,
                     double plateLength, double plateWidth, double S_coef)
{
  double RadicalEdgeList[16];
  double RadicalRecList[16];
  const double *currentNestedPartsList_data;
  double b_tightness_tmp;
  double tightness;
  double tightness_tmp;
  int i;
  int j;
  currentNestedPartsList_data = currentNestedPartsList->data;
  tightness = 0.0;
  /*  建立辐射矩形列表4*4列表，分别代表上下左右四个辐射矩形的左上、右下顶点xyxy
   */
  RadicalRecList[0] = rectangleCurrent[0];
  RadicalRecList[4] = rectangleCurrent[1] + tightnessCoef;
  RadicalRecList[8] = rectangleCurrent[2];
  RadicalRecList[12] = rectangleCurrent[1];
  RadicalRecList[1] = rectangleCurrent[0];
  RadicalRecList[5] = rectangleCurrent[3];
  RadicalRecList[9] = rectangleCurrent[2];
  RadicalRecList[13] = rectangleCurrent[3] - tightnessCoef;
  RadicalRecList[2] = rectangleCurrent[0] - tightnessCoef;
  RadicalRecList[6] = rectangleCurrent[1];
  RadicalRecList[10] = rectangleCurrent[0];
  RadicalRecList[14] = rectangleCurrent[3];
  RadicalRecList[3] = rectangleCurrent[2];
  RadicalRecList[7] = rectangleCurrent[1];
  RadicalRecList[11] = rectangleCurrent[2] + tightnessCoef;
  RadicalRecList[15] = rectangleCurrent[3];
  RadicalEdgeList[0] = plateWidth + tightnessCoef;
  RadicalEdgeList[4] = 0.0;
  RadicalEdgeList[8] = plateLength;
  RadicalEdgeList[12] = plateWidth;
  RadicalEdgeList[1] = 0.0;
  RadicalEdgeList[5] = 0.0;
  RadicalEdgeList[9] = plateLength;
  RadicalEdgeList[13] = -tightnessCoef;
  RadicalEdgeList[2] = -tightnessCoef;
  RadicalEdgeList[6] = plateWidth;
  RadicalEdgeList[10] = 0.0;
  RadicalEdgeList[14] = 0.0;
  RadicalEdgeList[3] = plateLength;
  RadicalEdgeList[7] = plateWidth;
  RadicalEdgeList[11] = plateLength + tightnessCoef;
  RadicalEdgeList[15] = 0.0;
  for (i = 0; i < 4; i++) {
    double b_RadicalRecList[4];
    double b_currentNestedPartsList[4];
    double d;
    if (currentNestedParts_pointer > 0.0) {
      int b_i;
      b_i = (int)currentNestedParts_pointer;
      tightness_tmp = RadicalRecList[i];
      for (j = 0; j < b_i; j++) {
        /*  判断两个矩形是否相互相交 */
        /*  rec1\rec2格式：[左上角顶点x，左上角顶点y,右下角顶点x，右下角顶点y]
         */
        if ((!(tightness_tmp + 0.0001 >
               currentNestedPartsList_data[j + currentNestedPartsList->size[0] *
                                                   2])) &&
            (!(RadicalRecList[i + 8] - 0.0001 <
               currentNestedPartsList_data[j]))) {
          b_tightness_tmp = RadicalRecList[i + 12];
          if (!(b_tightness_tmp + 0.0001 >
                currentNestedPartsList_data[j +
                                            currentNestedPartsList->size[0]])) {
            d = RadicalRecList[i + 4];
            if (!(d - 0.0001 < currentNestedPartsList_data
                                   [j + currentNestedPartsList->size[0] * 3])) {
              b_RadicalRecList[0] = tightness_tmp;
              b_currentNestedPartsList[0] = currentNestedPartsList_data[j];
              b_RadicalRecList[1] = d;
              b_currentNestedPartsList[1] =
                  currentNestedPartsList_data[j +
                                              currentNestedPartsList->size[0]];
              b_RadicalRecList[2] = RadicalRecList[i + 8];
              b_currentNestedPartsList[2] =
                  currentNestedPartsList_data[j +
                                              currentNestedPartsList->size[0] *
                                                  2];
              b_RadicalRecList[3] = b_tightness_tmp;
              b_currentNestedPartsList[3] =
                  currentNestedPartsList_data[j +
                                              currentNestedPartsList->size[0] *
                                                  3];
              tightness += rectangleOverlapAreaCalc(b_RadicalRecList,
                                                    b_currentNestedPartsList);
            }
          }
        }
      }
    }
    /*  判断两个矩形是否相互相交 */
    /*  rec1\rec2格式：[左上角顶点x，左上角顶点y,右下角顶点x，右下角顶点y] */
    tightness_tmp = RadicalEdgeList[i + 8];
    b_tightness_tmp = RadicalRecList[i];
    if (!(b_tightness_tmp + 0.0001 > tightness_tmp)) {
      d = RadicalRecList[i + 8];
      if (!(d - 0.0001 < RadicalEdgeList[i])) {
        double d1;
        double d2;
        d1 = RadicalEdgeList[i + 4];
        d2 = RadicalRecList[i + 12];
        if (!(d2 + 0.0001 > d1)) {
          double d3;
          double d4;
          d3 = RadicalRecList[i + 4];
          d4 = RadicalEdgeList[i + 12];
          if (!(d3 - 0.0001 < d4)) {
            b_RadicalRecList[0] = b_tightness_tmp;
            b_currentNestedPartsList[0] = RadicalEdgeList[i];
            b_RadicalRecList[1] = d3;
            b_currentNestedPartsList[1] = d1;
            b_RadicalRecList[2] = d;
            b_currentNestedPartsList[2] = tightness_tmp;
            b_RadicalRecList[3] = d2;
            b_currentNestedPartsList[3] = d4;
            tightness += rectangleOverlapAreaCalc(b_RadicalRecList,
                                                  b_currentNestedPartsList);
          }
        }
      }
    }
  }
  tightness_tmp = rectangleCurrent[1] - rectangleCurrent[3];
  b_tightness_tmp = rectangleCurrent[2] - rectangleCurrent[0];
  tightness = (tightness * 2.0 - tightnessCoef * tightness_tmp) -
              tightnessCoef * b_tightness_tmp;
  tightness += S_coef * tightness_tmp * b_tightness_tmp;
  return tightness;
}

/*
 * File trailer for tightnessCalc.c
 *
 * [EOF]
 */
