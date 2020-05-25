#include "D3D10Hook.h"

D3D10HOOK::D3D10HOOK(HWND hWnd) {
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
		if (D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, D3D10_SDK_VERSION, &sd, &pSwapChain, &pDevice) == S_OK) {
			SwapChainVtable = *(void***)pSwapChain;

			DeviceVtable = *(void***)pDevice;

			for (unsigned char i = 0; i <= 97; i++)
				oDeviceFunc[i] = 0;
			for (unsigned char i = 0; i <= 17; i++)
				oSwapChainFunc[i] = 0;

			initializedHook = true;
		}
	}
}

D3D10HOOK::~D3D10HOOK() {
	for (unsigned char i = 0; i <= 97; i++) {
		if (oDeviceFunc[i] != 0)
			DeviceVtable[i] = (void*)oDeviceFunc[i];
	}
	for (unsigned char i = 0; i <= 17; i++) {
		if (oSwapChainFunc[i] != 0)
			SwapChainVtable[i] = (void*)oSwapChainFunc[i];
	}
	pDevice->Release();
	pSwapChain->Release();
}

void D3D10HOOK::SetSwapChainHook(void* Func, SwapChainIndex index) {
	if (oSwapChainFunc[index] == 0) {
		DWORD oldProtection;
		VirtualProtect(&SwapChainVtable[index], 4, PAGE_EXECUTE_READWRITE, &oldProtection);
		oSwapChainFunc[index] = (unsigned long long)SwapChainVtable[index];
		SwapChainVtable[index] = Func;
		VirtualProtect(&SwapChainVtable[index], 4, oldProtection, NULL);
	}
}

void D3D10HOOK::SetDeviceHook(void* Func, DeviceIndex index) {
	if (oDeviceFunc[index] == 0) {
		unsigned long oldProt;
		VirtualProtect((void*)&DeviceVtable[index], 4, PAGE_EXECUTE_READWRITE, &oldProt);
		oDeviceFunc[index] = (unsigned long long)DeviceVtable[index];
		DeviceVtable[index] = Func;
		VirtualProtect((void*)&DeviceVtable[index], 4, oldProt, NULL);
	}
}

long long D3D10HOOK::GetOriginDeviceFunc(DeviceIndex index) {
	return oDeviceFunc[index];
}

long long D3D10HOOK::GetOriginSwapChainFunc(SwapChainIndex index) {
	return oSwapChainFunc[index];
}

bool D3D10HOOK::GetHookStatus() {
	return initializedHook;
}