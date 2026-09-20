/*
 * wrapper.cpp - flat C interface over the MATLAB Coder generated
 * Nest_and_reNest algorithm, exported from nesting.dll for P/Invoke.
 */

#include <mutex>
#include <exception>

#include "Nest_and_reNest.h"
#include "Nest_and_reNest_emxAPI.h"
#include "Nest_and_reNest_initialize.h"

namespace {

std::once_flag g_initFlag;
std::mutex g_algoMutex;

} /* namespace */

extern "C" __declspec(dllexport) int __cdecl nesting_run(
    const double* partLengths, const double* partWidths, int numParts,
    double plateLength, double plateWidth,
    double tightnessCoef, double intervalParts, double intervalPlate,
    double sCoef, double reNestStep,
    double* layouts, double* sheetDetails, double* partIsNested,
    double* outNumPlates, double* outLastSurplus, double* outUtilization)
{
  if (partLengths == nullptr || partWidths == nullptr ||
      layouts == nullptr || sheetDetails == nullptr ||
      partIsNested == nullptr || outNumPlates == nullptr ||
      outLastSurplus == nullptr || outUtilization == nullptr ||
      numParts <= 0) {
    return 1;
  }

  std::lock_guard<std::mutex> lock(g_algoMutex);

  try {
    std::call_once(g_initFlag, Nest_and_reNest_initialize);

    emxArray_real_T* partsSize = emxCreate_real_T(numParts, 2);
    for (int i = 0; i < numParts; ++i) {
      partsSize->data[i] = partLengths[i];
      partsSize->data[numParts + i] = partWidths[i];
    }

    double plateLength_Width[2];
    plateLength_Width[0] = plateLength;
    plateLength_Width[1] = plateWidth;

    emxArray_real_T* layoutsArr = nullptr;
    emxArray_real_T* sheetArr = nullptr;
    emxArray_real_T* nestedArr = nullptr;
    emxInitArray_real_T(&layoutsArr, 2);
    emxInitArray_real_T(&sheetArr, 2);
    emxInitArray_real_T(&nestedArr, 1);

    double num_plate = 0.0;
    double lastPlateSurplusLength = 0.0;
    double utilization = 0.0;

    Nest_and_reNest(partsSize, plateLength_Width, tightnessCoef,
                    intervalParts, intervalPlate, sCoef, reNestStep,
                    layoutsArr, sheetArr, &num_plate,
                    &lastPlateSurplusLength, &utilization, nestedArr);

    /* layouts: column-major (layoutsArr->size[0]) x 6 */
    int layoutStride = layoutsArr->size[0];
    int layoutRows = layoutStride < numParts ? layoutStride : numParts;
    for (int j = 0; j < layoutRows; ++j) {
      for (int c = 0; c < 6; ++c) {
        layouts[j * 6 + c] = layoutsArr->data[j + (size_t)c * layoutStride];
      }
    }

    /* sheetDetails: column-major (sheetArr->size[0]) x 4 */
    int sheetStride = sheetArr->size[0];
    int sheetRows = sheetStride < numParts ? sheetStride : numParts;
    for (int j = 0; j < sheetRows; ++j) {
      for (int c = 0; c < 4; ++c) {
        sheetDetails[j * 4 + c] = sheetArr->data[j + (size_t)c * sheetStride];
      }
    }

    for (int i = 0; i < numParts; ++i) {
      partIsNested[i] = nestedArr->data[i];
    }

    *outNumPlates = num_plate;
    *outLastSurplus = lastPlateSurplusLength;
    *outUtilization = utilization;

    emxDestroyArray_real_T(nestedArr);
    emxDestroyArray_real_T(sheetArr);
    emxDestroyArray_real_T(layoutsArr);
    emxDestroyArray_real_T(partsSize);

    return 0;
  } catch (...) {
    return 2;
  }
}
