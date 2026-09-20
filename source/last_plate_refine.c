/*
 * File: last_plate_refine.c
 *
 * 末板 skyline（天际线）紧凑重排实现。纯 C，无 MATLAB 运行时依赖。
 *
 * 坐标模型与 nest_and_calc_Ver5 完全一致：
 *   - 每个零件内部尺寸 = 实际尺寸 + gap（零件间距），相邻零件内部矩形
 *     边缘相贴即等价于实际间距为 gap；
 *   - 调用方负责内部可用区域与布局坐标的换算（布局坐标 = 内部 + 板边距）。
 *
 * 算法：skyline bottom-left 放置（在等高楼面上取最低、最左位置，支持
 * 90° 旋转），对多种排序（最大边降序 / 面积降序 / 宽度降序 / 原始顺序）
 * 各跑一次取最右端最小者。末板零件数通常为几十，耗时微秒级。
 */

#include "last_plate_refine.h"
#include <math.h>
#include <string.h>

#define REFINE_MAXN (2048)
#define EPS (1e-9)

typedef struct {
  double x; /* 区段左端 x（天际线边界） */
  double h; /* 该区段（至下一区段左端）上的高度 */
} Seg;

/* 位置 b 处（及其右侧）的天际线高度：最后一个左端 <= b 的区段高度 */
static double seg_height_at(const Seg *s, int m, double b)
{
  double h = 0.0;
  int i;
  for (i = 0; i < m; i++) {
    if (s[i].x <= b + EPS) {
      h = s[i].h;
    } else {
      break;
    }
  }
  return h;
}

/*
 * 将内部矩形 [x0, x1)（底 = top - 高）放入天际线：
 * [x0, x1) 内高度提升至 top，区段表重建并合并等高边界。
 * 约定：s[0].x 恒为 0（原点边界）。
 */
static int seg_insert(Seg *s, int *m, double x0, double x1, double top)
{
  static double bx[2 * REFINE_MAXN + 8];
  static double bh[2 * REFINE_MAXN + 8];
  int nb = 0;
  int i, nm;
  int cap = 2 * REFINE_MAXN + 8;

  if (*m + 2 >= cap) {
    return -1;
  }
  /* 新区段表 = 全部旧边界 ∪ {x0, x1}（旧边界必须保留！） */
  for (i = 0; i < *m; i++) {
    bx[nb] = s[i].x;
    nb++;
  }
  bx[nb] = x0; nb++;
  bx[nb] = x1; nb++;
  /* 插入排序（区段数很小） */
  for (i = 1; i < nb; i++) {
    double tx = bx[i];
    int j = i - 1;
    while (j >= 0 && bx[j] > tx + EPS) {
      bx[j + 1] = bx[j];
      j--;
    }
    bx[j + 1] = tx;
  }
  /* 各边界处的新高度：[x0,x1) 内为 top，其余保持旧高度 */
  for (i = 0; i < nb; i++) {
    if (bx[i] >= x0 - EPS && bx[i] < x1 - EPS) {
      bh[i] = top;
    } else {
      bh[i] = seg_height_at(s, *m, bx[i]);
    }
  }
  /* 仅保留高度发生变化的边界 */
  nm = 0;
  for (i = 0; i < nb; i++) {
    if (nm == 0 ||
        fabs(bh[i] - s[nm - 1].h) > EPS * (1.0 + fabs(bh[i]))) {
      s[nm].x = bx[i];
      s[nm].h = bh[i];
      nm++;
    }
  }
  *m = nm;
  return 0;
}

/* 插入排序：按 key 降序（相等保持原顺序，稳定） */
static void sort_desc(const double *key, const int *in, int n, int *out)
{
  int i, j;
  for (i = 0; i < n; i++) {
    out[i] = in[i];
  }
  for (i = 1; i < n; i++) {
    int v = in[i];
    double kv = key[v];
    for (j = i - 1; j >= 0 && key[out[j]] < kv - EPS; j--) {
      out[j + 1] = out[j];
    }
    out[j + 1] = v;
  }
}

