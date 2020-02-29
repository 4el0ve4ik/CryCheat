#include "proxyCryGame.h"
#include "CryGame_dll.hpp"

CreateEditorGame_t CreateEditorGame_p;
CreateGame_t CreateGame_p;
CreateGameStartup_t CreateGameStartup_p;
CryModuleGetMemoryInfo_t CryModuleGetMemoryInfo_p;

EXPORT int64_t  CreateEditorGame()
{
	return CreateEditorGame_p();
}

EXPORT int64_t  CreateGame(int64_t arg)
{
	return CreateGame_p(arg);
}

EXPORT void*  CreateGameStartup(void)
{
	return CreateGameStartup_p();
}

EXPORT int64_t  CryModuleGetMemoryInfo(int64_t arg)
{
	return CryModuleGetMemoryInfo_p(arg);
}

HMEMORYMODULE LoadCryGame()
{
	HMEMORYMODULE hmodule = MemoryLoadLibrary(CryGame_dll::data, CryGame_dll::size);
	CreateEditorGame_p = (CreateEditorGame_t)MemoryGetProcAddress(hmodule, "CreateEditorGame");
	CreateGame_p = (CreateGame_t)MemoryGetProcAddress(hmodule, "CreateGame");
	CreateGameStartup_p = (CreateGameStartup_t)MemoryGetProcAddress(hmodule, "CreateGameStartup");
	CryModuleGetMemoryInfo_p = (CryModuleGetMemoryInfo_t)MemoryGetProcAddress(hmodule, "CryModuleGetMemoryInfo");
	return hmodule;
}

