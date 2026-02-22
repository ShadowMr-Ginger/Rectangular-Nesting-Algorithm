/*
 * File: tightnessCalc.h
 *
 * MATLAB Coder version            : 5.4
 * C/C++ source code generated on  : 01-Sep-2023 10:52:31
 */

#ifndef TIGHTNESSCALC_H
#define TIGHTNESSCALC_H

/* Include Files */
#include "Nest_and_reNest_types.h"
#include "rtwtypes.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
double tightnessCalc(const double rectangleCurrent[4],
                     const emxArray_real_T *currentNestedPartsList,
                     double currentNestedParts_pointer, double tightnessCoef,
                     double plateLength, double plateWidth, double S_coef);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for tightnessCalc.h
 *
 * [EOF]
 */
