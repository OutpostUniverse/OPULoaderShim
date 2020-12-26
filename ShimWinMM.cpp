
// Wrap functionality of winmm.dll.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdlib>

#define EXPORT __declspec(dllexport)


// WinMM typedefs
DECLARE_HANDLE(HMMIO);
DECLARE_HANDLE(HPSTR);
DECLARE_HANDLE(HDRVR);
DECLARE_HANDLE(HMIDI);
DECLARE_HANDLE(HMIDIIN);
DECLARE_HANDLE(HMIDIOUT);
DECLARE_HANDLE(HMIDISTRM);
DECLARE_HANDLE(HMIXER);
DECLARE_HANDLE(HMIXEROBJ);
DECLARE_HANDLE(HWAVEIN);
DECLARE_HANDLE(HWAVEOUT);
using LPHMIDIIN             = HMIDIIN*;
using LPHMIDIOUT            = HMIDIOUT*;
using LPHMIDISTRM           = HMIDISTRM*;
using LPHMIXER              = HMIXER*;
using LPHWAVEIN             = HWAVEIN*;
using LPHWAVEOUT            = HWAVEOUT*;
using MMRESULT              = UINT;
using MCIDEVICEID           = DWORD;
using MCIERROR              = DWORD;
using FOURCC                = DWORD;
using LPAUXCAPSA            = void*;
using LPAUXCAPSW            = void*;
using LPMIDIHDR             = void*;
using LPMIDIINCAPSA         = void*;
using LPMIDIINCAPSW         = void*;
using LPMIDIOUTCAPSA        = void*;
using LPMIDIOUTCAPSW        = void*;
using LPMMTIME              = void*;
using LPMIXERCONTROLDETAILS = void*;
using LPMIXERCAPSA          = void*;
using LPMIXERCAPSW          = void*;
using LPMIXERLINEA          = void*;
using LPMIXERLINEW          = void*;
using LPMIXERLINECONTROLSA  = void*;
using LPMIXERLINECONTROLSW  = void*;
using LPWAVEHDR             = void*;
using LPWAVEINCAPSA         = void*;
using LPWAVEINCAPSW         = void*;
using LPWAVEOUTCAPSA        = void*;
using LPWAVEOUTCAPSW        = void*;
using LPJOYINFO             = void*;
using LPJOYINFOEX           = void*;
using LPJOYCAPSA            = void*;
using LPJOYCAPSW            = void*;
using LPTIMECAPS            = void*;
using LPMMIOINFO            = void*;
using LPMMCKINFO            = void*;
using LPCMMIOINFO           = const void*;
using LPCMMCKINFO           = const void*;
using LPCWAVEFORMATEX       = const void*;
using LPMMIOPROC            = void(WINAPI*)();
using YIELDPROC             = void(WINAPI*)();
using LPTIMECALLBACK        = void(WINAPI*)();


// =====================================================================================================================
// Calls the given function only once on one thread during execution.  Each template instance is expected to be unique.
// This is used because Windows XP does not support magic statics or std::call_once.
template <typename T>
static void CallOnce(
  T&&  callable)
{
  static volatile unsigned int flag = 0;
  if (InterlockedExchange(&flag, 1) == 0) {
    callable();
  }
}

// =====================================================================================================================
// Loads the real winmm.dll, registers an exit handler to unload it, and returns the module handle.
static HMODULE GetWinmm() {
  static volatile HMODULE hWinmm = NULL;
  CallOnce([] {
    wchar_t dllPath[MAX_PATH] = L"";
    GetSystemDirectoryW(&dllPath[0], MAX_PATH);
    wcsncat_s(&dllPath[0], MAX_PATH, L"\\winmm.dll", _TRUNCATE);

    hWinmm = LoadLibraryW(&dllPath[0]);
    if (hWinmm != NULL) {
      atexit([] { FreeLibrary(hWinmm);  hWinmm = NULL; });
    }
  });
  return hWinmm;
}

// =====================================================================================================================
// Gets the specified export from the real winmm.dll.
template <auto* Pfn>
static auto* GetFunction(
  const char*  pExport)
{
  static volatile decltype(Pfn) pfn = nullptr;
  CallOnce([pExport] { pfn = reinterpret_cast<decltype(Pfn)>(GetProcAddress(GetWinmm(), pExport)); });
  return pfn;
}

// ------------------------------------------------ WinMM API wrappers -------------------------------------------------

