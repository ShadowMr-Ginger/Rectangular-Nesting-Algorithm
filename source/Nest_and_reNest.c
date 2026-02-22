/*
 * File: Nest_and_reNest.c
 *
 * MATLAB Coder version            : 5.4
 * C/C++ source code generated on  : 01-Sep-2023 10:52:31
 */

/* Include Files */
#include "Nest_and_reNest.h"
#include "Nest_and_reNest_data.h"
#include "Nest_and_reNest_emxutil.h"
#include "Nest_and_reNest_initialize.h"
#include "Nest_and_reNest_types.h"
#include "nest_and_calc_Ver5.h"
#include "rt_nonfinite.h"
#include "tic.h"
#include "toc.h"
#include "rt_nonfinite.h"

/* Function Definitions */
/*
 * Arguments    : const emxArray_real_T *partsSize
 *                const double plateLength_Width[2]
 *                double tightnessCoef
 *                double interval_distance_parts
 *                double interval_distance_plate
 *                double S_coef
 *                double reNestStepLength
 *                emxArray_real_T *layouts
 *                emxArray_real_T *sheetDetails
 *                double *num_plate
 *                double *lastPlateSurplusLength
 *                double *utilization
 *                emxArray_real_T *partIsNested
 * Return Type  : void
 */
void Nest_and_reNest(const emxArray_real_T *partsSize,
                     const double plateLength_Width[2], double tightnessCoef,
                     double interval_distance_parts,
                     double interval_distance_plate, double S_coef,
                     double reNestStepLength, emxArray_real_T *layouts,
                     emxArray_real_T *sheetDetails, double *num_plate,
                     double *lastPlateSurplusLength, double *utilization,
                     emxArray_real_T *partIsNested)
{
  emxArray_real_T *b_reNestparts_Size;
  emxArray_real_T *layouts_last;
  emxArray_real_T *reNestparts_Ndx;
  emxArray_real_T *reNestparts_Size;
  emxArray_real_T *sheetDetails_last;
  double plateLength_Width_last[2];
  const double *partsSize_data;
  double bsum;
  double max_partLen;
  double max_partWid;
  double pointer;
  double pointer_end;
  double widPart;
  double y;
  double *layouts_data;
  double *partIsNested_data;
  double *reNestparts_Ndx_data;
  double *reNestparts_Size_data;
  double *sheetDetails_data;
  int firstBlockLength;
  int hi;
  int ib;
  int k;
  unsigned int pointer_reNest;
  if (!isInitialized_Nest_and_reNest) {
    Nest_and_reNest_initialize();
  }
  partsSize_data = partsSize->data;
  emxInit_real_T(&reNestparts_Size, 2);
  tic();
  hi = reNestparts_Size->size[0] * reNestparts_Size->size[1];
  reNestparts_Size->size[0] = partsSize->size[0];
  reNestparts_Size->size[1] = 2;
  emxEnsureCapacity_real_T(reNestparts_Size, hi);
  reNestparts_Size_data = reNestparts_Size->data;
  firstBlockLength = partsSize->size[0] * partsSize->size[1] - 1;
  for (hi = 0; hi <= firstBlockLength; hi++) {
    reNestparts_Size_data[hi] = partsSize_data[hi];
  }
  nest_and_calc_Ver5(reNestparts_Size, plateLength_Width, tightnessCoef,
                     interval_distance_parts, interval_distance_plate, S_coef,
                     layouts, sheetDetails, num_plate, utilization,
                     partIsNested);
  partIsNested_data = partIsNested->data;
  sheetDetails_data = sheetDetails->data;
  layouts_data = layouts->data;
  /*  末板重套料 */
  /* 索出最后一张板的全部排料 */
  if (partIsNested->size[0] == 0) {
    y = 0.0;
  } else {
    int lastBlockLength;
    int nblocks;
    if (partIsNested->size[0] <= 1024) {
      firstBlockLength = partIsNested->size[0];
      lastBlockLength = 0;
      nblocks = 1;
    } else {
      firstBlockLength = 1024;
      nblocks = partIsNested->size[0] / 1024;
      lastBlockLength = partIsNested->size[0] - (nblocks << 10);
      if (lastBlockLength > 0) {
        nblocks++;
      } else {
        lastBlockLength = 1024;
      }
    }
    y = partIsNested_data[0];
    for (k = 2; k <= firstBlockLength; k++) {
      y += partIsNested_data[k - 1];
    }
    for (ib = 2; ib <= nblocks; ib++) {
      firstBlockLength = (ib - 1) << 10;
      bsum = partIsNested_data[firstBlockLength];
      if (ib == nblocks) {
        hi = lastBlockLength;
      } else {
        hi = 1024;
      }
      for (k = 2; k <= hi; k++) {
        bsum += partIsNested_data[(firstBlockLength + k) - 1];
      }
      y += bsum;
    }
  }
  pointer_end = (double)layouts->size[0] - ((double)layouts->size[0] - y);
  for (pointer = pointer_end; layouts_data[(int)pointer - 1] == *num_plate;
       pointer--) {
  }
  hi = (int)(pointer_end - pointer);
  firstBlockLength = reNestparts_Size->size[0] * reNestparts_Size->size[1];
  reNestparts_Size->size[0] = hi;
  reNestparts_Size->size[1] = 2;
  emxEnsureCapacity_real_T(reNestparts_Size, firstBlockLength);
  reNestparts_Size_data = reNestparts_Size->data;
  firstBlockLength = hi << 1;
  for (hi = 0; hi < firstBlockLength; hi++) {
    reNestparts_Size_data[hi] = 0.0;
  }
  pointer = pointer_end;
  pointer_reNest = 0U;
  while (layouts_data[(int)pointer - 1] == *num_plate) {
    pointer_reNest++;
    reNestparts_Size_data[(int)pointer_reNest - 1] =
        layouts_data[((int)pointer + layouts->size[0] * 4) - 1];
    reNestparts_Size_data[((int)pointer_reNest + reNestparts_Size->size[0]) -
                          1] =
        layouts_data[((int)pointer + layouts->size[0] * 5) - 1];
    pointer--;
  }
  /* 开始压缩最后一张板的大小 */
  plateLength_Width_last[0] = plateLength_Width[0];
  plateLength_Width_last[1] = plateLength_Width[1];
  *lastPlateSurplusLength = 0.0;
  max_partLen = rtMinusInf;
  max_partWid = rtMinusInf;
  hi = (int)pointer_reNest;
  for (firstBlockLength = 0; firstBlockLength < hi; firstBlockLength++) {
    widPart = reNestparts_Size_data[firstBlockLength];
    y = reNestparts_Size_data[firstBlockLength + reNestparts_Size->size[0]];
    if ((widPart < y) || (rtIsNaN(widPart) && (!rtIsNaN(y)))) {
      bsum = y;
    } else {
      bsum = widPart;
    }
    if ((widPart > y) || (rtIsNaN(widPart) && (!rtIsNaN(y)))) {
      widPart = y;
    }
    if (max_partLen < bsum) {
      max_partLen = bsum;
    }
    if (max_partWid < widPart) {
      max_partWid = widPart;
    }
  }
  emxInit_real_T(&reNestparts_Ndx, 1);
  emxInit_real_T(&layouts_last, 2);
  emxInit_real_T(&sheetDetails_last, 2);
  emxInit_real_T(&b_reNestparts_Size, 2);
  double plateLength_Width_last_tmp_tmp;
  int exitg1;
  do {
    exitg1 = 0;
    plateLength_Width_last_tmp_tmp =
        plateLength_Width_last[0] - reNestStepLength;
    plateLength_Width_last[0] = plateLength_Width_last_tmp_tmp;
    *lastPlateSurplusLength += reNestStepLength;
    hi = b_reNestparts_Size->size[0] * b_reNestparts_Size->size[1];
    b_reNestparts_Size->size[0] = reNestparts_Size->size[0];
    b_reNestparts_Size->size[1] = 2;
    emxEnsureCapacity_real_T(b_reNestparts_Size, hi);
    partIsNested_data = b_reNestparts_Size->data;
    firstBlockLength =
        reNestparts_Size->size[0] * reNestparts_Size->size[1] - 1;
    for (hi = 0; hi <= firstBlockLength; hi++) {
      partIsNested_data[hi] = reNestparts_Size_data[hi];
    }
    nest_and_calc_Ver5(b_reNestparts_Size, plateLength_Width_last,
                       tightnessCoef, interval_distance_parts,
                       interval_distance_plate, S_coef, layouts_last,
                       sheetDetails_last, &y, &bsum, reNestparts_Ndx);
    if (y == 2.0) {
      plateLength_Width_last[0] =
          plateLength_Width_last_tmp_tmp + reNestStepLength;
      *lastPlateSurplusLength -= reNestStepLength;
      reNestStepLength /= 2.0;
      if (reNestStepLength < 0.5) {
        exitg1 = 1;
      }
    } else {
      bool guard1 = false;
      y = plateLength_Width_last_tmp_tmp - reNestStepLength;
      if ((y < plateLength_Width_last[1]) ||
          (rtIsNaN(y) && (!rtIsNaN(plateLength_Width_last[1])))) {
        bsum = plateLength_Width_last[1];
      } else {
        bsum = y;
      }
      if ((interval_distance_plate < interval_distance_parts) ||
          (rtIsNaN(interval_distance_plate) &&
           (!rtIsNaN(interval_distance_parts)))) {
        widPart = interval_distance_parts;
      } else {
        widPart = interval_distance_plate;
      }
      guard1 = false;
      if (bsum - widPart * 2.0 < max_partLen) {
        guard1 = true;
      } else {
        if ((y > plateLength_Width_last[1]) ||
            (rtIsNaN(y) && (!rtIsNaN(plateLength_Width_last[1])))) {
          y = plateLength_Width_last[1];
        }
        if ((interval_distance_plate < interval_distance_parts) ||
            (rtIsNaN(interval_distance_plate) &&
             (!rtIsNaN(interval_distance_parts)))) {
          bsum = interval_distance_parts;
        } else {
          bsum = interval_distance_plate;
        }
        if (y - bsum * 2.0 < max_partWid) {
          guard1 = true;
        } else if (plateLength_Width_last_tmp_tmp - reNestStepLength < 0.0) {
          exitg1 = 1;
        }
      }
      if (guard1) {
        plateLength_Width_last[0] =
            plateLength_Width_last_tmp_tmp + reNestStepLength;
        *lastPlateSurplusLength -= reNestStepLength;
        reNestStepLength /= 2.0;
        if (reNestStepLength < 0.5) {
          exitg1 = 1;
        }
      }
    }
  } while (exitg1 == 0);
  emxFree_real_T(&b_reNestparts_Size);
  nest_and_calc_Ver5(reNestparts_Size, plateLength_Width_last, tightnessCoef,
                     interval_distance_parts, interval_distance_plate, S_coef,
                     layouts_last, sheetDetails_last, &y, &bsum,
                     reNestparts_Ndx);
  partIsNested_data = sheetDetails_last->data;
  reNestparts_Size_data = layouts_last->data;
  firstBlockLength = layouts_last->size[0] - 1;
  hi = reNestparts_Ndx->size[0];
  reNestparts_Ndx->size[0] = layouts_last->size[0];
  emxEnsureCapacity_real_T(reNestparts_Ndx, hi);
  reNestparts_Ndx_data = reNestparts_Ndx->data;
  emxFree_real_T(&reNestparts_Size);
  for (hi = 0; hi <= firstBlockLength; hi++) {
    reNestparts_Ndx_data[hi] = (reNestparts_Size_data[hi] + *num_plate) - 1.0;
  }
  firstBlockLength = reNestparts_Ndx->size[0];
  for (hi = 0; hi < firstBlockLength; hi++) {
    reNestparts_Size_data[hi] = reNestparts_Ndx_data[hi];
  }
  emxFree_real_T(&reNestparts_Ndx);
  pointer_reNest = 0U;
  while (pointer < pointer_end) {
    pointer++;
    pointer_reNest++;
    for (hi = 0; hi < 6; hi++) {
      layouts_data[((int)pointer + layouts->size[0] * hi) - 1] =
          reNestparts_Size_data[((int)pointer_reNest +
                                 layouts_last->size[0] * hi) -
                                1];
    }
  }
  emxFree_real_T(&layouts_last);
  partIsNested_data[0] = *num_plate;
  partIsNested_data[sheetDetails_last->size[0]] += *lastPlateSurplusLength;
  partIsNested_data[sheetDetails_last->size[0] * 3] =
      partIsNested_data[sheetDetails_last->size[0] * 3] *
          (plateLength_Width[0] - *lastPlateSurplusLength) /
          plateLength_Width[0] +
      *lastPlateSurplusLength / plateLength_Width[0];
  sheetDetails_data[(int)*num_plate - 1] = partIsNested_data[0];
  sheetDetails_data[((int)*num_plate + sheetDetails->size[0]) - 1] =
      partIsNested_data[sheetDetails_last->size[0]];
  sheetDetails_data[((int)*num_plate + sheetDetails->size[0] * 2) - 1] =
      partIsNested_data[sheetDetails_last->size[0] * 2];
  sheetDetails_data[((int)*num_plate + sheetDetails->size[0] * 3) - 1] =
      partIsNested_data[sheetDetails_last->size[0] * 3];
  toc();
  *utilization =
      (*utilization * plateLength_Width[0] * plateLength_Width[1] * *num_plate +
       *lastPlateSurplusLength * plateLength_Width[1]) /
      (plateLength_Width[0] * plateLength_Width[1] * *num_plate);
  emxFree_real_T(&sheetDetails_last);
}

/*
 * File trailer for Nest_and_reNest.c
 *
 * [EOF]
 */
