#pragma once
#include <Windows.h>
#define EXPORT extern "C" __declspec(dllexport)

HMODULE LoadCryGame();

typedef long long int64_t;
typedef int64_t(*CreateEditorGame_t)();
typedef int64_t(__fastcall*CreateGame_t)(int64_t);
typedef void*(*CreateGameStartup_t)(void);
typedef int64_t(__fastcall*CryModuleGetMemoryInfo_t)(int64_t);

