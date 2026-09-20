// 复现测试：加载 parts.csv，调用 nesting_run，输出末板关键指标并导出末板 SVG。
// 用于对比“末板 skyline 再压缩”改进前后的效果。
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
using namespace std;

typedef int (__cdecl *run_fn)(
    const double*, const double*, int,
    double, double, double, double, double, double, double,
    double*, double*, double*,
    double*, double*, double*);

static bool loadParts(const char* path, vector<double>& lens, vector<double>& wids) {
    ifstream f(path);
    if (!f) { fprintf(stderr, "cannot open %s\n", path); return false; }
    string line;
    bool first = true;
    while (getline(f, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        // 去 BOM
        if (first && line.size() >= 3 && (unsigned char)line[0] == 0xEF) line = line.substr(3);
        first = false;
        stringstream ss(line);
        string c0, c1, c2;
        getline(ss, c0, ','); getline(ss, c1, ','); getline(ss, c2, ',');
        double a = atof(c1.c_str()), b = atof(c2.c_str());
        if (a > 0 && b > 0) { lens.push_back(a); wids.push_back(b); }
    }
    return !lens.empty();
}

int main(int argc, char** argv) {
    const char* csv = argc > 1 ? argv[1] : "../../../../parts.csv";
    double plateL = argc > 2 ? atof(argv[2]) : 100.0;
    double plateW = argc > 3 ? atof(argv[3]) : 50.0;
    double edge   = argc > 4 ? atof(argv[4]) : 0.2;

    HMODULE h = LoadLibraryA("nesting.dll");
    if (!h) { fprintf(stderr, "LoadLibrary nesting.dll failed\n"); return 1; }
    run_fn run = (run_fn)GetProcAddress(h, "nesting_run");
    if (!run) { fprintf(stderr, "GetProcAddress nesting_run failed\n"); return 1; }

    vector<double> lens, wids;
    if (!loadParts(csv, lens, wids)) return 1;
    int n = (int)lens.size();
    printf("parts: %d, plate %.1f x %.1f, edge %.2f\n", n, plateL, plateW, edge);

    // 与后端一致的派生参数
    double intervalParts = max(0.01, floor(min(plateL, plateW) * 0.002 * 1000.0 + 0.5) / 1000.0);
    double tightness = 1e300;
    for (int i = 0; i < n; i++) tightness = min(tightness, min(lens[i], wids[i]));
    double sCoef = 0.1;
    double reNestStep = max(0.5, plateL / 61.0);
    printf("params: tightness=%.3f intervalParts=%.3f intervalPlate=%.3f S=%.2f reNestStep=%.3f\n",
           tightness, intervalParts, edge, sCoef, reNestStep);

    vector<double> layouts(n * 6), sheets(n * 4), nested(n);
    double numPlates, lastSurplus, util;
    auto t0 = chrono::high_resolution_clock::now();
    int rc = run(lens.data(), wids.data(), n, plateL, plateW, tightness,
                 intervalParts, edge, sCoef, reNestStep,
                 layouts.data(), sheets.data(), nested.data(),
                 &numPlates, &lastSurplus, &util);
    auto t1 = chrono::high_resolution_clock::now();
    if (rc != 0) { fprintf(stderr, "nesting_run rc=%d\n", rc); return 1; }
    double ms = chrono::duration<double, milli>(t1 - t0).count();

    int np = (int)llround(numPlates);
    // 末板：col0 == numPlates 的行（row-major：行 j 从 j*6 开始）
    double rightmost = 0; int lastCount = 0; double lastArea = 0;
    for (int j = 0; j < n; j++) {
        size_t b = (size_t)j * 6;
        if ((int)llround(layouts[b]) == np) {
            double x = layouts[b + 2], xl = layouts[b + 4];
            double y = layouts[b + 3], yl = layouts[b + 5];
            rightmost = max(rightmost, x + xl);
            lastCount++;
            lastArea += xl * yl;
        }
    }
    double cutSurplus = plateL - rightmost;
    printf("RESULT numPlates=%d util=%.4f (%.2f%%) runtime=%.1fms\n", np, util, util * 100, ms);
    printf("RESULT lastPlate: parts=%d rightmost=%.3f reportedSurplus=%.3f cutSurplus=%.3f area=%.1f\n",
           lastCount, rightmost, lastSurplus, cutSurplus, lastArea);
    // sheetDetails 末板行（row-major：行 j 从 j*4 开始）
    size_t sb = (size_t)(np - 1) * 4;
    printf("RESULT sheetDetails[last]: no=%.0f surplusLen=%.3f surplusWid=%.3f util=%.4f\n",
           sheets[sb], sheets[sb + 1], sheets[sb + 2], sheets[sb + 3]);

    // 末板 SVG
    ofstream svg("last_plate.svg");
    double sc = 800.0 / plateL;
    svg << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"800\" height=\"" << (int)(plateW * sc + 40)
        << "\" viewBox=\"0 0 " << plateL << " " << plateW << "\">\n";
    svg << "<rect x=\"0\" y=\"0\" width=\"" << plateL << "\" height=\"" << plateW << "\" fill=\"white\" stroke=\"black\"/>\n";
    for (int j = 0; j < n; j++) {
        size_t b = (size_t)j * 6;
        if ((int)llround(layouts[b]) != np) continue;
        double x = layouts[b + 2], xl = layouts[b + 4];
        double y = plateW - layouts[b + 3] - layouts[b + 5], yl = layouts[b + 5];
        svg << "<rect x=\"" << x << "\" y=\"" << y << "\" width=\"" << xl << "\" height=\"" << yl
            << "\" fill=\"#dbeafe\" stroke=\"#2563eb\" stroke-width=\"0.2\"/>\n";
    }
    svg << "<line x1=\"" << rightmost << "\" y1=\"0\" x2=\"" << rightmost << "\" y2=\"" << plateW << "\" stroke=\"red\" stroke-width=\"0.3\"/>\n";
    svg << "</svg>\n";
    printf("last plate svg written: last_plate.svg\n");

    // 导出末板零件矩形，供占用分析
    ofstream txt("last_plate_rects.txt");
    for (int j = 0; j < n; j++) {
        size_t b = (size_t)j * 6;
        if ((int)llround(layouts[b]) != np) continue;
        txt << layouts[b + 1] << " " << layouts[b + 2] << " " << layouts[b + 3] << " "
            << layouts[b + 4] << " " << layouts[b + 5] << "\n";
    }
    return 0;
}
