
// Passively loads op2ext.dll by importing from it.
// It is op2ext's responsibility to sanity check Outpost2.exe is the loading process before attempting to patch it.

extern "C" __declspec(dllimport) volatile int StubExt; // Dummy import to force loading of op2ext.dll.
namespace { auto*volatile g_pStubExt = &StubExt; }     // Dummy to prevent StubExt from being optimized away.
