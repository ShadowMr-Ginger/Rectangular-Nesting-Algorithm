using System.Runtime.InteropServices;

namespace Backend.Native;

/// <summary>
/// nesting.dll 的 C 接口 P/Invoke 声明。
/// 该 DLL 非线程安全，所有调用必须经过 <see cref="NestingLibrary.SyncRoot"/> 串行化。
/// </summary>
internal static class NativeMethods
{
    /// <summary>
    /// 运行矩形件套料。
    /// layouts / sheetDetails / partIsNested 为 C# 预分配数组：
    /// blittable double 数组默认 pin 住并在调用后回拷，与 [In, Out] 语义一致，
    /// 这里显式标注 [Out] 强调这些缓冲主要由原生侧写出。
    /// 输出布局由 wrapper 以逐行连续（row-major）形式回拷：layouts 每行 6 列、
    /// sheetDetails 每行 4 列。
    /// </summary>
    [DllImport("nesting.dll", CallingConvention = CallingConvention.Cdecl)]
    internal static extern int nesting_run(
        double[] partLengths, double[] partWidths, int numParts,
        double plateLength, double plateWidth,
        double tightnessCoef, double intervalParts, double intervalPlate,
        double sCoef, double reNestStep,
        [Out] double[] layouts, [Out] double[] sheetDetails, [Out] double[] partIsNested,
        out double numPlates, out double lastPlateSurplus, out double utilization);
}
