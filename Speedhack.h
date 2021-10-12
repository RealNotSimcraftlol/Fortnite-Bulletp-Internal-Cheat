#include "SDK.h"
#include "detours.h"

#pragma comment(lib, "detours.lib")

template<class DataType>
class SpeedHack {
	DataType time_offset;
	DataType time_last_update;

	double speed_;

public:
	SpeedHack(DataType currentRealTime, double initialSpeed) {
		time_offset = currentRealTime;
		time_last_update = currentRealTime;

		speed_ = initialSpeed;
	}

	// TODO: put lock around for thread safety
	void setSpeed(DataType currentRealTime, double speed) {
		time_offset = getCurrentTime(currentRealTime);
		time_last_update = currentRealTime;

		speed_ = speed;
	}

	// TODO: put lock around for thread safety
	DataType getCurrentTime(DataType currentRealTime) {
		DataType difference = currentRealTime - time_last_update;

		return (DataType)(speed_ * difference) + time_offset;
	}
};


// function signature typedefs
typedef DWORD(WINAPI* GetTickCountType)(void);
typedef ULONGLONG(WINAPI* GetTickCount64Type)(void);

typedef BOOL(WINAPI* QueryPerformanceCounterType)(LARGE_INTEGER* lpPerformanceCount);

// globals
GetTickCountType   g_GetTickCountOriginal;
GetTickCount64Type g_GetTickCount64Original;
GetTickCountType   g_TimeGetTimeOriginal;    // Same function signature as GetTickCount

QueryPerformanceCounterType g_QueryPerformanceCounterOriginal;


const double kInitialSpeed = 1.0; // initial speed hack speed

//                                  (initialTime,      initialSpeed)
SpeedHack<DWORD>     g_speedHack(GetTickCount(), kInitialSpeed);
SpeedHack<ULONGLONG> g_speedHackULL(GetTickCount64(), kInitialSpeed);
SpeedHack<LONGLONG>  g_speedHackLL(0, kInitialSpeed); // Gets set properly in DllMain

// function prototypes

DWORD     WINAPI GetTickCountHacked(void);
ULONGLONG WINAPI GetTickCount64Hacked(void);

BOOL      WINAPI QueryPerformanceCounterHacked(LARGE_INTEGER* lpPerformanceCount);

DWORD     WINAPI KeysThread(LPVOID lpThreadParameter);

// functions

void MainGay()
{
	// TODO: split up this function for readability.

	HMODULE kernel32 = GetModuleHandleA(xorstr("Kernel32.dll"));
	HMODULE winmm = GetModuleHandleA(xorstr("Winmm.dll"));

	// TODO: check if the modules are even loaded.

	// Get all the original addresses of target functions
	g_GetTickCountOriginal = (GetTickCountType)GetProcAddress(kernel32, xorstr("GetTickCount"));
	g_GetTickCount64Original = (GetTickCount64Type)GetProcAddress(kernel32, xorstr("GetTickCount64"));

	g_TimeGetTimeOriginal = (GetTickCountType)GetProcAddress(winmm, xorstr("timeGetTime"));

	g_QueryPerformanceCounterOriginal = (QueryPerformanceCounterType)GetProcAddress(kernel32, xorstr("QueryPerformanceCounter"));

	// Setup the speed hack object for the Performance Counter
	LARGE_INTEGER performanceCounter;
	g_QueryPerformanceCounterOriginal(&performanceCounter);

	g_speedHackLL = SpeedHack<LONGLONG>(performanceCounter.QuadPart, kInitialSpeed);

	// Detour functions
	DetourTransactionBegin();

	DetourAttach((PVOID*)&g_GetTickCountOriginal, (PVOID)GetTickCountHacked);
	DetourAttach((PVOID*)&g_GetTickCount64Original, (PVOID)GetTickCount64Hacked);

	// Detour timeGetTime to the hacked GetTickCount (same signature)
	DetourAttach((PVOID*)&g_TimeGetTimeOriginal, (PVOID)GetTickCountHacked);

	DetourAttach((PVOID*)&g_QueryPerformanceCounterOriginal, (PVOID)QueryPerformanceCounterHacked);

	DetourTransactionCommit();
}

void setAllToSpeed(double speed) {
	g_speedHack.setSpeed(g_GetTickCountOriginal(), speed);

	g_speedHackULL.setSpeed(g_GetTickCount64Original(), speed);

	LARGE_INTEGER performanceCounter;
	g_QueryPerformanceCounterOriginal(&performanceCounter);

	g_speedHackLL.setSpeed(performanceCounter.QuadPart, speed);
}

DWORD WINAPI GetTickCountHacked(void) {
	return g_speedHack.getCurrentTime(g_GetTickCountOriginal());
}

ULONGLONG WINAPI GetTickCount64Hacked(void) {
	return g_speedHackULL.getCurrentTime(g_GetTickCount64Original());
}

BOOL WINAPI QueryPerformanceCounterHacked(LARGE_INTEGER* lpPerformanceCount) {
	LARGE_INTEGER performanceCounter;

	BOOL result = g_QueryPerformanceCounterOriginal(&performanceCounter);

	lpPerformanceCount->QuadPart = g_speedHackLL.getCurrentTime(performanceCounter.QuadPart);

	return result;
}