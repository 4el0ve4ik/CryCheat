#pragma once
#include "DirectXHook\D3D10Hook.h"
#include "imgui.h"
#include "examples\imgui_impl_dx10.h"
#include "examples\imgui_impl_win32.h"

struct AddressDll
{
	unsigned long CryGame;
	unsigned long CryAction;
	unsigned long CrySystem;
	struct CryRender
	{
		unsigned long DX9;
		unsigned long DX10;
	}Render;
	unsigned long pCPlayer;
	unsigned long pCModeCostum;
};

bool InitHookDX(HWND);