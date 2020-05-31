#pragma once
#include <Windows.h>
#define EXPORT extern "C" __declspec(dllexport)

HMODULE LoadCryGame();

typedef int(*CreateEditorGame_t)(void);
typedef int(__fastcall*CreateGame_t)(int);
typedef void*(*CreateGameStartup_t)(void);
typedef int(__fastcall*CryModuleGetMemoryInfo_t)(int);