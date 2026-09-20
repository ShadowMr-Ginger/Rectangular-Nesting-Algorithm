/*
 * File: last_plate_refine.h
 *
 * 末板 skyline 再压缩（后处理）：在 Nest_and_reNest 末板重套料完成之后，
 * 对最后一张板的零件集合用 skyline（天际线）启发式重新排列，
 * 直接最小化零件最右端边界 —— 等价于按用户定义最大化“一刀切”余料面积
 * （最右端竖直切一刀，右侧剩余板料全部计为有效余料）。
 *
 * 只作用于最后一张板，不影响主套料流程，额外耗时为微秒级。
 */

#ifndef LAST_PLATE_REFINE_H
#define LAST_PLATE_REFINE_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Arguments:
 *   len, wid : 零件实际尺寸（未加间距），长度 n
 *   gap      : 零件间距 interval_distance_parts（内部将零件放大 gap，与
 *              nest_and_calc_Ver5 的坐标模型一致）
 *   usableLen, usableWid : 内部可用区域尺寸（调用方按 nest_and_calc_Ver5
 *              的规则由板长/板宽/边距换算）
 *   outX, outY   : 输出内部左下坐标（预分配，长度 n）
 *   outLen,outWid: 输出实际摆放尺寸（选定方向后的 x/y 向长度，预分配）
 * Returns:
 *   成功：内部坐标系下达到的最右端 usedLen（> 0）
 *   失败（如零件放不进 usableWid）：-1
 */
double refine_last_plate(const double *len, const double *wid, int n,
                         double gap, double usableLen, double usableWid,
                         double *outX, double *outY,
                         double *outLen, double *outWid);

#ifdef __cplusplus
}
#endif

#endif /* LAST_PLATE_REFINE_H */
