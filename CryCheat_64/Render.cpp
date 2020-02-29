#include "Render.h"
#include <string>
WNDPROC oldWNDPROC = nullptr;
bool gShowDemo = false;
ID3D10Device * pDevice = NULL;
bool gInitialized = false;
bool CheatMenuActive = false;
bool CheckPressed = false;
extern AddressDll AddressDLL;
extern 	D3D10HK::IDXGISwapChain__Present Present;
extern int64_t __stdcall AddConsoleMessage(const std::string& message);

struct _CheatActivation {
	bool GoodMode = false;
	bool UnlimitedEnergy = false;
	bool NoRecoil = false;
	bool UnlimitedAmmo = false;
	bool UnlimitedNightVision = false;
	bool NoTimeReloadTB = false;
	bool NoOwerheat = false;
} CActivation;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI NWNDPROC(HWND hWND, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWND, Msg, wParam, lParam))
		return true;

	if (Msg == WM_KEYUP)
	{
		if (wParam == 72) gShowDemo ^= true;
		if (wParam == 45) {
			CheatMenuActive ^= true; 
			ShowCursor(CheatMenuActive);
		}
	}
	
	return CallWindowProc(oldWNDPROC, hWND, Msg, wParam, lParam);
}



void CheckBox(const std::string& label, bool* boolean) {
	if (ImGui::Checkbox(label.c_str(), boolean)) {
		AddConsoleMessage(label + ": " + (*boolean ? "Enable" : "Disable"));
		CheckPressed = true;
	}
}

void Menu(bool* p_open) {
	if (!ImGui::Begin("CryCheat", p_open)) {
		ImGui::End();
		return;
	}

	CheckBox("No Recoil", &CActivation.NoRecoil);
	CheckBox("Unlimited Ammo", &CActivation.UnlimitedAmmo);
	CheckBox("No Owerheat", &CActivation.NoOwerheat);
	CheckBox("No Reload Poison Dart", &CActivation.NoTimeReloadTB);
	CheckBox("Unlimited Night Vision", &CActivation.UnlimitedNightVision);
}

HRESULT HookedPresentD3D10(IDXGISwapChain * pSwap, UINT SyncInterval, UINT Flags)
{
	if (!gInitialized)
	{
		DXGI_SWAP_CHAIN_DESC sd;
		pSwap->GetDesc(&sd);
		HWND ghWnd = nullptr;
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NoMouseCursorChange;
		io.IniFilename = "";
		ImGui::StyleColorsDark();
		ghWnd = sd.OutputWindow;
		pSwap->GetDevice(__uuidof(ID3D10Device), (LPVOID*)&pDevice);
		ImGui_ImplWin32_Init(ghWnd);
		ImGui_ImplDX10_Init(pDevice);
		io.ImeWindowHandle = ghWnd;

		oldWNDPROC = (WNDPROC)SetWindowLongPtr(ghWnd, -4, reinterpret_cast<LONG_PTR>(NWNDPROC));
		AddConsoleMessage("Present initialized");

		gInitialized = true;
	} else {

		ImGui_ImplWin32_NewFrame();
		ImGui_ImplDX10_NewFrame();
		ImGui::NewFrame();

		if (CheatMenuActive) {
			Menu(&CheatMenuActive);
		}

		if (gShowDemo) {
			ImGui::ShowDemoWindow(&gShowDemo);
		}

		ImGui::EndFrame();

		ImGui::Render();

		ImGui_ImplDX10_RenderDrawData(ImGui::GetDrawData());
	}
	return Present(pSwap, SyncInterval, Flags);
}

void UpdateCheat() {
	if (CheckPressed)
	{	
		CheckPressed = false;
	}
}