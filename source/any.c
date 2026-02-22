/*
 * File: any.c
 *
 * MATLAB Coder version            : 5.4
 * C/C++ source code generated on  : 01-Sep-2023 10:52:31
 */

/* Include Files */
#include "any.h"
#include "Nest_and_reNest_types.h"
#include "rt_nonfinite.h"
#include "rt_nonfinite.h"

/* Function Definitions */
/*
 * Arguments    : const emxArray_real_T *x
 * Return Type  : bool
 */
bool any(const emxArray_real_T *x)
{
  const double *x_data;
  int ix;
  bool exitg1;
  bool y;
  x_data = x->data;
  y = false;
  ix = 0;
  exitg1 = false;
  while ((!exitg1) && (ix + 1 <= x->size[0])) {
    if ((x_data[ix] == 0.0) || rtIsNaN(x_data[ix])) {
      ix++;
    } else {
      y = true;
      exitg1 = true;
    }
  }
  return y;
}

/*
 * File trailer for any.c
 *
 * [EOF]
 */
