/*
 * File: projectionLocate.h
 *
 * MATLAB Coder version            : 5.4
 * C/C++ source code generated on  : 01-Sep-2023 10:52:31
 */

#ifndef PROJECTIONLOCATE_H
#define PROJECTIONLOCATE_H

/* Include Files */
#include "Nest_and_reNest_types.h"
#include "rtwtypes.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
void projectionLocate(const double originPoint[2], double projectionMode,
                      const emxArray_real_T *currentNestedPartsList,
                      double currentNestedParts_pointer, double *x, double *y);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for projectionLocate.h
 *
 * [EOF]
 */
