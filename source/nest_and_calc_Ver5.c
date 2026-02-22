/*
 * File: nest_and_calc_Ver5.c
 *
 * MATLAB Coder version            : 5.4
 * C/C++ source code generated on  : 01-Sep-2023 10:52:31
 */

/* Include Files */
#include "nest_and_calc_Ver5.h"
#include "Nest_and_reNest_emxutil.h"
#include "Nest_and_reNest_types.h"
#include "any.h"
#include "gapIsTooNarrow.h"
#include "isRectangleOverlapped.h"
#include "minOrMax.h"
#include "projectionLocate.h"
#include "rt_nonfinite.h"
#include "tightnessCalc.h"
#include <math.h>

/* Function Definitions */
/*
 * 排料与成材率计算函数（Ver.5 更优）
 *  用于指定板材顺序的情况下进行排料并计算成材率
 *  输入参数：
 *     partsSize：板件尺寸(长，宽)
 *     plateLength_Width：原料板的长宽
 *     interval_distance_parts  零件间距
 *     interval_distancec_plate 板间距
 *  输出参数：
 *     layouts：板件的布局（sheet编号，板件编号，左下顶点坐标x，y，x方向长度，y方向长度）
 *     sheetDetails：每张原料的细节（余料，成材率）
 *     num_plate 消耗原材料数量
 *     utilization：整体成材率
 *
 * Arguments    : emxArray_real_T *partsSize
 *                const double plateLength_Width[2]
 *                double tightnessCoef
 *                double interval_distance_parts
 *                double interval_distance_plate
 *                double S_coef
 *                emxArray_real_T *layouts
 *                emxArray_real_T *sheetDetails
 *                double *num_plate
 *                double *utilization
 *                emxArray_real_T *partIsNested
 * Return Type  : void
 */
