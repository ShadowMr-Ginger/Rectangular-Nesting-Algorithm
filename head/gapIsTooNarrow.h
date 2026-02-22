/*
 * File: gapIsTooNarrow.h
 *
 * MATLAB Coder version            : 5.4
 * C/C++ source code generated on  : 01-Sep-2023 10:52:31
 */

#ifndef GAPISTOONARROW_H
#define GAPISTOONARROW_H

/* Include Files */
#include "Nest_and_reNest_types.h"
#include "rtwtypes.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
double gapIsTooNarrow(const double vertex[2],
                      const emxArray_real_T *currentNestedPartsList,
                      double currentNestedParts_pointer, double lengthLimit,
                      double widthLimit, double plateLength, double platewidth);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for gapIsTooNarrow.h
 *
 * [EOF]
 */
