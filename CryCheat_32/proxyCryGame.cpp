#include "proxyCryGame.h"

CreateEditorGame_t CreateEditorGame_p;
CreateGame_t CreateGame_p;
CreateGameStartup_t CreateGameStartup_p;
CryModuleGetMemoryInfo_t CryModuleGetMemoryInfo_p;

EXPORT int  CreateEditorGame(void)
{
	return CreateEditorGame_p();
}

EXPORT int  CreateGame(int arg)
{
	return CreateGame_p(arg);
}

EXPORT void*  CreateGameStartup(void)
{
	return CreateGameStartup_p();
}

EXPORT int  CryModuleGetMemoryInfo(int arg)
{
	return CryModuleGetMemoryInfo_p(arg);
}

HMODULE LoadCryGame()
{
	HMODULE hmodule = LoadLibrary("CryGameHooked.dll");
	CreateEditorGame_p = (CreateEditorGame_t)GetProcAddress(hmodule, "CreateEditorGame");
	CreateGame_p = (CreateGame_t)GetProcAddress(hmodule, "CreateGame");
	CreateGameStartup_p = (CreateGameStartup_t)GetProcAddress(hmodule, "CreateGameStartup");
	CryModuleGetMemoryInfo_p = (CryModuleGetMemoryInfo_t)GetProcAddress(hmodule, "CryModuleGetMemoryInfo");
	return hmodule;
}