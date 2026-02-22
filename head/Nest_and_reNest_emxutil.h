/*
 * File: Nest_and_reNest_emxutil.h
 *
 * MATLAB Coder version            : 5.4
 * C/C++ source code generated on  : 01-Sep-2023 10:52:31
 */

#ifndef NEST_AND_RENEST_EMXUTIL_H
#define NEST_AND_RENEST_EMXUTIL_H

/* Include Files */
#include "Nest_and_reNest_types.h"
#include "rtwtypes.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function Declarations */
extern void emxEnsureCapacity_real_T(emxArray_real_T *emxArray, int oldNumel);

extern void emxFree_real_T(emxArray_real_T **pEmxArray);

extern void emxInit_real_T(emxArray_real_T **pEmxArray, int numDimensions);

#ifdef __cplusplus
}
#endif

#endif
/*
 * File trailer for Nest_and_reNest_emxutil.h
 *
 * [EOF]
 */
