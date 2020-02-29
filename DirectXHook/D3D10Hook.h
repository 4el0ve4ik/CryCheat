#pragma once
#include <d3d10_1.h>
#include <d3d10.h>
#include <dxgi.h>
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Lib/x64/d3d10.lib")
#pragma comment(lib, "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Lib/x64/d3dx10.lib")

namespace D3D10HK
{
	typedef HRESULT(__fastcall * IDXGISwapChain__Present)(IDXGISwapChain * pSwapChain, UINT SyncInterval, UINT Flags);
	bool InitiateHook(HWND hWnd, void* hkfunc, IDXGISwapChain__Present *oFunc);
}

