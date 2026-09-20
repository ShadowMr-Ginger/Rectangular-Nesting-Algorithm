# nesting.dll 冒烟测试

临时验证工具，确认 `nesting.dll` 导出函数 `nesting_run` 能正确完成套料。

## 运行

已在 `test/` 下编译好 `smoke_test.exe`，并把 `nesting.dll` 复制到了同目录，直接双击或：

```bash
cd online-platform/native/test
./smoke_test.exe
```

若重新编译了 DLL，需要重新复制：

```bash
cp ../x64/Release/nesting.dll .
```

## 重新编译测试程序

测试程序只依赖 `windows.h`，通过 `LoadLibrary` 在运行时加载 DLL，不需要链接 import lib。

Git Bash 中（把 `<ver>` 换成实际版本目录，如 `14.51.36231`）：

```bash
export VCDIR="/c/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/<ver>"
export KIT="/c/Program Files (x86)/Windows Kits/10"
export INCLUDE="$VCDIR/include;$KIT/Include/10.0.26100.0/um;$KIT/Include/10.0.26100.0/ucrt;$KIT/Include/10.0.26100.0/shared"
export LIB="$VCDIR/lib/x64;$KIT/Lib/10.0.26100.0/um/x64;$KIT/Lib/10.0.26100.0/ucrt/x64"
export PATH="$VCDIR/bin/Hostx64/x64:$PATH"
cd online-platform/native/test
cl //EHsc //MD //nologo smoke_test.cpp
```

## 测试内容

- 板材 100×50，10 个零件（长宽伪随机 1~49，种子 42 可复现）
- 参数：tightnessCoef=50, intervalParts=2, intervalPlate=2, sCoef=0.1, reNestStep=40
- 检查项：返回码为 0；所有零件 `partIsNested=1`；坐标均在板材范围内；
  同板零件两两不重叠；打印 numPlates / utilization / 各板 sheetDetails

预期输出结尾为 `SMOKE TEST PASSED`。当前一次运行结果：numPlates=3，utilization≈0.792。

注意：算法允许旋转零件，因此 `layouts` 第 4/5 列（x/y 方向长度）可能与输入长宽互换，属正常行为。
