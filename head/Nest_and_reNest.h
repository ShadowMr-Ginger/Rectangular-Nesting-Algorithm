/*
 * File: Nest_and_reNest.h
 *
 * MATLAB Coder version            : 5.4
 * C/C++ source code generated on  : 01-Sep-2023 10:52:31
 */

#ifndef NEST_AND_RENEST_H
#define NEST_AND_RENEST_H

/* Include Files */
#include "Nest_and_reNest_types.h"
#include "rtwtypes.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
extern void Nest_and_reNest(const emxArray_real_T *partsSize,
                            const double plateLength_Width[2],
                            double tightnessCoef,
                            double interval_distance_parts,
                            double interval_distance_plate, double S_coef,
                            double reNestStepLength, emxArray_real_T *layouts,
                            emxArray_real_T *sheetDetails, double *num_plate,
                            double *lastPlateSurplusLength, double *utilization,
                            emxArray_real_T *partIsNested);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for Nest_and_reNest.h
 *
 * [EOF]
 */
