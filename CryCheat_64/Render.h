#pragma once
#include "../DirectXHook/D3D10Hook.h"
#include "../ImGUI/imgui.h"
#include "../ImGUI/imgui_impl_dx10.h"
#include "../ImGUI/imgui_impl_win32.h"

HRESULT HookedPresentD3D10(IDXGISwapChain * pSwap, UINT SyncInterval, UINT Flags);
void UpdateCheat();
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
};