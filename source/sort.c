/*
 * File: sort.c
 *
 * MATLAB Coder version            : 5.4
 * C/C++ source code generated on  : 01-Sep-2023 10:52:31
 */

/* Include Files */
#include "sort.h"
#include "rt_nonfinite.h"
#include "rt_nonfinite.h"

/* Function Definitions */
/*
 * Arguments    : double x[4]
 * Return Type  : void
 */
void sort(double x[4])
{
  double x4[4];
  double xwork[4];
  int i2;
  int i3;
  int ib;
  int nNaNs;
  signed char perm[4];
  x4[0] = 0.0;
  xwork[0] = 0.0;
  x4[1] = 0.0;
  xwork[1] = 0.0;
  x4[2] = 0.0;
  xwork[2] = 0.0;
  x4[3] = 0.0;
  xwork[3] = 0.0;
  nNaNs = 0;
  ib = 0;
  if (rtIsNaN(x[0])) {
    xwork[3] = x[0];
    nNaNs = 1;
  } else {
    ib = 1;
    x4[0] = x[0];
  }
  if (rtIsNaN(x[1])) {
    xwork[3 - nNaNs] = x[1];
    nNaNs++;
  } else {
    ib++;
    x4[ib - 1] = x[1];
  }
  if (rtIsNaN(x[2])) {
    xwork[3 - nNaNs] = x[2];
    nNaNs++;
  } else {
    ib++;
    x4[ib - 1] = x[2];
  }
  if (rtIsNaN(x[3])) {
    xwork[3 - nNaNs] = x[3];
    nNaNs++;
  } else {
    ib++;
    x4[ib - 1] = x[3];
    if (ib == 4) {
      double d;
      double d1;
      int i4;
      if (x4[0] <= x4[1]) {
        ib = 1;
        i2 = 2;
      } else {
        ib = 2;
        i2 = 1;
      }
      if (x4[2] <= x4[3]) {
        i3 = 3;
        i4 = 4;
      } else {
        i3 = 4;
        i4 = 3;
      }
      d = x4[ib - 1];
      d1 = x4[i3 - 1];
      if (d <= d1) {
        d = x4[i2 - 1];
        if (d <= d1) {
          perm[0] = (signed char)ib;
          perm[1] = (signed char)i2;
          perm[2] = (signed char)i3;
          perm[3] = (signed char)i4;
        } else if (d <= x4[i4 - 1]) {
          perm[0] = (signed char)ib;
          perm[1] = (signed char)i3;
          perm[2] = (signed char)i2;
          perm[3] = (signed char)i4;
        } else {
          perm[0] = (signed char)ib;
          perm[1] = (signed char)i3;
          perm[2] = (signed char)i4;
          perm[3] = (signed char)i2;
        }
      } else {
        d1 = x4[i4 - 1];
        if (d <= d1) {
          if (x4[i2 - 1] <= d1) {
            perm[0] = (signed char)i3;
            perm[1] = (signed char)ib;
            perm[2] = (signed char)i2;
            perm[3] = (signed char)i4;
          } else {
            perm[0] = (signed char)i3;
            perm[1] = (signed char)ib;
            perm[2] = (signed char)i4;
            perm[3] = (signed char)i2;
          }
        } else {
          perm[0] = (signed char)i3;
          perm[1] = (signed char)i4;
          perm[2] = (signed char)ib;
          perm[3] = (signed char)i2;
        }
      }
      x[-nNaNs] = x4[perm[0] - 1];
      x[1 - nNaNs] = x4[perm[1] - 1];
      x[2 - nNaNs] = x4[perm[2] - 1];
      x[3 - nNaNs] = x4[perm[3] - 1];
      ib = 0;
    }
  }
  if (ib > 0) {
    perm[1] = 0;
    perm[2] = 0;
    perm[3] = 0;
    if (ib == 1) {
      perm[0] = 1;
    } else if (ib == 2) {
      if (x4[0] <= x4[1]) {
        perm[0] = 1;
        perm[1] = 2;
      } else {
        perm[0] = 2;
        perm[1] = 1;
      }
    } else if (x4[0] <= x4[1]) {
      if (x4[1] <= x4[2]) {
        perm[0] = 1;
        perm[1] = 2;
        perm[2] = 3;
      } else if (x4[0] <= x4[2]) {
        perm[0] = 1;
        perm[1] = 3;
        perm[2] = 2;
      } else {
        perm[0] = 3;
        perm[1] = 1;
        perm[2] = 2;
      }
    } else if (x4[0] <= x4[2]) {
      perm[0] = 2;
      perm[1] = 1;
      perm[2] = 3;
    } else if (x4[1] <= x4[2]) {
      perm[0] = 2;
      perm[1] = 3;
      perm[2] = 1;
    } else {
      perm[0] = 3;
      perm[1] = 2;
      perm[2] = 1;
    }
    for (i3 = 0; i3 < ib; i3++) {
      x[((i3 - nNaNs) - ib) + 4] = x4[perm[i3] - 1];
    }
  }
  ib = (nNaNs >> 1) + 4;
  for (i3 = 0; i3 <= ib - 5; i3++) {
    i2 = (i3 - nNaNs) + 4;
    x[i2] = xwork[3 - i3];
    x[3 - i3] = xwork[i2];
  }
  if ((nNaNs & 1) != 0) {
    ib -= nNaNs;
    x[ib] = xwork[ib];
  }
}

/*
 * File trailer for sort.c
 *
 * [EOF]
 */
