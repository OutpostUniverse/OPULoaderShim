# OPULoaderShim

OPULoaderShim injects the op2ext mod loader into Outpost 2. It works by shimming the `winmm.dll` system DLL (with a fallback mode replacing `helphook.dll`) which `Outpost2.exe` requires, and then passively loads `op2ext.dll`.

The use of OPULoaderShim does not result in any functional differences with `winmm.dll` (or `helphook.dll`) interfaces.

Note that while OPULoaderShim and therefore op2ext are unconditionally loaded into Outpost 2, op2ext does not activate unless the `/OPU` command switch is passed to the game when loaded. This allows the unmodified game to still be played.
