#include "main.h"
#include <fstream>

extern AddressDll AddressDLL{ 0 };

extern int64_t __stdcall AddConsoleMessage(const std::string& message)
{
	SYSTEMTIME today;
	GetLocalTime(&today);
	char systime[255];
	GetTimeFormat(LOCALE_SYSTEM_DEFAULT, TIME_FORCE24HOURFORMAT, &today, "[h:m:s] ", systime, 255);
	std::string msg = std::string("[CryCheat]") + systime + message;
	return ((int64_t(*)(const char*, ...))(0x36501770))(msg.c_str());
}

inline unsigned __int64 GetModuleH(LPCSTR module)
{
	return reinterpret_cast<unsigned __int64>(GetModuleHandle(module));
}

bool Init()
{
	if (!(AddressDLL.CryAction = GetModuleH(CRYSIS_ACTION))) return false;
	if (!(AddressDLL.CrySystem = GetModuleH(CRYSIS_SYSTEM))) return false;
	AddressDLL.Render.DX9 = GetModuleH(CRYSIS_RENDERDX9);
	AddressDLL.Render.DX10 = GetModuleH(CRYSIS_RENDERDX10);
	if (AddressDLL.Render.DX10 == 0 && AddressDLL.Render.DX9 == 0) return false;
	
	HWND ghWnd = GetForegroundWindow();
	if (!ghWnd)	return false;
	if (AddressDLL.Render.DX10) {
		return InitHookDX(ghWnd);
	}
	
	return true;
}

void Thread(void* hCryGame)
{
	AddressDLL.CryGame = (unsigned long long)hCryGame;
	while (!Init()) 
		Sleep(350);
	while (true)
	{
		Sleep(350);
		UpdateCheat();
	}
}

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID)
{
	if (fdwReason == 1)
	{
		HMODULE hCryGame = LoadCryGame();
		_beginthread(Thread, NULL, hCryGame);
	}
	return TRUE;
}