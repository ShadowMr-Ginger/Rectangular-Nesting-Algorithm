using System.Runtime.InteropServices;

namespace Backend.Native;

/// <summary>
/// 封装 nesting.dll 的加载状态与调用。DLL 非线程安全，用全局 lock 串行化。
/// </summary>
internal static class NestingLibrary
{
    /// <summary>串行化所有原生调用的全局锁。</summary>
    internal static readonly object SyncRoot = new();

    private static bool? _available;

    /// <summary>
    /// nesting.dll 是否可被加载。首次调用时做一次试探性 P/Invoke。
    /// </summary>
    internal static bool IsAvailable
    {
        get
        {
            if (_available.HasValue) return _available.Value;
            lock (SyncRoot)
            {
                try
                {
                    double numPlates, surplus, utilization;
                    // 空数组探测：numParts=0，只验证 DLL 能被加载且入口存在。
                    NativeMethods.nesting_run(
                        Array.Empty<double>(), Array.Empty<double>(), 0,
                        1.0, 1.0, 1.0, 0.0, 0.0, 0.1, 1.0,
                        Array.Empty<double>(), Array.Empty<double>(), Array.Empty<double>(),
                        out numPlates, out surplus, out utilization);
                    _available = true;
                }
                catch (DllNotFoundException)
                {
                    _available = false;
                }
                catch (EntryPointNotFoundException)
                {
                    _available = false;
                }
                catch (MarshalDirectiveException)
                {
                    _available = false;
                }
                return _available.Value;
            }
        }
    }
}
