#pragma once
#include "../DirectXHook/D3D10Hook.h"
#include "../ImGUI/imgui.h"
#include "../ImGUI/imgui_impl_dx10.h"
#include "../ImGUI/imgui_impl_win32.h"

HRESULT HookedPresentD3D10(IDXGISwapChain * pSwap, UINT SyncInterval, UINT Flags);
void UpdateCheat();
struct AddressDll
{
	unsigned long long CryGame;
	unsigned long long CryAction;
	unsigned long long CrySystem;
	struct CryRender
	{
		unsigned long long DX9;
		unsigned long long DX10;
	}Render;
};