double refine_last_plate(const double *len, const double *wid, int n,
                         double gap, double usableLen, double usableWid,
                         double *outX, double *outY,
                         double *outLen, double *outWid)
{
  static double ew[REFINE_MAXN], eh[REFINE_MAXN];
  static double ox[REFINE_MAXN], oy[REFINE_MAXN];
  static double oxBest[REFINE_MAXN], oyBest[REFINE_MAXN];
  static double key[REFINE_MAXN];
  static int ord0[REFINE_MAXN], ord[REFINE_MAXN];
  static int rotated[REFINE_MAXN], rotBest[REFINE_MAXN];
  static Seg sk[2 * REFINE_MAXN + 8];
  int i, no, k;
  double bestUsed = -1.0;
  int haveBest = 0;

  if (n <= 0 || n > REFINE_MAXN || usableWid <= 0.0 || usableLen <= 0.0) {
    return -1.0;
  }

  for (i = 0; i < n; i++) {
    ew[i] = len[i] + gap; /* 内部尺寸：实际 + 间距 */
    eh[i] = wid[i] + gap;
    ord0[i] = i;
  }

  for (no = 0; no < 4; no++) {
    double used2 = 0.0;
    int m = 1;
    int fail = 0;

    switch (no) {
    case 0: /* 最大边降序 */
      for (i = 0; i < n; i++) {
        key[i] = ew[i] > eh[i] ? ew[i] : eh[i];
      }
      sort_desc(key, ord0, n, ord);
      break;
    case 1: /* 面积降序 */
      for (i = 0; i < n; i++) {
        key[i] = ew[i] * eh[i];
      }
      sort_desc(key, ord0, n, ord);
      break;
    case 2: /* 宽度（x 向）降序 */
      for (i = 0; i < n; i++) {
        key[i] = ew[i];
      }
      sort_desc(key, ord0, n, ord);
      break;
    default: /* 原始顺序 */
      for (i = 0; i < n; i++) {
        ord[i] = ord0[i];
      }
      break;
    }

    memset(rotated, 0, sizeof(int) * (size_t)n);
    sk[0].x = 0.0;
    sk[0].h = 0.0;

    for (k = 0; k < n && !fail; k++) {
      int p = ord[k];
      int o, ii;
      double bTop = -1.0, bX = 0.0, bw = 0.0;
      int bSeg = -1;
      for (o = 0; o < 2; o++) {
        double w, h;
        if (o == 0) {
          w = ew[p];
          h = eh[p];
        } else {
          if (fabs(ew[p] - eh[p]) <= EPS) {
            continue; /* 正方形旋转无意义 */
          }
          w = eh[p];
          h = ew[p];
        }
        for (ii = 0; ii < m; ii++) {
          int j = ii;
          double runEnd, top;
          /* 与区段 ii 等高的连续区段构成可放楼面 [s[ii].x, runEnd) */
          while (j + 1 < m &&
                 fabs(sk[j + 1].h - sk[ii].h) <= EPS * (1.0 + fabs(sk[ii].h))) {
            j++;
          }
          runEnd = (j + 1 < m) ? sk[j + 1].x : 1.0e300;
          if (runEnd - sk[ii].x >= w - EPS) {
            top = sk[ii].h + h;
            /* 选择准则：最左优先，同 x 取更低落点（最小化 x 向长度） */
            if (top <= usableWid + 1e-6 &&
                (bSeg < 0 || sk[ii].x < bX - EPS ||
                 (fabs(sk[ii].x - bX) <= EPS && top < bTop - EPS))) {
              bTop = top;
              bX = sk[ii].x;
              bSeg = ii;
              bw = w;
              rotated[p] = (o == 1);
            }
          }
          ii = j;
        }
      }
      if (bSeg < 0) {
        fail = 1; /* 该顺序放不下（超宽） */
        break;
      }
      ox[p] = bX;
      oy[p] = sk[bSeg].h;
      if (seg_insert(sk, &m, bX, bX + bw, bTop) != 0) {
        fail = 1;
        break;
      }
      if (bX + bw > used2) {
        used2 = bX + bw;
      }
    }

    if (!fail && (!haveBest || used2 < bestUsed - EPS)) {
      haveBest = 1;
      bestUsed = used2;
      for (i = 0; i < n; i++) {
        oxBest[i] = ox[i];
        oyBest[i] = oy[i];
        rotBest[i] = rotated[i];
      }
    }
  }

  if (!haveBest) {
    return -1.0;
  }

  for (i = 0; i < n; i++) {
    outX[i] = oxBest[i];
    outY[i] = oyBest[i];
    if (rotBest[i]) {
      outLen[i] = eh[i] - gap; /* 旋转：x 向 = 原宽 */
      outWid[i] = ew[i] - gap; /*        y 向 = 原长 */
    } else {
      outLen[i] = ew[i] - gap;
      outWid[i] = eh[i] - gap;
    }
  }
  return bestUsed;
}
