#include "Render.h"
#include <string>
D3D10HOOK* d3d10hk = nullptr;
WNDPROC oldWNDPROC = nullptr;

bool gInitialized = false;
bool CheatMenuActive = false;
bool CheckPressed = false;

typedef HRESULT(__stdcall *oPresent)(IDXGISwapChain * pSwap, UINT SyncInterval, UINT Flags);
oPresent Present;

struct _CheatActivation {
	bool GodMode = false;
	bool UnlimitedEnergy = false;
	bool UnlimitedAmmo = false;
	bool UnlimitedNightVision = false;
	bool NoTimeReloadTB = false;
	bool NoOverheat = false;
	bool SpeedHack = false;
	float speed = 100.0;
	bool NoReloadingWeapon = false;
	bool OneHitKill = false;
} CActivation;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI NWNDPROC(HWND hWND, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWND, Msg, wParam, lParam))
		return true;

	if (Msg == WM_KEYUP)
	{
		if (wParam == 45) {
			CheatMenuActive ^= true;
			ShowCursor(CheatMenuActive);
		}
	}

	return CallWindowProc(oldWNDPROC, hWND, Msg, wParam, lParam);
}
void CheckBox(const std::string& label, bool* boolean) {
	if (ImGui::Checkbox(label.c_str(), boolean)) {
		//AddConsoleMessage(label + ": " + (*boolean ? "Enable" : "Disable"));
		CheckPressed = true;
	}
}
void Menu(bool* p_open) {
	if (!ImGui::Begin("CryCheat", p_open, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::End();
		return;
	}
	CheckBox("GodMode", &CActivation.GodMode);
	CheckBox("No Reloading Weapon", &CActivation.NoReloadingWeapon);
	CheckBox("Unlimited Ammo", &CActivation.UnlimitedAmmo);
	CheckBox("One Hit Kill", &CActivation.OneHitKill);
	CheckBox("No Overheat", &CActivation.NoOverheat);
	CheckBox("No Reload Poison Dart", &CActivation.NoTimeReloadTB);
	CheckBox("Unlimited Night Vision", &CActivation.UnlimitedNightVision);
	CheckBox("Unlimited Energy", &CActivation.UnlimitedEnergy);
	CheckBox("Speed Hack", &CActivation.SpeedHack);
	if (CActivation.SpeedHack)
		ImGui::SliderFloat("##Speed", &CActivation.speed, 1.0F, 1000.0F, "%.0f");
}

HRESULT HookedPresentD3D10(IDXGISwapChain *pSwap, UINT SyncInterval, UINT Flags)
{
	if (!gInitialized)
	{
		ID3D10Device * pDevice = NULL;
		DXGI_SWAP_CHAIN_DESC sd;
		pSwap->GetDesc(&sd);
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NoMouseCursorChange;
		io.IniFilename = "";
		ImGui::StyleColorsDark();
		HWND ghWnd = sd.OutputWindow;
		pSwap->GetDevice(__uuidof(ID3D10Device), (LPVOID*)&pDevice);
		ImGui_ImplWin32_Init(ghWnd);
		ImGui_ImplDX10_Init(pDevice);
		io.ImeWindowHandle = ghWnd;

		oldWNDPROC = (WNDPROC)SetWindowLongPtr(ghWnd, -4, reinterpret_cast<LONG_PTR>(NWNDPROC));
		Present = (oPresent)d3d10hk->GetOriginSwapChainFunc(SwapChainIndex::iPresent);
		gInitialized = true;
	} else {
		if (CheatMenuActive) {
			ImGui_ImplWin32_NewFrame();
			ImGui_ImplDX10_NewFrame();
			ImGui::NewFrame();


			Menu(&CheatMenuActive);


			ImGui::EndFrame();

			ImGui::Render();

			ImGui_ImplDX10_RenderDrawData(ImGui::GetDrawData());
		}
	}
	
return	Present(pSwap, SyncInterval, Flags);
}

bool InitHookDX(HWND hWnd) {
	d3d10hk = new D3D10HOOK(hWnd);
	if (!d3d10hk->GetHookStatus()) return false;

	d3d10hk->SetSwapChainHook(&HookedPresentD3D10, SwapChainIndex::iPresent);
	return true;
}