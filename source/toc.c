/*
 * File: toc.c
 *
 * MATLAB Coder version            : 5.4
 * C/C++ source code generated on  : 01-Sep-2023 10:52:31
 */

/* Include Files */
#include "toc.h"
#include "Nest_and_reNest_data.h"
#include "rt_nonfinite.h"
#include "timeKeeper.h"
#include "coder_posix_time.h"
#include <stdio.h>

/* Function Definitions */
/*
 * Arguments    : void
 * Return Type  : void
 */
void toc(void)
{
  coderTimespec b_timespec;
  double tstart_tv_nsec;
  double tstart_tv_sec;
  b_timeKeeper(&tstart_tv_sec, &tstart_tv_nsec);
  if (!freq_not_empty) {
    freq_not_empty = true;
    coderInitTimeFunctions(&freq);
  }
  coderTimeClockGettimeMonotonic(&b_timespec, freq);
  printf("Elapsed time is %f seconds\n",
         (b_timespec.tv_sec - tstart_tv_sec) +
             (b_timespec.tv_nsec - tstart_tv_nsec) / 1.0E+9);
  fflush(stdout);
}

/*
 * File trailer for toc.c
 *
 * [EOF]
 */