void nest_and_calc_Ver5(emxArray_real_T *partsSize,
                        const double plateLength_Width[2], double tightnessCoef,
                        double interval_distance_parts,
                        double interval_distance_plate, double S_coef,
                        emxArray_real_T *layouts, emxArray_real_T *sheetDetails,
                        double *num_plate, double *utilization,
                        emxArray_real_T *partIsNested)
{
  static const signed char iv[5] = {0, 0, 0, 0, 1};
  emxArray_real_T *b_best_nest_list;
  emxArray_real_T *best_nest_list;
  emxArray_real_T *currentNestedPartsList;
  emxArray_real_T *currentVertexList;
  double d;
  double layout_pointer;
  double left_bottom_suppplement;
  double lengthLimit;
  double plateLength;
  double plateWidth;
  double surplusLength;
  double surplusLength_tmp;
  double tightness;
  double *b_best_nest_list_data;
  double *best_nest_list_data;
  double *currentNestedPartsList_data;
  double *currentVertexList_data;
  double *layouts_data;
  double *partIsNested_data;
  double *partsSize_data;
  double *sheetDetails_data;
  int b_i;
  int h;
  int i;
  int j;
  int num_parts;
  int overlapped;
  left_bottom_suppplement = interval_distance_plate - interval_distance_parts;
  if (interval_distance_plate > interval_distance_parts) {
    plateLength = (plateLength_Width[0] - 2.0 * interval_distance_plate) +
                  interval_distance_parts;
    plateWidth = (plateLength_Width[1] - 2.0 * interval_distance_plate) +
                 interval_distance_parts;
  } else {
    plateLength = plateLength_Width[0] - interval_distance_plate;
    plateWidth = plateLength_Width[1] - interval_distance_plate;
  }
  overlapped = partsSize->size[0] * 2;
  i = partsSize->size[0] * partsSize->size[1];
  partsSize->size[1] = 2;
  emxEnsureCapacity_real_T(partsSize, i);
  partsSize_data = partsSize->data;
  for (i = 0; i < overlapped; i++) {
    partsSize_data[i] += interval_distance_parts;
  }
  /*  初始化输出结果 */
  num_parts = partsSize->size[0];
  i = layouts->size[0] * layouts->size[1];
  layouts->size[0] = partsSize->size[0];
  layouts->size[1] = 6;
  emxEnsureCapacity_real_T(layouts, i);
  layouts_data = layouts->data;
  overlapped = partsSize->size[0] * 6;
  for (i = 0; i < overlapped; i++) {
    layouts_data[i] = 0.0;
  }
  layout_pointer = 0.0;
  /* 上述变量的指针 */
  i = sheetDetails->size[0] * sheetDetails->size[1];
  sheetDetails->size[0] = partsSize->size[0];
  sheetDetails->size[1] = 4;
  emxEnsureCapacity_real_T(sheetDetails, i);
  sheetDetails_data = sheetDetails->data;
  overlapped = partsSize->size[0] << 2;
  for (i = 0; i < overlapped; i++) {
    sheetDetails_data[i] = 0.0;
  }
  /*  初始化中间变量 */
  *num_plate = 0.0;
  i = partIsNested->size[0];
  partIsNested->size[0] = partsSize->size[0];
  emxEnsureCapacity_real_T(partIsNested, i);
  partIsNested_data = partIsNested->data;
  overlapped = partsSize->size[0];
  for (i = 0; i < overlapped; i++) {
    partIsNested_data[i] = 0.0;
  }
  /*  记录最小的板件长和宽 */
  lengthLimit = rtInf;
  i = partsSize->size[0];
  for (b_i = 0; b_i < i; b_i++) {
    d = partsSize_data[b_i];
    if (lengthLimit > d) {
      lengthLimit = d;
    }
    d = partsSize_data[b_i + partsSize->size[0]];
    if (lengthLimit > d) {
      lengthLimit = d;
    }
  }
  emxInit_real_T(&currentNestedPartsList, 2);
  emxInit_real_T(&currentVertexList, 2);
  emxInit_real_T(&best_nest_list, 2);
  emxInit_real_T(&b_best_nest_list, 1);
  int exitg1;
  bool exitg2;
  bool y;
  do {
    exitg1 = 0;
    y = true;
    overlapped = 1;
    exitg2 = false;
    while ((!exitg2) && (overlapped <= partIsNested->size[0])) {
      if (partIsNested_data[overlapped - 1] == 0.0) {
        y = false;
        exitg2 = true;
      } else {
        overlapped++;
      }
    }
    if (!y) {
      double S_efficient;
      double currentNestedParts_pointer;
      double surplusWidth;
      int canContinueNest;
      unsigned int currentVertex_pointer;
      /*     %% Step2.开始套料，新增一个矩形 */
      (*num_plate)++;
      i = currentNestedPartsList->size[0] * currentNestedPartsList->size[1];
      currentNestedPartsList->size[0] = num_parts;
      currentNestedPartsList->size[1] = 9;
      emxEnsureCapacity_real_T(currentNestedPartsList, i);
      currentNestedPartsList_data = currentNestedPartsList->data;
      overlapped = num_parts * 9;
      for (i = 0; i < overlapped; i++) {
        currentNestedPartsList_data[i] = 0.0;
      }
      /*  已套料的矩形表 */
      /*  9列
       * [①左上角顶点坐标x，②左上角顶点坐标y，③右下角顶点x，④右下角顶点y，⑤左顶点是否可选，⑥下顶点是否可选，⑦左投影点是否可选，⑧下投影点是否可选，⑨板子编号]
       */
      currentNestedParts_pointer = 0.0;
      /* 上述变量的指针 */
      S_efficient = 0.0;
      /* 用于计算成材率 */
      i = currentVertexList->size[0] * currentVertexList->size[1];
      currentVertexList->size[0] = 4 * num_parts;
      currentVertexList->size[1] = 5;
      emxEnsureCapacity_real_T(currentVertexList, i);
      currentVertexList_data = currentVertexList->data;
      overlapped = 4 * num_parts * 5;
      for (i = 0; i < overlapped; i++) {
        currentVertexList_data[i] = 0.0;
      }
      /* 过程变量：可选顶点列表 */
      for (i = 0; i < 5; i++) {
        currentVertexList_data[currentVertexList->size[0] * i] = iv[i];
      }
      /* 5列  [①x坐标，②y坐标，③附属矩形，④附属类型，⑤是否可用]   */
      currentVertex_pointer = 1U;
      /* 上述变量的指针 */
      /*  初始化判断变量 */
      canContinueNest = 1;
      while (canContinueNest == 1) {
        double b_currentNestedPartsList[2];
        bool exitg3;
        /* 初始化 所有板件的排样最佳列表 */
        canContinueNest = 0;
        i = best_nest_list->size[0] * best_nest_list->size[1];
        best_nest_list->size[0] = num_parts;
        best_nest_list->size[1] = 8;
        emxEnsureCapacity_real_T(best_nest_list, i);
        best_nest_list_data = best_nest_list->data;
        overlapped = num_parts << 3;
        for (i = 0; i < overlapped; i++) {
          best_nest_list_data[i] = 0.0;
        }
        /*  [是否可排，可排左上顶点x,y，右下x，y，顶点编号,横0or竖1，紧密度]；
         */
        for (b_i = 0; b_i < num_parts; b_i++) {
          if (!(partIsNested_data[b_i] == 1.0)) {
            /*  先calc第一块板件的排样 */
            if (currentNestedParts_pointer == 0.0) {
              for (h = 0; h < 2; h++) {
                double partLength;
                double partWidth;
                if (h == 0) {
                  tightness = partsSize_data[b_i];
                  partLength = tightness;
                  d = partsSize_data[b_i + partsSize->size[0]];
                  partWidth = d;
                } else {
                  d = partsSize_data[b_i + partsSize->size[0]];
                  partLength = d;
                  tightness = partsSize_data[b_i];
                  partWidth = tightness;
                }
                if ((!(partLength > plateLength)) &&
                    (!(partWidth > plateWidth))) {
                  double rectangleCurrent[4];
                  best_nest_list_data[b_i] = 1.0;
                  best_nest_list_data[b_i + best_nest_list->size[0] * 5] = 1.0;
                  if (h == 0) {
                    best_nest_list_data[b_i + best_nest_list->size[0] * 6] =
                        0.0;
                    rectangleCurrent[0] = 0.0;
                    rectangleCurrent[1] = d;
                    rectangleCurrent[2] = tightness;
                    rectangleCurrent[3] = 0.0;
                    best_nest_list_data[b_i + best_nest_list->size[0]] = 0.0;
                    best_nest_list_data[b_i + best_nest_list->size[0] * 2] = d;
                    best_nest_list_data[b_i + best_nest_list->size[0] * 3] =
                        tightness;
                    best_nest_list_data[b_i + best_nest_list->size[0] * 4] =
                        0.0;
                  } else {
                    best_nest_list_data[b_i + best_nest_list->size[0] * 6] =
                        1.0;
                    rectangleCurrent[0] = 0.0;
                    rectangleCurrent[1] = tightness;
                    rectangleCurrent[2] = d;
                    rectangleCurrent[3] = 0.0;
                    best_nest_list_data[b_i + best_nest_list->size[0]] = 0.0;
                    best_nest_list_data[b_i + best_nest_list->size[0] * 2] =
                        tightness;
                    best_nest_list_data[b_i + best_nest_list->size[0] * 3] = d;
                    best_nest_list_data[b_i + best_nest_list->size[0] * 4] =
                        0.0;
                  }
                  best_nest_list_data[b_i + best_nest_list->size[0] * 7] =
                      tightnessCalc(rectangleCurrent, currentNestedPartsList,
                                    0.0, tightnessCoef, plateLength, plateWidth,
                                    S_coef);
                }
              }
            } else {
              /*                 %% Step3.计算套料紧密度 */
              surplusWidth = 0.0;
              /* 记录最大紧密度 */
              for (h = 0; h < 2; h++) {
                double partLength;
                double partWidth;
                if (h == 0) {
                  partLength = partsSize_data[b_i];
                  partWidth = partsSize_data[b_i + partsSize->size[0]];
                } else {
                  partLength = partsSize_data[b_i + partsSize->size[0]];
                  partWidth = partsSize_data[b_i];
                }
                if ((!(partLength > plateLength)) &&
                    (!(partWidth > plateWidth))) {
                  i = (int)currentVertex_pointer;
                  for (j = 0; j < i; j++) {
                    if (!(currentVertexList_data
                              [j + currentVertexList->size[0] * 4] == 0.0)) {
                      /* 判断第j个顶点放置该矩形是否超出矩形框，是则不可排样 */
                      d = currentVertexList_data[j];
                      tightness = d + partLength;
                      if (!(tightness > plateLength)) {
                        surplusLength_tmp =
                            currentVertexList_data[j +
                                                   currentVertexList->size[0]];
                        surplusLength = surplusLength_tmp + partWidth;
                        if (!(surplusLength > plateWidth)) {
                          double rectangleCurrent[4];
                          int k;
                          overlapped = 0;
                          rectangleCurrent[0] = d;
                          rectangleCurrent[1] = surplusLength;
                          rectangleCurrent[2] = tightness;
                          rectangleCurrent[3] = surplusLength_tmp;
                          /* 判断第j个顶点放置该矩形是否与其他矩形交叠，是则不可排样
                           */
                          k = 0;
                          exitg3 = false;
                          while ((!exitg3) &&
                                 (k <= (int)currentNestedParts_pointer - 1)) {
                            double c_currentNestedPartsList[4];
                            c_currentNestedPartsList[0] =
                                currentNestedPartsList_data[k];
                            c_currentNestedPartsList[1] =
                                currentNestedPartsList_data
                                    [k + currentNestedPartsList->size[0]];
                            c_currentNestedPartsList[2] =
                                currentNestedPartsList_data
                                    [k + currentNestedPartsList->size[0] * 2];
                            c_currentNestedPartsList[3] =
                                currentNestedPartsList_data
                                    [k + currentNestedPartsList->size[0] * 3];
                            if (isRectangleOverlapped(
                                    rectangleCurrent,
                                    c_currentNestedPartsList) != 0.0) {
                              overlapped = 1;
                              /* 记录矩形交叠 */
                              exitg3 = true;
                            } else {
                              k++;
                            }
                          }
                          if (overlapped != 1) {
                            /* 可以摆放，计算紧密度 */
                            tightness = tightnessCalc(
                                rectangleCurrent, currentNestedPartsList,
                                currentNestedParts_pointer, tightnessCoef,
                                plateLength, plateWidth, S_coef);
                            if (tightness > surplusWidth) {
                              surplusWidth = tightness;
                              best_nest_list_data[b_i] = 1.0;
                              best_nest_list_data[b_i +
                                                  best_nest_list->size[0] * 5] =
                                  (double)j + 1.0;
                              if (h == 0) {
                                best_nest_list_data[b_i +
                                                    best_nest_list->size[0] *
                                                        6] = 0.0;
                                rectangleCurrent[0] = currentVertexList_data[j];
                                rectangleCurrent[1] =
                                    currentVertexList_data[j + currentVertexList
                                                                   ->size[0]] +
                                    partsSize_data[b_i + partsSize->size[0]];
                                rectangleCurrent[2] =
                                    currentVertexList_data[j] +
                                    partsSize_data[b_i];
                                rectangleCurrent[3] =
                                    currentVertexList_data[j + currentVertexList
                                                                   ->size[0]];
                                best_nest_list_data[b_i +
                                                    best_nest_list->size[0]] =
                                    rectangleCurrent[0];
                                best_nest_list_data[b_i +
                                                    best_nest_list->size[0] *
                                                        2] =
                                    rectangleCurrent[1];
                                best_nest_list_data[b_i +
                                                    best_nest_list->size[0] *
                                                        3] =
                                    rectangleCurrent[2];
                                best_nest_list_data[b_i +
                                                    best_nest_list->size[0] *
                                                        4] =
                                    rectangleCurrent[3];
                              } else {
                                best_nest_list_data[b_i +
                                                    best_nest_list->size[0] *
                                                        6] = 1.0;
                                rectangleCurrent[0] = currentVertexList_data[j];
                                rectangleCurrent[1] =
                                    currentVertexList_data[j + currentVertexList
                                                                   ->size[0]] +
                                    partsSize_data[b_i];
                                rectangleCurrent[2] =
                                    currentVertexList_data[j] +
                                    partsSize_data[b_i + partsSize->size[0]];
                                rectangleCurrent[3] =
                                    currentVertexList_data[j + currentVertexList
                                                                   ->size[0]];
                                best_nest_list_data[b_i +
                                                    best_nest_list->size[0]] =
                                    rectangleCurrent[0];
                                best_nest_list_data[b_i +
                                                    best_nest_list->size[0] *
                                                        2] =
                                    rectangleCurrent[1];
                                best_nest_list_data[b_i +
                                                    best_nest_list->size[0] *
                                                        3] =
                                    rectangleCurrent[2];
                                best_nest_list_data[b_i +
                                                    best_nest_list->size[0] *
                                                        4] =
                                    rectangleCurrent[3];
                              }
                              best_nest_list_data[b_i +
                                                  best_nest_list->size[0] * 7] =
                                  tightnessCalc(
                                      rectangleCurrent, currentNestedPartsList,
                                      currentNestedParts_pointer, tightnessCoef,
                                      plateLength, plateWidth, S_coef);
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
        overlapped = best_nest_list->size[0];
        i = b_best_nest_list->size[0];
        b_best_nest_list->size[0] = best_nest_list->size[0];
        emxEnsureCapacity_real_T(b_best_nest_list, i);
        b_best_nest_list_data = b_best_nest_list->data;
        for (i = 0; i < overlapped; i++) {
          b_best_nest_list_data[i] = best_nest_list_data[i];
        }
        if (any(b_best_nest_list)) {
          /*             %% Step4. 套料，并将顶点、部件信息循环更新 */
          /* 在紧密度最高的顶点上排料。 */
          canContinueNest = 1;
          overlapped = best_nest_list->size[0];
          i = b_best_nest_list->size[0];
          b_best_nest_list->size[0] = best_nest_list->size[0];
          emxEnsureCapacity_real_T(b_best_nest_list, i);
          b_best_nest_list_data = b_best_nest_list->data;
          for (i = 0; i < overlapped; i++) {
            b_best_nest_list_data[i] =
                best_nest_list_data[i + best_nest_list->size[0] * 7];
          }
          maximum(b_best_nest_list, &tightness, &overlapped);
          currentNestedParts_pointer++;
          surplusLength_tmp =
              best_nest_list_data[(overlapped + best_nest_list->size[0]) - 1];
          currentNestedPartsList_data[(int)currentNestedParts_pointer - 1] =
              surplusLength_tmp;
          surplusLength =
              best_nest_list_data[(overlapped + best_nest_list->size[0] * 2) -
                                  1];
          currentNestedPartsList_data[((int)currentNestedParts_pointer +
                                       currentNestedPartsList->size[0]) -
                                      1] = surplusLength;
          tightness =
              best_nest_list_data[(overlapped + best_nest_list->size[0] * 3) -
                                  1];
          currentNestedPartsList_data[((int)currentNestedParts_pointer +
                                       currentNestedPartsList->size[0] * 2) -
                                      1] = tightness;
          surplusWidth =
              best_nest_list_data[(overlapped + best_nest_list->size[0] * 4) -
                                  1];
          currentNestedPartsList_data[((int)currentNestedParts_pointer +
                                       currentNestedPartsList->size[0] * 3) -
                                      1] = surplusWidth;
          currentNestedPartsList_data[((int)currentNestedParts_pointer +
                                       currentNestedPartsList->size[0] * 4) -
                                      1] = 1.0;
          currentNestedPartsList_data[((int)currentNestedParts_pointer +
                                       currentNestedPartsList->size[0] * 5) -
                                      1] = 1.0;
          currentNestedPartsList_data[((int)currentNestedParts_pointer +
                                       currentNestedPartsList->size[0] * 6) -
                                      1] = 0.0;
          currentNestedPartsList_data[((int)currentNestedParts_pointer +
                                       currentNestedPartsList->size[0] * 7) -
                                      1] = 0.0;
          currentNestedPartsList_data[((int)currentNestedParts_pointer +
                                       currentNestedPartsList->size[0] * 8) -
                                      1] = overlapped;
          partIsNested_data[overlapped - 1] = 1.0;
          S_efficient +=
              ((tightness - surplusLength_tmp) - interval_distance_parts) *
              ((surplusLength - surplusWidth) - interval_distance_parts);
          currentVertexList_data
              [((int)best_nest_list_data
                    [(overlapped + best_nest_list->size[0] * 5) - 1] +
                currentVertexList->size[0] * 4) -
               1] = 0.0;
          /*  先增加左和下各两个顶点 */
          currentVertex_pointer += 2U;
          surplusLength_tmp =
              currentNestedPartsList_data[(int)currentNestedParts_pointer - 1];
          currentVertexList_data[(int)currentVertex_pointer - 2] =
              surplusLength_tmp;
          surplusLength =
              currentNestedPartsList_data[((int)currentNestedParts_pointer +
                                           currentNestedPartsList->size[0]) -
                                          1];
          currentVertexList_data[((int)currentVertex_pointer +
                                  currentVertexList->size[0]) -
                                 2] = surplusLength;
          currentVertexList_data[((int)currentVertex_pointer +
                                  currentVertexList->size[0] * 2) -
                                 2] = currentNestedParts_pointer;
          currentVertexList_data[((int)currentVertex_pointer +
                                  currentVertexList->size[0] * 3) -
                                 2] = 1.0;
          currentVertexList_data[((int)currentVertex_pointer +
                                  currentVertexList->size[0] * 4) -
                                 2] = 1.0;
          tightness =
              currentNestedPartsList_data[((int)currentNestedParts_pointer +
                                           currentNestedPartsList->size[0] *
                                               2) -
                                          1];
          currentVertexList_data[(int)currentVertex_pointer - 1] = tightness;
          surplusWidth =
              currentNestedPartsList_data[((int)currentNestedParts_pointer +
                                           currentNestedPartsList->size[0] *
                                               3) -
                                          1];
          currentVertexList_data[((int)currentVertex_pointer +
                                  currentVertexList->size[0]) -
                                 1] = surplusWidth;
          currentVertexList_data[((int)currentVertex_pointer +
                                  currentVertexList->size[0] * 2) -
                                 1] = currentNestedParts_pointer;
          currentVertexList_data[((int)currentVertex_pointer +
                                  currentVertexList->size[0] * 3) -
                                 1] = 2.0;
          currentVertexList_data[((int)currentVertex_pointer +
                                  currentVertexList->size[0] * 4) -
                                 1] = 1.0;
          /*  检查是否需要增加投影点 */
          overlapped = 1;
          /*  左投 */
          if (surplusLength_tmp == 0.0) {
            overlapped = 0;
          } else {
            j = 0;
            exitg3 = false;
            while ((!exitg3) && (j <= (int)currentNestedParts_pointer - 2)) {
              /* 条件1 右边线条在一直线 */
              if ((fabs(currentNestedPartsList_data
                            [j + currentNestedPartsList->size[0] * 2] -
                        surplusLength_tmp) < 0.0001) &&
                  (currentNestedPartsList_data[j + currentNestedPartsList
                                                       ->size[0]] +
                       0.0001 >
                   surplusLength) &&
                  (currentNestedPartsList_data
                           [j + currentNestedPartsList->size[0] * 3] -
                       0.0001 <
                   surplusLength)) {
                /* 条件2 上下包含 */
                overlapped = 0;
                exitg3 = true;
              } else {
                j++;
              }
            }
          }
          if (overlapped == 1) {
            currentNestedPartsList_data[((int)currentNestedParts_pointer +
                                         currentNestedPartsList->size[0] * 6) -
                                        1] = 1.0;
            currentVertex_pointer++;
            /* 投影源坐标 */
            /* 投影坐标 */
            b_currentNestedPartsList[0] = surplusLength_tmp;
            b_currentNestedPartsList[1] = surplusLength;
            projectionLocate(b_currentNestedPartsList, 1.0,
                             currentNestedPartsList, currentNestedParts_pointer,
                             &surplusLength_tmp, &surplusLength);
            currentVertexList_data[(int)currentVertex_pointer - 1] =
                surplusLength_tmp;
            currentVertexList_data[((int)currentVertex_pointer +
                                    currentVertexList->size[0]) -
                                   1] = surplusLength;
            currentVertexList_data[((int)currentVertex_pointer +
                                    currentVertexList->size[0] * 2) -
                                   1] = currentNestedParts_pointer;
            currentVertexList_data[((int)currentVertex_pointer +
                                    currentVertexList->size[0] * 3) -
                                   1] = 3.0;
            currentVertexList_data[((int)currentVertex_pointer +
                                    currentVertexList->size[0] * 4) -
                                   1] = 1.0;
          }
          overlapped = 1;
          /*  下投 */
          if (surplusWidth == 0.0) {
            overlapped = 0;
          } else {
            j = 0;
            exitg3 = false;
            while ((!exitg3) && (j <= (int)currentNestedParts_pointer - 2)) {
              /* 条件1 上线条在一直线 */
              if ((fabs(currentNestedPartsList_data[j + currentNestedPartsList
                                                            ->size[0]] -
                        surplusWidth) < 0.0001) &&
                  (currentNestedPartsList_data
                           [j + currentNestedPartsList->size[0] * 2] +
                       0.0001 >
                   tightness) &&
                  (currentNestedPartsList_data[j] - 0.0001 < tightness)) {
                /* 条件2 左右包含 */
                overlapped = 0;
                exitg3 = true;
              } else {
                j++;
              }
            }
          }
          if (overlapped == 1) {
            currentNestedPartsList_data[((int)currentNestedParts_pointer +
                                         currentNestedPartsList->size[0] * 7) -
                                        1] = 1.0;
            currentVertex_pointer++;
            /* 投影源坐标 */
            /* 投影坐标 */
            b_currentNestedPartsList[0] = tightness;
            b_currentNestedPartsList[1] = surplusWidth;
            projectionLocate(b_currentNestedPartsList, 2.0,
                             currentNestedPartsList, currentNestedParts_pointer,
                             &surplusLength_tmp, &surplusLength);
            currentVertexList_data[(int)currentVertex_pointer - 1] =
                surplusLength_tmp;
            currentVertexList_data[((int)currentVertex_pointer +
                                    currentVertexList->size[0]) -
                                   1] = surplusLength;
            currentVertexList_data[((int)currentVertex_pointer +
                                    currentVertexList->size[0] * 2) -
                                   1] = currentNestedParts_pointer;
            currentVertexList_data[((int)currentVertex_pointer +
                                    currentVertexList->size[0] * 3) -
                                   1] = 4.0;
            currentVertexList_data[((int)currentVertex_pointer +
                                    currentVertexList->size[0] * 4) -
                                   1] = 1.0;
          }
        }
        /*         %% Step5.
         * 检查是否需要取消一些间隙过小的点、是否需要更新投影点 */
        j = 0;
        exitg2 = false;
        while ((!exitg2) && (j <= (int)currentVertex_pointer - 1)) {
          if (currentVertexList_data[j + currentVertexList->size[0] * 4] ==
              0.0) {
            j++;
          } else {
            /* 取消间隙过小的点 */
            b_currentNestedPartsList[0] = currentVertexList_data[j];
            b_currentNestedPartsList[1] =
                currentVertexList_data[j + currentVertexList->size[0]];
            if (gapIsTooNarrow(b_currentNestedPartsList, currentNestedPartsList,
                               currentNestedParts_pointer, lengthLimit,
                               lengthLimit, plateLength, plateWidth) != 0.0) {
              currentVertexList_data[j + currentVertexList->size[0] * 4] = 0.0;
              j++;

              /* 跳过本体 */
            } else if (currentVertexList_data[j +
                                              currentVertexList->size[0] * 2] ==
                       currentNestedParts_pointer) {
              exitg2 = true;
            } else {
              /* 更新投影点 */
              if (currentVertexList_data[j + currentVertexList->size[0] * 3] ==
                  3.0) {
                /* 更新左投 */
                /* 取消重复 */
                d = currentNestedPartsList_data
                    [((int)currentNestedParts_pointer +
                      currentNestedPartsList->size[0]) -
                     1];
                if ((fabs(currentVertexList_data[j +
                                                 currentVertexList->size[0]] -
                          d) < 0.0001) &&
                    (currentNestedPartsList_data
                         [((int)currentNestedParts_pointer +
                           currentNestedPartsList->size[0] * 6) -
                          1] == 1.0)) {
                  currentVertexList_data[j + currentVertexList->size[0] * 4] =
                      0.0;
                } else {
                  /* 更新投影左边界 */
                  /* 条件1：新矩形在附属矩形左边，且右边界比投影点更右 */
                  tightness = currentNestedPartsList_data
                      [((int)currentNestedParts_pointer +
                        currentNestedPartsList->size[0] * 2) -
                       1];
                  if ((currentNestedPartsList_data
                               [(int)currentVertexList_data
                                    [j + currentVertexList->size[0] * 2] -
                                1] -
                           tightness >
                       0.0001) &&
                      (currentVertexList_data[j] < tightness) &&
                      (d - 0.0001 >
                       currentVertexList_data[j +
                                              currentVertexList->size[0]]) &&
                      (currentNestedPartsList_data
                               [((int)currentNestedParts_pointer +
                                 currentNestedPartsList->size[0] * 3) -
                                1] -
                           0.0001 <
                       currentVertexList_data[j +
                                              currentVertexList->size[0]])) {
                    /* 条件2：上下包含 */
                    currentVertexList_data[j] = tightness;
                  }
                }
              } else if (currentVertexList_data[j + currentVertexList->size[0] *
                                                        3] == 4.0) {
                /* 更新下投 */
                /* 取消重复 */
                d = currentNestedPartsList_data
                    [((int)currentNestedParts_pointer +
                      currentNestedPartsList->size[0] * 2) -
                     1];
                if ((fabs(currentVertexList_data[j] - d) < 0.0001) &&
                    (currentNestedPartsList_data
                         [((int)currentNestedParts_pointer +
                           currentNestedPartsList->size[0] * 7) -
                          1] == 1.0)) {
                  currentVertexList_data[j + currentVertexList->size[0] * 4] =
                      0.0;
                } else {
                  /* 更新投影下边界 */
                  /* 条件1：新矩形在附属矩形下边，且上边界比投影点更上 */
                  tightness = currentNestedPartsList_data
                      [((int)currentNestedParts_pointer +
                        currentNestedPartsList->size[0]) -
                       1];
                  if ((currentNestedPartsList_data
                               [((int)currentVertexList_data
                                     [j + currentVertexList->size[0] * 2] +
                                 currentNestedPartsList->size[0] * 3) -
                                1] -
                           tightness >
                       0.0001) &&
                      (currentVertexList_data[j + currentVertexList->size[0]] <
                       tightness) &&
                      (d - 0.0001 > currentVertexList_data[j]) &&
                      (currentNestedPartsList_data
                               [(int)currentNestedParts_pointer - 1] -
                           0.0001 <
                       currentVertexList_data[j])) {
                    /* 条件2：左右包含 */
                    currentVertexList_data[j + currentVertexList->size[0]] =
                        tightness;
                  }
                }
              }
              j++;
            }
          }
        }
      }
      if ((int)currentVertex_pointer == 1) {
        (*num_plate)--;
        exitg1 = 1;
      } else {
        /*     %% Step6：记录一张原料的布局 */
        surplusLength_tmp = plateLength + interval_distance_parts;
        surplusLength = surplusLength_tmp;
        tightness = plateWidth + interval_distance_parts;
        surplusWidth = tightness;
        i = (int)currentNestedParts_pointer;
        for (b_i = 0; b_i < i; b_i++) {
          d = plateLength -
              currentNestedPartsList_data[b_i +
                                          currentNestedPartsList->size[0] * 2];
          if (d < surplusLength) {
            surplusLength = d;
          }
          d = plateWidth -
              currentNestedPartsList_data[b_i +
                                          currentNestedPartsList->size[0]];
          if (d < surplusWidth) {
            surplusWidth = d;
          }
        }
        sheetDetails_data[(int)*num_plate - 1] = *num_plate;
        sheetDetails_data[((int)*num_plate + sheetDetails->size[0]) - 1] =
            surplusLength + interval_distance_parts;
        sheetDetails_data[((int)*num_plate + sheetDetails->size[0] * 2) - 1] =
            surplusWidth + interval_distance_parts;
        sheetDetails_data[((int)*num_plate + sheetDetails->size[0] * 3) - 1] =
            S_efficient / tightness / surplusLength_tmp;
        d = layout_pointer + currentNestedParts_pointer;
        i = (int)(d + (1.0 - (layout_pointer + 1.0)));
        for (b_i = 0; b_i < i; b_i++) {
          tightness = (layout_pointer + 1.0) + (double)b_i;
          layouts_data[(int)tightness - 1] = *num_plate;
          /* sheet编号 */
          overlapped = (int)(tightness - layout_pointer) - 1;
          layouts_data[((int)tightness + layouts->size[0]) - 1] =
              currentNestedPartsList_data[overlapped +
                                          currentNestedPartsList->size[0] * 8];
          /* part编号 */
          surplusLength_tmp = currentNestedPartsList_data[overlapped];
          layouts_data[((int)tightness + layouts->size[0] * 2) - 1] =
              (surplusLength_tmp + interval_distance_parts) +
              left_bottom_suppplement;
          /* 左下顶点坐标x */
          surplusLength =
              currentNestedPartsList_data[overlapped +
                                          currentNestedPartsList->size[0] * 3];
          layouts_data[((int)tightness + layouts->size[0] * 3) - 1] =
              (surplusLength + interval_distance_parts) +
              left_bottom_suppplement;
          /* y */
          layouts_data[((int)tightness + layouts->size[0] * 4) - 1] =
              (currentNestedPartsList_data[overlapped +
                                           currentNestedPartsList->size[0] *
                                               2] -
               surplusLength_tmp) -
              interval_distance_parts;
          /* x方向长度 */
          layouts_data[((int)tightness + layouts->size[0] * 5) - 1] =
              (currentNestedPartsList_data[overlapped +
                                           currentNestedPartsList->size[0]] -
               surplusLength) -
              interval_distance_parts;
          /* y方向长度 */
        }
        layout_pointer = d;
      }
    } else {
      exitg1 = 1;
    }
  } while (exitg1 == 0);
  emxFree_real_T(&b_best_nest_list);
  emxFree_real_T(&best_nest_list);
  emxFree_real_T(&currentVertexList);
  emxFree_real_T(&currentNestedPartsList);
  *utilization = 0.0;
  i = partsSize->size[0];
  for (b_i = 0; b_i < i; b_i++) {
    *utilization +=
        (partsSize_data[b_i] - interval_distance_parts) *
        (partsSize_data[b_i + partsSize->size[0]] - interval_distance_parts);
  }
  *utilization /= *num_plate * plateLength_Width[0] * plateLength_Width[1];
}

/*
 * File trailer for nest_and_calc_Ver5.c
 *
 * [EOF]
 */
