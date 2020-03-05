#include "main.h"

HWND ghWnd = nullptr;
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

extern 	D3D10HK::IDXGISwapChain__Present Present = nullptr;
bool Init(HMEMORYMODULE hCryGame)
{
	if (!(AddressDLL.CryAction = GetModuleH(CRYSIS_ACTION))) return false;
	if (!(AddressDLL.CrySystem = GetModuleH(CRYSIS_SYSTEM))) return false;
	AddressDLL.Render.DX9 = GetModuleH(CRYSIS_RENDERDX9);
	AddressDLL.Render.DX10 = GetModuleH(CRYSIS_RENDERDX10);
	if (AddressDLL.Render.DX10 == 0 && AddressDLL.Render.DX9 == 0) return false;
	
	ghWnd = GetForegroundWindow();
	if (!ghWnd)	return false;



	if (AddressDLL.Render.DX10) {
		if (D3D10HK::InitiateHook(ghWnd, &HookedPresentD3D10, &Present))
			return true;
	}
	

	return true;
}

void Thread(HMEMORYMODULE hCryGame)
{
	AddressDLL.CryGame = *(DWORD*)((*(DWORD**)&hCryGame) + 2);
	while (!Init(hCryGame)) 
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
		HMEMORYMODULE hCryGame = LoadCryGame();
		_beginthread(Thread, NULL, hCryGame);
	}
	return TRUE;
}

/*TODO:////////////////////////////////////////////////////////////////////////////////////////////
Vehicle goodmod //
no spread
one hit kill 
*/