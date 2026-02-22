/*
 * File: nest_and_calc_Ver5.h
 *
 * MATLAB Coder version            : 5.4
 * C/C++ source code generated on  : 01-Sep-2023 10:52:31
 */

#ifndef NEST_AND_CALC_VER5_H
#define NEST_AND_CALC_VER5_H

/* Include Files */
#include "Nest_and_reNest_types.h"
#include "rtwtypes.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
void nest_and_calc_Ver5(emxArray_real_T *partsSize,
                        const double plateLength_Width[2], double tightnessCoef,
                        double interval_distance_parts,
                        double interval_distance_plate, double S_coef,
                        emxArray_real_T *layouts, emxArray_real_T *sheetDetails,
                        double *num_plate, double *utilization,
                        emxArray_real_T *partIsNested);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for nest_and_calc_Ver5.h
 *
 * [EOF]
 */
