#pragma once
#include "../DirectXHook/D3D10Hook.h"
#include "../imgui/imgui.h"
#include "../imgui/examples/imgui_impl_dx10.h"
#include "../imgui/examples/imgui_impl_win32.h"

void UpdateCheat();
bool InitHookDX(HWND);
struct AddressDll
{
	unsigned __int64 CryGame;
	unsigned __int64 CryAction;
	unsigned __int64 CrySystem;
	struct CryRender
	{
		unsigned __int64 DX9;
		unsigned __int64 DX10;
	}Render;
	unsigned __int64 pCPlayer;
	unsigned __int64 pCModeCostum;
};