extern "C" {

EXPORT int WINAPI mciExecute(PCHAR MbString) { return GetFunction<&mciExecute>(__func__)(MbString); }

EXPORT LRESULT WINAPI CloseDriver(HDRVR hDriver, LPARAM lParam1, LPARAM lParam2)
  { return GetFunction<&CloseDriver>(__func__)(hDriver, lParam1, lParam2); }

EXPORT LRESULT WINAPI DefDriverProc(
  DWORD_PTR dwDriverIdentifier, HDRVR hdrvr, UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
  return GetFunction<&DefDriverProc>(__func__)(dwDriverIdentifier, hdrvr, uMsg, lParam1, lParam2);
}

EXPORT int WINAPI DriverCallback(HWND hWnd, int a, WPARAM wParam, UINT uMsg, int b, LPARAM lParam, int c)
  { return GetFunction<&DriverCallback>(__func__)(hWnd, a, wParam, uMsg, b, lParam, c); }

EXPORT HMODULE WINAPI DrvGetModuleHandle(HDRVR hDriver) { return GetFunction<&DrvGetModuleHandle>(__func__)(hDriver); }

EXPORT HMODULE WINAPI GetDriverModuleHandle(HDRVR hDriver)
  { return GetFunction<&GetDriverModuleHandle>(__func__)(hDriver); }

EXPORT int WINAPI NotifyCallbackData(int a, int b, int c, int d, int e)
  { return GetFunction<&NotifyCallbackData>(__func__)(a, b, c, d, e); }

EXPORT HDRVR WINAPI OpenDriver(LPCWSTR szDriverName, LPCWSTR szSectionName, LPARAM lParam2)
  { return GetFunction<&OpenDriver>(__func__)(szDriverName, szSectionName, lParam2); }

EXPORT BOOL WINAPI PlaySoundA(LPCSTR pszSound, HMODULE hmod, DWORD fdwSound)  // Aliases: @2, PlaySound
  { return GetFunction<&PlaySoundA>(__func__)(pszSound, hmod, fdwSound); }

EXPORT BOOL WINAPI PlaySoundW(LPCWSTR pszSound, HMODULE hmod, DWORD fdwSound)
  { return GetFunction<&PlaySoundW>(__func__)(pszSound, hmod, fdwSound); }

EXPORT LRESULT WINAPI SendDriverMessage(HDRVR hDriver, UINT message, LPARAM lParam1, LPARAM lParam2)
  { return GetFunction<&SendDriverMessage>(__func__)(hDriver, message, lParam1, lParam2); }

EXPORT int WINAPI WOW32DriverCallback(int a, int b, WORD c, WORD d, int e, LPARAM lParam, int f)
  { return GetFunction<&WOW32DriverCallback>(__func__)(a, b, c, d, e, lParam, f); }

EXPORT int WINAPI WOW32ResolveMultiMediaHandle(int a, int b, int c, int d, int e, int f)
  { return GetFunction<&WOW32ResolveMultiMediaHandle>(__func__)(a, b, c, d, e, f); }

EXPORT int WINAPI WOWAppExit(int a) { return GetFunction<&WOWAppExit>(__func__)(a); }

EXPORT int WINAPI aux32Message(UINT uDeviceID, UINT uMsg, int a, DWORD_PTR dw1, DWORD_PTR dw2)
  { return GetFunction<&aux32Message>(__func__)(uDeviceID, uMsg, a, dw1, dw2); }

EXPORT MMRESULT WINAPI auxGetDevCapsA(UINT_PTR uDeviceID, LPAUXCAPSA pac, UINT cbac)
  { return GetFunction<&auxGetDevCapsA>(__func__)(uDeviceID, pac, cbac); }

EXPORT MMRESULT WINAPI auxGetDevCapsW(UINT_PTR uDeviceID, LPAUXCAPSW pac, UINT cbac)
  { return GetFunction<&auxGetDevCapsW>(__func__)(uDeviceID, pac, cbac); }

EXPORT UINT WINAPI auxGetNumDevs() { return GetFunction<&auxGetNumDevs>(__func__)(); }

EXPORT MMRESULT WINAPI auxGetVolume(UINT uDeviceID, LPDWORD pdwVolume)
  { return GetFunction<&auxGetVolume>(__func__)(uDeviceID, pdwVolume); }

EXPORT MMRESULT WINAPI auxOutMessage(UINT uDeviceID, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2)
  { return GetFunction<&auxOutMessage>(__func__)(uDeviceID, uMsg, dw1, dw2); }

EXPORT MMRESULT WINAPI auxSetVolume(UINT uDeviceID, DWORD dwVolume)
  { return GetFunction<&auxSetVolume>(__func__)(uDeviceID, dwVolume); }

EXPORT int WINAPI joy32Message(UINT uJoyID, int a, int b, int c, size_t Size)
  { return GetFunction<&joy32Message>(__func__)(uJoyID, a, b, c, Size); }

EXPORT MMRESULT WINAPI joyConfigChanged(DWORD dwFlags) { return GetFunction<&joyConfigChanged>(__func__)(dwFlags); }

EXPORT MMRESULT WINAPI joyGetDevCapsA(UINT_PTR uJoyID, LPJOYCAPSA pjc, UINT cbjc)
  { return GetFunction<&joyGetDevCapsA>(__func__)(uJoyID, pjc, cbjc); }

EXPORT MMRESULT WINAPI joyGetDevCapsW(UINT_PTR uJoyID, LPJOYCAPSW pjc, UINT cbjc)
  { return GetFunction<&joyGetDevCapsW>(__func__)(uJoyID, pjc, cbjc); }

EXPORT UINT WINAPI joyGetNumDevs() { return GetFunction<&joyGetNumDevs>(__func__)(); }

EXPORT MMRESULT WINAPI joyGetPos(UINT uJoyID, LPJOYINFO pji) { return GetFunction<&joyGetPos>(__func__)(uJoyID, pji); }

EXPORT MMRESULT WINAPI joyGetPosEx(UINT uJoyID, LPJOYINFOEX pji)
  { return GetFunction<&joyGetPosEx>(__func__)(uJoyID, pji); }

EXPORT MMRESULT WINAPI joyGetThreshold(UINT uJoyID, LPUINT puThreshold)
  { return GetFunction<&joyGetThreshold>(__func__)(uJoyID, puThreshold); }

EXPORT MMRESULT WINAPI joyReleaseCapture(UINT uJoyID) { return GetFunction<&joyReleaseCapture>(__func__)(uJoyID); }

EXPORT MMRESULT WINAPI joySetCapture(HWND hwnd, UINT uJoyID, UINT uPeriod, BOOL fChanged)
  { return GetFunction<&joySetCapture>(__func__)(hwnd, uJoyID, uPeriod, fChanged); }

EXPORT MMRESULT WINAPI joySetThreshold(UINT uJoyID, UINT uThreshold)
  { return GetFunction<&joySetThreshold>(__func__)(uJoyID, uThreshold); }

EXPORT int WINAPI mci32Message(int a, DWORD dwElementID, int b, UINT cchText, int c)
  { return GetFunction<&mci32Message>(__func__)(a, dwElementID, b, cchText, c); }

EXPORT int WINAPI mciDriverNotify(HWND hWnd, LPARAM lParam, WPARAM wParam)
  { return GetFunction<&mciDriverNotify>(__func__)(hWnd, lParam, wParam); }

EXPORT int WINAPI mciDriverYield(int a) { return GetFunction<&mciDriverYield>(__func__)(a); }

EXPORT int WINAPI mciFreeCommandResource(int a) { return GetFunction<&mciFreeCommandResource>(__func__)(a); }

EXPORT HTASK WINAPI mciGetCreatorTask(MCIDEVICEID mciId) { return GetFunction<&mciGetCreatorTask>(__func__)(mciId); }

EXPORT MCIDEVICEID WINAPI mciGetDeviceIDA(LPCSTR pszDevice)
  { return GetFunction<&mciGetDeviceIDA>(__func__)(pszDevice); }

EXPORT MCIDEVICEID WINAPI mciGetDeviceIDFromElementIDA(DWORD dwElementID, LPCSTR lpstrType)
  { return GetFunction<&mciGetDeviceIDFromElementIDA>(__func__)(dwElementID, lpstrType); }

EXPORT MCIDEVICEID WINAPI mciGetDeviceIDFromElementIDW(DWORD dwElementID, LPCWSTR lpstrType)
  { return GetFunction<&mciGetDeviceIDFromElementIDW>(__func__)(dwElementID, lpstrType); }

EXPORT MCIDEVICEID WINAPI mciGetDeviceIDW(LPCWSTR pszDevice)
  { return GetFunction<&mciGetDeviceIDW>(__func__)(pszDevice); }

EXPORT int WINAPI mciGetDriverData(int a) { return GetFunction<&mciGetDriverData>(__func__)(a); }

EXPORT BOOL WINAPI mciGetErrorStringA(MCIERROR mcierr, LPSTR pszText, UINT cchText)
  { return GetFunction<&mciGetErrorStringA>(__func__)(mcierr, pszText, cchText); }

EXPORT BOOL WINAPI mciGetErrorStringW(MCIERROR mcierr, LPWSTR pszText, UINT cchText)
  { return GetFunction<&mciGetErrorStringW>(__func__)(mcierr, pszText, cchText); }

EXPORT YIELDPROC WINAPI mciGetYieldProc(MCIDEVICEID mciId, LPDWORD pdwYieldData)
  { return GetFunction<&mciGetYieldProc>(__func__)(mciId, pdwYieldData); }

EXPORT int WINAPI mciLoadCommandResource(int a, LPCWSTR lpName, void* b)
  { return GetFunction<&mciLoadCommandResource>(__func__)(a, lpName, b); }

EXPORT MCIERROR WINAPI mciSendCommandA(MCIDEVICEID mciId, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
  { return GetFunction<&mciSendCommandA>(__func__)(mciId, uMsg, dwParam1, dwParam2); }

EXPORT MCIERROR WINAPI mciSendCommandW(MCIDEVICEID mciId, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
  { return GetFunction<&mciSendCommandW>(__func__)(mciId, uMsg, dwParam1, dwParam2); }

EXPORT MCIERROR WINAPI mciSendStringA(
  LPCSTR lpstrCommand, LPSTR lpstrReturnString, UINT uReturnLength, HWND hwndCallback)
{
  return GetFunction<&mciSendStringA>(__func__)(lpstrCommand, lpstrReturnString, uReturnLength, hwndCallback);
}

EXPORT MCIERROR WINAPI mciSendStringW(
  LPCWSTR lpstrCommand, LPWSTR lpstrReturnString, UINT uReturnLength, HWND hwndCallback)
{
  return GetFunction<&mciSendStringW>(__func__)(lpstrCommand, lpstrReturnString, uReturnLength, hwndCallback);
}

EXPORT int WINAPI mciSetDriverData(int a, int b) { return GetFunction<&mciSetDriverData>(__func__)(a, b); }

EXPORT BOOL WINAPI mciSetYieldProc(MCIDEVICEID mciId, YIELDPROC fpYieldProc, DWORD dwYieldData)
  { return GetFunction<&mciSetYieldProc>(__func__)(mciId, fpYieldProc, dwYieldData); }

EXPORT int WINAPI mid32Message(UINT_PTR uDeviceID, UINT uMsg, HMIDIIN hmi, DWORD_PTR dw1, size_t Size)
  { return GetFunction<&mid32Message>(__func__)(uDeviceID, uMsg, hmi, dw1, Size); }

EXPORT MMRESULT WINAPI midiConnect(HMIDI hmi, HMIDIOUT hmo, LPVOID pReserved)
  { return GetFunction<&midiConnect>(__func__)(hmi, hmo, pReserved); }

EXPORT MMRESULT WINAPI midiDisconnect(HMIDI hmi, HMIDIOUT hmo, LPVOID pReserved)
  { return GetFunction<&midiDisconnect>(__func__)(hmi, hmo, pReserved); }

EXPORT MMRESULT WINAPI midiInAddBuffer(HMIDIIN hmi, LPMIDIHDR pmh, UINT cbmh)
  { return GetFunction<&midiInAddBuffer>(__func__)(hmi, pmh, cbmh); }

EXPORT MMRESULT WINAPI midiInClose(HMIDIIN hmi) { return GetFunction<&midiInClose>(__func__)(hmi); }

EXPORT MMRESULT WINAPI midiInGetDevCapsA(UINT_PTR uDeviceID, LPMIDIINCAPSA pmic, UINT cbmic)
  { return GetFunction<&midiInGetDevCapsA>(__func__)(uDeviceID, pmic, cbmic); }

EXPORT MMRESULT WINAPI midiInGetDevCapsW(UINT_PTR uDeviceID, LPMIDIINCAPSW pmic, UINT cbmic)
  { return GetFunction<&midiInGetDevCapsW>(__func__)(uDeviceID, pmic, cbmic); }

EXPORT MMRESULT WINAPI midiInGetErrorTextA(MMRESULT mmrError, LPSTR pszText, UINT cchText)
  { return GetFunction<&midiInGetErrorTextA>(__func__)(mmrError, pszText, cchText); }

EXPORT MMRESULT WINAPI midiInGetErrorTextW(MMRESULT mmrError, LPWSTR pszText, UINT cchText)
  { return GetFunction<&midiInGetErrorTextW>(__func__)(mmrError, pszText, cchText); }

EXPORT MMRESULT WINAPI midiInGetID(HMIDIIN hmi, LPUINT puDeviceID)
  { return GetFunction<&midiInGetID>(__func__)(hmi, puDeviceID); }

EXPORT UINT WINAPI midiInGetNumDevs() { return GetFunction<&midiInGetNumDevs>(__func__)(); }

EXPORT MMRESULT WINAPI midiInMessage(HMIDIIN hmi, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2)
  { return GetFunction<&midiInMessage>(__func__)(hmi, uMsg, dw1, dw2); }

EXPORT MMRESULT WINAPI midiInOpen(
  LPHMIDIIN phmi, UINT uDeviceID, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen)
{
  return GetFunction<&midiInOpen>(__func__)(phmi, uDeviceID, dwCallback, dwInstance, fdwOpen);
}

EXPORT MMRESULT WINAPI midiInPrepareHeader(HMIDIIN hmi, LPMIDIHDR pmh, UINT cbmh)
  { return GetFunction<&midiInPrepareHeader>(__func__)(hmi, pmh, cbmh); }

EXPORT MMRESULT WINAPI midiInReset(HMIDIIN hmi) { return GetFunction<&midiInReset>(__func__)(hmi); }

EXPORT MMRESULT WINAPI midiInStart(HMIDIIN hmi) { return GetFunction<&midiInStart>(__func__)(hmi); }

EXPORT MMRESULT WINAPI midiInStop(HMIDIIN hmi) { return GetFunction<&midiInStop>(__func__)(hmi); }

EXPORT MMRESULT WINAPI midiInUnprepareHeader(HMIDIIN hmi, LPMIDIHDR pmh, UINT cbmh)
  { return GetFunction<&midiInUnprepareHeader>(__func__)(hmi, pmh, cbmh); }

EXPORT MMRESULT WINAPI midiOutCacheDrumPatches(HMIDIOUT hmo, UINT uPatch, LPWORD pwkya, UINT fuCache)
  { return GetFunction<&midiOutCacheDrumPatches>(__func__)(hmo, uPatch, pwkya, fuCache); }

EXPORT MMRESULT WINAPI midiOutCachePatches(HMIDIOUT hmo, UINT uBank, LPWORD pwpa, UINT fuCache)
  { return GetFunction<&midiOutCachePatches>(__func__)(hmo, uBank, pwpa, fuCache); }

EXPORT MMRESULT WINAPI midiOutClose(HMIDIOUT hmo) { return GetFunction<&midiOutClose>(__func__)(hmo); }

EXPORT MMRESULT WINAPI midiOutGetDevCapsA(UINT_PTR uDeviceID, LPMIDIOUTCAPSA pmoc, UINT cbmoc)
  { return GetFunction<&midiOutGetDevCapsA>(__func__)(uDeviceID, pmoc, cbmoc); }

EXPORT MMRESULT WINAPI midiOutGetDevCapsW(UINT_PTR uDeviceID, LPMIDIOUTCAPSW pmoc, UINT cbmoc)
  { return GetFunction<&midiOutGetDevCapsW>(__func__)(uDeviceID, pmoc, cbmoc); }

EXPORT MMRESULT WINAPI midiOutGetErrorTextA(MMRESULT mmrError, LPSTR pszText, UINT cchText)
  { return GetFunction<&midiOutGetErrorTextA>(__func__)(mmrError, pszText, cchText); }

EXPORT MMRESULT WINAPI midiOutGetErrorTextW(MMRESULT mmrError, LPWSTR pszText, UINT cchText)
  { return GetFunction<&midiOutGetErrorTextW>(__func__)(mmrError, pszText, cchText); }

EXPORT MMRESULT WINAPI midiOutGetID(HMIDIOUT hmo, LPUINT puDeviceID)
  { return GetFunction<&midiOutGetID>(__func__)(hmo, puDeviceID); }

EXPORT UINT WINAPI midiOutGetNumDevs() { return GetFunction<&midiOutGetNumDevs>(__func__)(); }

EXPORT MMRESULT WINAPI midiOutGetVolume(HMIDIOUT hmo, LPDWORD pdwVolume)
  { return GetFunction<&midiOutGetVolume>(__func__)(hmo, pdwVolume); }

EXPORT MMRESULT WINAPI midiOutLongMsg(HMIDIOUT hmo, LPMIDIHDR pmh, UINT cbmh)
  { return GetFunction<&midiOutLongMsg>(__func__)(hmo, pmh, cbmh); }

EXPORT MMRESULT WINAPI midiOutMessage(HMIDIOUT hmo, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2)
  { return GetFunction<&midiOutMessage>(__func__)(hmo, uMsg, dw1, dw2); }

EXPORT MMRESULT WINAPI midiOutOpen(
  LPHMIDIOUT phmo, UINT uDeviceID, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen)
{
  return GetFunction<&midiOutOpen>(__func__)(phmo, uDeviceID, dwCallback, dwInstance, fdwOpen);
}

EXPORT MMRESULT WINAPI midiOutPrepareHeader(HMIDIOUT hmo, LPMIDIHDR pmh, UINT cbmh)
  { return GetFunction<&midiOutPrepareHeader>(__func__)(hmo, pmh, cbmh); }

EXPORT MMRESULT WINAPI midiOutReset(HMIDIOUT hmo) { return GetFunction<&midiOutReset>(__func__)(hmo); }

EXPORT MMRESULT WINAPI midiOutSetVolume(HMIDIOUT hmo, DWORD dwVolume)
  { return GetFunction<&midiOutSetVolume>(__func__)(hmo, dwVolume); }

EXPORT MMRESULT WINAPI midiOutShortMsg(HMIDIOUT hmo, DWORD dwMsg)
  { return GetFunction<&midiOutShortMsg>(__func__)(hmo, dwMsg); }

EXPORT MMRESULT WINAPI midiOutUnprepareHeader(HMIDIOUT hmo, LPMIDIHDR pmh, UINT cbmh)
  { return GetFunction<&midiOutUnprepareHeader>(__func__)(hmo, pmh, cbmh); }

EXPORT MMRESULT WINAPI midiStreamClose(HMIDISTRM hms) { return GetFunction<&midiStreamClose>(__func__)(hms); }

EXPORT MMRESULT WINAPI midiStreamOpen(
  LPHMIDISTRM phms, LPUINT puDeviceID, DWORD cMidi, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen)
{
  return GetFunction<&midiStreamOpen>(__func__)(phms, puDeviceID, cMidi, dwCallback, dwInstance, fdwOpen);
}

EXPORT MMRESULT WINAPI midiStreamOut(HMIDISTRM hms, LPMIDIHDR pmh, UINT cbmh)
  { return GetFunction<&midiStreamOut>(__func__)(hms, pmh, cbmh); }

EXPORT MMRESULT WINAPI midiStreamPause(HMIDISTRM hms) { return GetFunction<&midiStreamPause>(__func__)(hms); }

EXPORT MMRESULT WINAPI midiStreamPosition(HMIDISTRM hms, LPMMTIME lpmmt, UINT cbmmt)
  { return GetFunction<&midiStreamPosition>(__func__)(hms, lpmmt, cbmmt); }

EXPORT MMRESULT WINAPI midiStreamProperty(HMIDISTRM hms, LPBYTE lppropdata, DWORD dwProperty)
  { return GetFunction<&midiStreamProperty>(__func__)(hms, lppropdata, dwProperty); }

EXPORT MMRESULT WINAPI midiStreamRestart(HMIDISTRM hms) { return GetFunction<&midiStreamRestart>(__func__)(hms); }

EXPORT MMRESULT WINAPI midiStreamStop(HMIDISTRM hms) { return GetFunction<&midiStreamStop>(__func__)(hms); }

EXPORT MMRESULT WINAPI mixerClose(HMIXER hmx) { return GetFunction<&mixerClose>(__func__)(hmx); }

EXPORT MMRESULT WINAPI mixerGetControlDetailsA(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails)
  { return GetFunction<&mixerGetControlDetailsA>(__func__)(hmxobj, pmxcd, fdwDetails); }

EXPORT MMRESULT WINAPI mixerGetControlDetailsW(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails)
  { return GetFunction<&mixerGetControlDetailsW>(__func__)(hmxobj, pmxcd, fdwDetails); }

EXPORT MMRESULT WINAPI mixerGetDevCapsA(UINT_PTR uMxId, LPMIXERCAPSA pmxcaps, UINT cbmxcaps)
  { return GetFunction<&mixerGetDevCapsA>(__func__)(uMxId, pmxcaps, cbmxcaps); }

EXPORT MMRESULT WINAPI mixerGetDevCapsW(UINT_PTR uMxId, LPMIXERCAPSW pmxcaps, UINT cbmxcaps)
  { return GetFunction<&mixerGetDevCapsW>(__func__)(uMxId, pmxcaps, cbmxcaps); }

EXPORT MMRESULT WINAPI mixerGetID(HMIXEROBJ hmxobj, UINT* puMxId, DWORD fdwId)
  { return GetFunction<&mixerGetID>(__func__)(hmxobj, puMxId, fdwId); }

EXPORT MMRESULT WINAPI mixerGetLineControlsA(HMIXEROBJ hmxobj, LPMIXERLINECONTROLSA pmxlc, DWORD fdwControls)
  { return GetFunction<&mixerGetLineControlsA>(__func__)(hmxobj, pmxlc, fdwControls); }

EXPORT MMRESULT WINAPI mixerGetLineControlsW(HMIXEROBJ hmxobj, LPMIXERLINECONTROLSW pmxlc, DWORD fdwControls)
  { return GetFunction<&mixerGetLineControlsW>(__func__)(hmxobj, pmxlc, fdwControls); }

EXPORT MMRESULT WINAPI mixerGetLineInfoA(HMIXEROBJ hmxobj, LPMIXERLINEA pmxl, DWORD fdwInfo)
  { return GetFunction<&mixerGetLineInfoA>(__func__)(hmxobj, pmxl, fdwInfo); }

EXPORT MMRESULT WINAPI mixerGetLineInfoW(HMIXEROBJ hmxobj, LPMIXERLINEW pmxl, DWORD fdwInfo)
  { return GetFunction<&mixerGetLineInfoW>(__func__)(hmxobj, pmxl, fdwInfo); }

EXPORT UINT WINAPI mixerGetNumDevs() { return GetFunction<&mixerGetNumDevs>(__func__)(); }

EXPORT DWORD WINAPI mixerMessage(HMIXER hmx, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
  { return GetFunction<&mixerMessage>(__func__)(hmx, uMsg, dwParam1, dwParam2); }

EXPORT MMRESULT WINAPI mixerOpen(
  LPHMIXER phmx, UINT uMxId, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen)
{
  return GetFunction<&mixerOpen>(__func__)(phmx, uMxId, dwCallback, dwInstance, fdwOpen);
}

EXPORT MMRESULT WINAPI mixerSetControlDetails(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails)
  { return GetFunction<&mixerSetControlDetails>(__func__)(hmxobj, pmxcd, fdwDetails); }

EXPORT int WINAPI mmDrvInstall(int a, int b, int c, int d) { return GetFunction<&mmDrvInstall>(__func__)(a, b, c, d); }

EXPORT DWORD WINAPI mmGetCurrentTask() { return GetFunction<&mmGetCurrentTask>(__func__)(); }

EXPORT void WINAPI mmTaskBlock(DWORD h) { return GetFunction<&mmTaskBlock>(__func__)(h); }

EXPORT UINT WINAPI mmTaskCreate(FARPROC pProc, HANDLE* pHandle, DWORD_PTR dwInst)
  { return GetFunction<&mmTaskCreate>(__func__)(pProc, pHandle, dwInst); }

EXPORT BOOL WINAPI mmTaskSignal(DWORD h) { return GetFunction<&mmTaskSignal>(__func__)(h); }

EXPORT DWORD WINAPI mmTaskYield() { return GetFunction<&mmTaskYield>(__func__)(); }

EXPORT MMRESULT WINAPI mmioAdvance(HMMIO hmmio, LPMMIOINFO pmmioinfo, UINT fuAdvance)
  { return GetFunction<&mmioAdvance>(__func__)(hmmio, pmmioinfo, fuAdvance); }

EXPORT MMRESULT WINAPI mmioAscend(HMMIO hmmio, LPMMCKINFO pmmcki, UINT fuAscend)
  { return GetFunction<&mmioAscend>(__func__)(hmmio, pmmcki, fuAscend); }

EXPORT MMRESULT WINAPI mmioClose(HMMIO hmmio, UINT fuClose)
  { return GetFunction<&mmioClose>(__func__)(hmmio, fuClose); }

EXPORT MMRESULT WINAPI mmioCreateChunk(HMMIO hmmio, LPMMCKINFO pmmcki, UINT fuCreate)
  { return GetFunction<&mmioCreateChunk>(__func__)(hmmio, pmmcki, fuCreate); }

EXPORT MMRESULT WINAPI mmioDescend(HMMIO hmmio, LPMMCKINFO pmmcki, LPCMMCKINFO pmmckiParent, UINT fuDescend)
  { return GetFunction<&mmioDescend>(__func__)(hmmio, pmmcki, pmmckiParent, fuDescend); }

EXPORT MMRESULT WINAPI mmioFlush(HMMIO hmmio, UINT fuFlush)
  { return GetFunction<&mmioFlush>(__func__)(hmmio, fuFlush); }

EXPORT MMRESULT WINAPI mmioGetInfo(HMMIO hmmio, LPMMIOINFO pmmioinfo, UINT fuInfo)
  { return GetFunction<&mmioGetInfo>(__func__)(hmmio, pmmioinfo, fuInfo); }

EXPORT LPMMIOPROC WINAPI mmioInstallIOProcA(FOURCC fccIOProc, LPMMIOPROC pIOProc, DWORD dwFlags)
  { return GetFunction<&mmioInstallIOProcA>(__func__)(fccIOProc, pIOProc, dwFlags); }

EXPORT LPMMIOPROC WINAPI mmioInstallIOProcW(FOURCC fccIOProc, LPMMIOPROC pIOProc, DWORD dwFlags)
  { return GetFunction<&mmioInstallIOProcW>(__func__)(fccIOProc, pIOProc, dwFlags); }

EXPORT HMMIO WINAPI mmioOpenA(LPSTR pszFileName, LPMMIOINFO pmmioinfo, DWORD fdwOpen)
  { return GetFunction<&mmioOpenA>(__func__)(pszFileName, pmmioinfo, fdwOpen); }

EXPORT HMMIO WINAPI mmioOpenW(LPWSTR pszFileName, LPMMIOINFO pmmioinfo, DWORD fdwOpen)
  { return GetFunction<&mmioOpenW>(__func__)(pszFileName, pmmioinfo, fdwOpen); }

EXPORT LONG WINAPI mmioRead(HMMIO hmmio, HPSTR pch, LONG cch)
  { return GetFunction<&mmioRead>(__func__)(hmmio, pch, cch); }

EXPORT MMRESULT WINAPI mmioRenameA(
  LPCSTR pszFileName, LPCSTR pszNewFileName, LPCMMIOINFO pmmioinfo, DWORD fdwRename)
{
  return GetFunction<&mmioRenameA>(__func__)(pszFileName, pszNewFileName, pmmioinfo, fdwRename);
}

EXPORT MMRESULT WINAPI mmioRenameW(
  LPCWSTR pszFileName, LPCWSTR pszNewFileName, LPCMMIOINFO pmmioinfo, DWORD fdwRename)
{
  return GetFunction<&mmioRenameW>(__func__)(pszFileName, pszNewFileName, pmmioinfo, fdwRename);
}

EXPORT LONG WINAPI mmioSeek(HMMIO hmmio, LONG lOffset, int iOrigin)
  { return GetFunction<&mmioSeek>(__func__)(hmmio, lOffset, iOrigin); }

EXPORT LRESULT WINAPI mmioSendMessage(HMMIO hmmio, UINT uMsg, LPARAM lParam1, LPARAM lParam2)
  { return GetFunction<&mmioSendMessage>(__func__)(hmmio, uMsg, lParam1, lParam2); }

EXPORT MMRESULT WINAPI mmioSetBuffer(HMMIO hmmio, LPSTR pchBuffer, LONG cchBuffer, UINT fuBuffer)
  { return GetFunction<&mmioSetBuffer>(__func__)(hmmio, pchBuffer, cchBuffer, fuBuffer); }

EXPORT MMRESULT WINAPI mmioSetInfo(HMMIO hmmio, LPCMMIOINFO pmmioinfo, UINT fuInfo)
  { return GetFunction<&mmioSetInfo>(__func__)(hmmio, pmmioinfo, fuInfo); }

EXPORT FOURCC WINAPI mmioStringToFOURCCA(LPCSTR sz, UINT uFlags)
  { return GetFunction<&mmioStringToFOURCCA>(__func__)(sz, uFlags); }

EXPORT FOURCC WINAPI mmioStringToFOURCCW(LPCWSTR sz, UINT uFlags)
  { return GetFunction<&mmioStringToFOURCCW>(__func__)(sz, uFlags); }

EXPORT LONG WINAPI mmioWrite(HMMIO hmmio, const char* pch, LONG cch)
  { return GetFunction<&mmioWrite>(__func__)(hmmio, pch, cch); }

EXPORT DWORD WINAPI mmsystemGetVersion() { return GetFunction<&mmsystemGetVersion>(__func__)(); }

EXPORT int WINAPI mod32Message(int a, UINT uMsg, HMIDIOUT hmo, DWORD dwMsg, DWORD_PTR dw2)
  { return GetFunction<&mod32Message>(__func__)(a, uMsg, hmo, dwMsg, dw2); }

EXPORT int WINAPI mxd32Message(HMIXER a, UINT uMsg, HMIXER hmx, int b, UINT uMxId)
  { return GetFunction<&mxd32Message>(__func__)(a, uMsg, hmx, b, uMxId); }

EXPORT BOOL WINAPI sndPlaySoundA(LPCSTR pszSound, UINT fuSound)
  { return GetFunction<&sndPlaySoundA>(__func__)(pszSound, fuSound); }

EXPORT BOOL WINAPI sndPlaySoundW(LPCWSTR pszSound, UINT fuSound)
  { return GetFunction<&sndPlaySoundW>(__func__)(pszSound, fuSound); }

EXPORT int WINAPI tid32Message(int a, int b, int c, UINT uPeriod, int d)
  { return GetFunction<&tid32Message>(__func__)(a, b, c, uPeriod, d); }

EXPORT MMRESULT WINAPI timeBeginPeriod(UINT uPeriod) { return GetFunction<&timeBeginPeriod>(__func__)(uPeriod); }

EXPORT MMRESULT WINAPI timeEndPeriod(UINT uPeriod) { return GetFunction<&timeEndPeriod>(__func__)(uPeriod); }

EXPORT MMRESULT WINAPI timeGetDevCaps(LPTIMECAPS ptc, UINT cbtc)
  { return GetFunction<&timeGetDevCaps>(__func__)(ptc, cbtc); }

EXPORT MMRESULT WINAPI timeGetSystemTime(LPMMTIME pmmt, UINT cbmmt)
  { return GetFunction<&timeGetSystemTime>(__func__)(pmmt, cbmmt); }

EXPORT DWORD WINAPI timeGetTime() { return GetFunction<&timeGetTime>(__func__)(); }

EXPORT MMRESULT WINAPI timeKillEvent(UINT uTimerID) { return GetFunction<&timeKillEvent>(__func__)(uTimerID); }

EXPORT MMRESULT WINAPI timeSetEvent(
  UINT uDelay, UINT uResolution, LPTIMECALLBACK fptc, DWORD_PTR dwUser, UINT fuEvent)
{
  return GetFunction<&timeSetEvent>(__func__)(uDelay, uResolution, fptc, dwUser, fuEvent);
}

EXPORT MMRESULT WINAPI waveInAddBuffer(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
  { return GetFunction<&waveInAddBuffer>(__func__)(hwi, pwh, cbwh); }

EXPORT MMRESULT WINAPI waveInClose(HWAVEIN hwi) { return GetFunction<&waveInClose>(__func__)(hwi); }

EXPORT MMRESULT WINAPI waveInGetDevCapsA(UINT_PTR uDeviceID, LPWAVEINCAPSA pwic, UINT cbwic)
  { return GetFunction<&waveInGetDevCapsA>(__func__)(uDeviceID, pwic, cbwic); }

EXPORT MMRESULT WINAPI waveInGetDevCapsW(UINT_PTR uDeviceID, LPWAVEINCAPSW pwic, UINT cbwic)
  { return GetFunction<&waveInGetDevCapsW>(__func__)(uDeviceID, pwic, cbwic); }

EXPORT MMRESULT WINAPI waveInGetErrorTextA(MMRESULT mmrError, LPSTR pszText, UINT cchText)
  { return GetFunction<&waveInGetErrorTextA>(__func__)(mmrError, pszText, cchText); }

EXPORT MMRESULT WINAPI waveInGetErrorTextW(MMRESULT mmrError, LPWSTR pszText, UINT cchText)
  { return GetFunction<&waveInGetErrorTextW>(__func__)(mmrError, pszText, cchText); }

EXPORT MMRESULT WINAPI waveInGetID(HWAVEIN hwi, LPUINT puDeviceID)
  { return GetFunction<&waveInGetID>(__func__)(hwi, puDeviceID); }

EXPORT UINT WINAPI waveInGetNumDevs() { return GetFunction<&waveInGetNumDevs>(__func__)(); }

EXPORT MMRESULT WINAPI waveInGetPosition(HWAVEIN hwi, LPMMTIME pmmt, UINT cbmmt)
  { return GetFunction<&waveInGetPosition>(__func__)(hwi, pmmt, cbmmt); }

EXPORT MMRESULT WINAPI waveInMessage(HWAVEIN hwi, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2)
  { return GetFunction<&waveInMessage>(__func__)(hwi, uMsg, dw1, dw2); }

EXPORT MMRESULT WINAPI waveInOpen(
  LPHWAVEIN phwi, UINT uDeviceID, LPCWAVEFORMATEX pwfx, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen)
{
  return GetFunction<&waveInOpen>(__func__)(phwi, uDeviceID, pwfx, dwCallback, dwInstance, fdwOpen);
}

EXPORT MMRESULT WINAPI waveInPrepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
  { return GetFunction<&waveInPrepareHeader>(__func__)(hwi, pwh, cbwh); }

EXPORT MMRESULT WINAPI waveInReset(HWAVEIN hwi) { return GetFunction<&waveInReset>(__func__)(hwi); }

EXPORT MMRESULT WINAPI waveInStart(HWAVEIN hwi) { return GetFunction<&waveInStart>(__func__)(hwi); }

EXPORT MMRESULT WINAPI waveInStop(HWAVEIN hwi) { return GetFunction<&waveInStop>(__func__)(hwi); }

EXPORT MMRESULT WINAPI waveInUnprepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
  { return GetFunction<&waveInUnprepareHeader>(__func__)(hwi, pwh, cbwh); }

EXPORT MMRESULT WINAPI waveOutBreakLoop(HWAVEOUT hwo) { return GetFunction<&waveOutBreakLoop>(__func__)(hwo); }

EXPORT MMRESULT WINAPI waveOutClose(HWAVEOUT hwo) { return GetFunction<&waveOutClose>(__func__)(hwo); }

EXPORT MMRESULT WINAPI waveOutGetDevCapsA(UINT_PTR uDeviceID, LPWAVEOUTCAPSA pwoc, UINT cbwoc)
  { return GetFunction<&waveOutGetDevCapsA>(__func__)(uDeviceID, pwoc, cbwoc); }

EXPORT MMRESULT WINAPI waveOutGetDevCapsW(UINT_PTR uDeviceID, LPWAVEOUTCAPSW pwoc, UINT cbwoc)
  { return GetFunction<&waveOutGetDevCapsW>(__func__)(uDeviceID, pwoc, cbwoc); }

EXPORT MMRESULT WINAPI waveOutGetErrorTextA(MMRESULT mmrError, LPSTR pszText, UINT cchText)
  { return GetFunction<&waveOutGetErrorTextA>(__func__)(mmrError, pszText, cchText); }

EXPORT MMRESULT WINAPI waveOutGetErrorTextW(MMRESULT mmrError, LPWSTR pszText, UINT cchText)
  { return GetFunction<&waveOutGetErrorTextW>(__func__)(mmrError, pszText, cchText); }

EXPORT MMRESULT WINAPI waveOutGetID(HWAVEOUT hwo, LPUINT puDeviceID)
  { return GetFunction<&waveOutGetID>(__func__)(hwo, puDeviceID); }

EXPORT UINT WINAPI waveOutGetNumDevs() { return GetFunction<&waveOutGetNumDevs>(__func__)(); }

EXPORT MMRESULT WINAPI waveOutGetPitch(HWAVEOUT hwo, LPDWORD pdwPitch)
  { return GetFunction<&waveOutGetPitch>(__func__)(hwo, pdwPitch); }

EXPORT MMRESULT WINAPI waveOutGetPlaybackRate(HWAVEOUT hwo, LPDWORD pdwRate)
  { return GetFunction<&waveOutGetPlaybackRate>(__func__)(hwo, pdwRate); }

EXPORT MMRESULT WINAPI waveOutGetPosition(HWAVEOUT hwo, LPMMTIME pmmt, UINT cbmmt)
  { return GetFunction<&waveOutGetPosition>(__func__)(hwo, pmmt, cbmmt); }

EXPORT MMRESULT WINAPI waveOutGetVolume(HWAVEOUT hwo, LPDWORD pdwVolume)
  { return GetFunction<&waveOutGetVolume>(__func__)(hwo, pdwVolume); }

EXPORT MMRESULT WINAPI waveOutMessage(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2)
  { return GetFunction<&waveOutMessage>(__func__)(hwo, uMsg, dw1, dw2); }

EXPORT MMRESULT WINAPI waveOutOpen(
  LPHWAVEOUT phwo, UINT uDeviceID, LPCWAVEFORMATEX pwfx, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen)
{
  return GetFunction<&waveOutOpen>(__func__)(phwo, uDeviceID, pwfx, dwCallback, dwInstance, fdwOpen);
}

EXPORT MMRESULT WINAPI waveOutPause(HWAVEOUT hwo) { return GetFunction<&waveOutPause>(__func__)(hwo); }

EXPORT MMRESULT WINAPI waveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
  { return GetFunction<&waveOutPrepareHeader>(__func__)(hwo, pwh, cbwh); }

EXPORT MMRESULT WINAPI waveOutReset(HWAVEOUT hwo) { return GetFunction<&waveOutReset>(__func__)(hwo); }

EXPORT MMRESULT WINAPI waveOutRestart(HWAVEOUT hwo) { return GetFunction<&waveOutRestart>(__func__)(hwo); }

EXPORT MMRESULT WINAPI waveOutSetPitch(HWAVEOUT hwo, DWORD dwPitch)
  { return GetFunction<&waveOutSetPitch>(__func__)(hwo, dwPitch); }

EXPORT MMRESULT WINAPI waveOutSetPlaybackRate(HWAVEOUT hwo, DWORD dwRate)
  { return GetFunction<&waveOutSetPlaybackRate>(__func__)(hwo, dwRate); }

EXPORT MMRESULT WINAPI waveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume)
  { return GetFunction<&waveOutSetVolume>(__func__)(hwo, dwVolume); }

EXPORT MMRESULT WINAPI waveOutUnprepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
  { return GetFunction<&waveOutUnprepareHeader>(__func__)(hwo, pwh, cbwh); }

EXPORT MMRESULT WINAPI waveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
  { return GetFunction<&waveOutWrite>(__func__)(hwo, pwh, cbwh); }

EXPORT int WINAPI wid32Message(UINT_PTR uDeviceID, UINT uMsg, HWAVEIN hwi, DWORD_PTR dw1, DWORD_PTR fdwOpen)
  { return GetFunction<&wid32Message>(__func__)(uDeviceID, uMsg, hwi, dw1, fdwOpen); }

EXPORT int WINAPI wod32Message(int a, UINT uMsg, HWAVEOUT hwo, DWORD dwVolume, int b)
  { return GetFunction<&wod32Message>(__func__)(a, uMsg, hwo, dwVolume, b); }

} // "C"
