/*
 * File: Nest_and_reNest_initialize.c
 *
 * MATLAB Coder version            : 5.4
 * C/C++ source code generated on  : 01-Sep-2023 10:52:31
 */

/* Include Files */
#include "Nest_and_reNest_initialize.h"
#include "CoderTimeAPI.h"
#include "Nest_and_reNest_data.h"
#include "rt_nonfinite.h"
#include "timeKeeper.h"

/* Function Definitions */
/*
 * Arguments    : void
 * Return Type  : void
 */
void Nest_and_reNest_initialize(void)
{
  savedTime_not_empty_init();
  freq_not_empty_init();
  isInitialized_Nest_and_reNest = true;
}

/*
 * File trailer for Nest_and_reNest_initialize.c
 *
 * [EOF]
 */
