/*
 * smoke_test.cpp - smoke test for nesting.dll (temporary verification tool).
 *
 * Build (from Git Bash, x64 Native Tools environment or full path cl):
 *   "/c/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/<ver>/bin/Hostx64/x64/cl.exe" \
 *     /EHsc /MD /I.. smoke_test.cpp /Fe:smoke_test.exe
 *   (link against ..\x64\Release\nesting.lib, or run smoke_test.exe with
 *    nesting.dll next to it)
 *
 * Simpler: build via MSBuild with test.vcxproj in this directory, or just
 * compile and put ..\x64\Release\nesting.dll next to the exe.
 *
 * The test loads nesting.dll at runtime, runs a 10-part nest on a 100x50
 * plate, and checks that all parts are nested, coordinates stay inside the
 * plate, and no two placed parts overlap.
 */

#include <windows.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>

typedef int (__cdecl *nesting_run_fn)(
    const double* partLengths, const double* partWidths, int numParts,
    double plateLength, double plateWidth,
    double tightnessCoef, double intervalParts, double intervalPlate,
    double sCoef, double reNestStep,
    double* layouts, double* sheetDetails, double* partIsNested,
    double* outNumPlates, double* outLastSurplus, double* outUtilization);

int main()
{
  HMODULE dll = LoadLibraryA("nesting.dll");
  if (!dll) {
    dll = LoadLibraryA("..\\x64\\Release\\nesting.dll");
  }
  if (!dll) {
    printf("FAIL: cannot load nesting.dll (error %lu)\n", GetLastError());
    return 1;
  }

  nesting_run_fn nesting_run =
      reinterpret_cast<nesting_run_fn>(GetProcAddress(dll, "nesting_run"));
  if (!nesting_run) {
    printf("FAIL: nesting_run not found in dll\n");
    return 1;
  }

  const int numParts = 10;
  const double plateL = 100.0, plateW = 50.0;

  double partL[numParts], partW[numParts];
  srand(42);
  for (int i = 0; i < numParts; ++i) {
    partL[i] = 1.0 + rand() % 49; /* 1..49 */
    partW[i] = 1.0 + rand() % 49;
    printf("part %2d: %.0f x %.0f\n", i, partL[i], partW[i]);
  }

  double layouts[numParts * 6];
  double sheetDetails[numParts * 4];
  double partIsNested[numParts];
  double numPlates = 0, lastSurplus = 0, utilization = 0;

  int rc = nesting_run(partL, partW, numParts, plateL, plateW,
                       /*tightnessCoef=*/50.0, /*intervalParts=*/2.0,
                       /*intervalPlate=*/2.0, /*sCoef=*/0.1,
                       /*reNestStep=*/40.0,
                       layouts, sheetDetails, partIsNested,
                       &numPlates, &lastSurplus, &utilization);

  if (rc != 0) {
    printf("FAIL: nesting_run returned %d\n", rc);
    return 1;
  }

  printf("\nnumPlates   = %.0f\n", numPlates);
  printf("utilization = %.4f\n", utilization);
  printf("lastSurplus = %.2f\n", lastSurplus);

  int failures = 0;
  int nestedCount = 0;
  for (int j = 0; j < numParts; ++j) {
    double plateNo = layouts[j * 6 + 0];
    double seqNo   = layouts[j * 6 + 1];
    double x       = layouts[j * 6 + 2];
    double y       = layouts[j * 6 + 3];
    double w       = layouts[j * 6 + 4];
    double h       = layouts[j * 6 + 5];
    printf("part %2d: plate=%.0f seq=%.0f (x=%.2f y=%.2f) %.2f x %.2f nested=%.0f\n",
           j, plateNo, seqNo, x, y, w, h, partIsNested[j]);

    if (partIsNested[j] > 0.5) {
      ++nestedCount;
      if (plateNo <= 0) {
        printf("  FAIL: nested but plate number not positive\n");
        ++failures;
      }
      if (x < -1e-6 || y < -1e-6 || x + w > plateL + 1e-6 ||
          y + h > plateW + 1e-6) {
        printf("  FAIL: part %d out of plate bounds\n", j);
        ++failures;
      }
    }
  }

  /* simple pairwise overlap check among parts on the same plate */
  for (int a = 0; a < numParts; ++a) {
    if (partIsNested[a] < 0.5) continue;
    for (int b = a + 1; b < numParts; ++b) {
      if (partIsNested[b] < 0.5) continue;
      if ((int)layouts[a * 6 + 0] != (int)layouts[b * 6 + 0]) continue;
      double ax = layouts[a * 6 + 2], ay = layouts[a * 6 + 3];
      double aw = layouts[a * 6 + 4], ah = layouts[a * 6 + 5];
      double bx = layouts[b * 6 + 2], by = layouts[b * 6 + 3];
      double bw = layouts[b * 6 + 4], bh = layouts[b * 6 + 5];
      double ox = (ax < bx ? ax + aw : bx + bw) - (ax > bx ? ax : bx);
      double oy = (ay < by ? ay + ah : by + bh) - (ay > by ? ay : by);
      if (ox > 1e-6 && oy > 1e-6) {
        printf("FAIL: parts %d and %d overlap by %.2f x %.2f\n",
               a, b, ox, oy);
        ++failures;
      }
    }
  }

  if (nestedCount != numParts) {
    printf("FAIL: only %d of %d parts nested\n", nestedCount, numParts);
    ++failures;
  }

  for (int s = 0; s < (int)numPlates && s < numParts; ++s) {
    printf("sheet %.0f: surplus %.2f x %.2f, utilization %.4f\n",
           sheetDetails[s * 4 + 0], sheetDetails[s * 4 + 1],
           sheetDetails[s * 4 + 2], sheetDetails[s * 4 + 3]);
  }

  FreeLibrary(dll);

  if (failures == 0 && nestedCount == numParts) {
    printf("\nSMOKE TEST PASSED: all %d parts nested, no overlaps, "
           "all within plate.\n", numParts);
    return 0;
  }
  printf("\nSMOKE TEST FAILED (%d failures)\n", failures);
  return 1;
}
