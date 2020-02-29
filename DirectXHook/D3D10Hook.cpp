#include "D3D10Hook.h"

namespace D3D10HK {

	ID3D10Device * pDevice = NULL;
	IDXGISwapChain * pSwapChain = NULL;
	bool initializedHook = false;

	bool InitiateHook(HWND hWnd,void* hkfunc, IDXGISwapChain__Present *oFunc)
	{
		if (!initializedHook) {
			D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_10_0;
			DXGI_SWAP_CHAIN_DESC sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.BufferCount = 2;
			sd.BufferDesc.Width = 0;
			sd.BufferDesc.Height = 0;
			sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			sd.BufferDesc.RefreshRate.Numerator = 60;
			sd.BufferDesc.RefreshRate.Denominator = 1;
			sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.OutputWindow = hWnd;
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
			sd.Windowed = TRUE;
			sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

			UINT createDeviceFlags = 0;
			createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
			if (D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, D3D10_SDK_VERSION, &sd, &pSwapChain, &pDevice) != S_OK)
				return false;

			DWORD oldProtection;
			void ** SwapChainVtable = *(void ***)pSwapChain;
			VirtualProtect(&SwapChainVtable[8], 4, PAGE_EXECUTE_READWRITE, &oldProtection);

			*oFunc = (IDXGISwapChain__Present)SwapChainVtable[8];

			SwapChainVtable[8] = hkfunc;
			VirtualProtect(&SwapChainVtable[8], 4, oldProtection, NULL);
			pSwapChain->Release();
			pDevice->Release();
			initializedHook = true;
		}
		return true;
	}
	
}