#include "main.h"
extern AddressDll AddressDLL{ 0 };

inline unsigned long GetModuleH(LPCSTR module)
{
	return reinterpret_cast<unsigned long>(GetModuleHandle(module));
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

void Thread(void* hCryGame) {
	while (!Init())
		Sleep(350);
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