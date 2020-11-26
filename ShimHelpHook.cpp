
// Reimplement functionality of helphook.dll.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define EXPORT __declspec(dllexport)

static HINSTANCE  g_hInst        = NULL;
static HHOOK      g_hHook        = NULL;
static HWND       g_hHookedWnd   = NULL;
static HWND       g_hSrcWnd      = NULL;
static int        g_onDestroyMsg = -1;

// =====================================================================================================================
BOOL APIENTRY DllMain(
  HINSTANCE  hInstDll,
  DWORD      fdwReason,
  LPVOID     lpvReserved)
{
  if (fdwReason == DLL_PROCESS_ATTACH) {
    g_hInst = hInstDll;
  }
  return TRUE;
}

// =====================================================================================================================
EXPORT int __fastcall MyFreeHook() {
  if (g_hHook != NULL) {
    UnhookWindowsHookEx(g_hHook);
    g_hHook = NULL;
  }
  return 1;
}

// =====================================================================================================================
EXPORT LRESULT WINAPI MyCBTProc(
  int     nCode,
  WPARAM  wParam,
  LPARAM  lParam)
{
  if ((nCode == 4) && (g_hSrcWnd == reinterpret_cast<HWND>(wParam))) {
    PostMessageA(g_hHookedWnd, g_onDestroyMsg, 0, 0);
    MyFreeHook();
  }
  return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

// =====================================================================================================================
EXPORT LRESULT WINAPI MyShellProc(
  int     nCode,
  WPARAM  wParam,
  LPARAM  lParam)
{
  if ((nCode == 2) && (g_hSrcWnd == reinterpret_cast<HWND>(wParam))) {
    PostMessageA(g_hHookedWnd, g_onDestroyMsg, 0, 0);
    MyFreeHook();
  }
  return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

// =====================================================================================================================
EXPORT int __fastcall MySetHook(
  int  hHookWnd,
  int  hSrcWnd,
  int  destroyMsg)
{
  if (g_hHook == NULL) {
    g_hHookedWnd   = reinterpret_cast<HWND>(hHookWnd);
    g_hSrcWnd      = reinterpret_cast<HWND>(hSrcWnd);
    g_onDestroyMsg = destroyMsg;

    DWORD processId;
    GetWindowThreadProcessId(g_hHookedWnd, &processId);
    g_hHook = SetWindowsHookExA(10, &MyShellProc, g_hInst, processId);
  }

  return 1;
}
