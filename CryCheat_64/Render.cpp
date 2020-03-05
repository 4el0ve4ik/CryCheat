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
const std::string oByte[9]{ "\x45\x89\x41\x28", "\xF3\x0F\x11\x73\x48", "\x89\x45\x4C", "\xF3\x0F\x11\x4F\x60", "\x89\x79\x28", 
"\xF3\x0F\x11\x83\x40\x02\x00\x00", "\xF3\x0F\x11\x45\x30", "\xF3\x0F\x11\x8B\x18\x06\x00\x00", "\xF3\x0F\x11\x83\x18\x06\x00\x00" };
/////////////////////////////////////functions of CryGame///////////////////////////////////
const unsigned __int64 MOVEAMMOINCLIP = 0x18773A;
const unsigned __int64 MOVEENERGY[2]{0x3C954, 0x411F0};
const unsigned __int64 MOVEHEALTH = 0xBAD5;
const unsigned __int64 DECREASENIGHTVISION = 0x21FFFD;
const unsigned __int64 INCREASETIMERPOISONDART = 0x1CAF44;
const unsigned __int64 OWERHEAT[2]{ 0x1BE724, 0x1BE740 };
////////////////////////////////////of CryAction///////////////////////////////////////////
const unsigned __int64 MOVEAMMO = 0x26446;

enum ioBYTE {
	bMoveAmmoInClip = 0,
	bMoveEnergy1,
	bMoveEnergy2,
	bMoveHealth,
	bMoveAmmo,
	bDecreaseNightVision,
	bIncreaseTimerPoisonDart,
	bOwerheat1,
	bOwerheat2
};

struct _CheatActivation {
	bool GoodMode = false;
	bool UnlimitedEnergy = false;
	bool UnlimitedAmmo = false;
	bool UnlimitedNightVision = false;
	bool NoTimeReloadTB = false;
	bool NoOwerheat = false;
	bool SpeedHack = false;
	float speed = 100.0;
	bool NoReloadingWeapon = false;
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
unsigned __int64 pool = 0;
void Menu(bool* p_open) {
	if (!ImGui::Begin("CryCheat", p_open,ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::End();
		return;
	}
	ImGui::Text("%x", pool);
	ImGui::Text("%x", AddressDLL.pCPlayer);
	CheckBox("GoodMode", &CActivation.GoodMode);
	CheckBox("No Reloading Weapon", &CActivation.NoReloadingWeapon);
	CheckBox("Unlimited Ammo", &CActivation.UnlimitedAmmo);
	CheckBox("No Owerheat", &CActivation.NoOwerheat);
	CheckBox("No Reload Poison Dart", &CActivation.NoTimeReloadTB);
	CheckBox("Unlimited Night Vision", &CActivation.UnlimitedNightVision);
	CheckBox("Unlimited Energy", &CActivation.UnlimitedEnergy);
	CheckBox("Speed Hack", &CActivation.SpeedHack);
	if (CActivation.SpeedHack)
		ImGui::SliderFloat("##Speed", &CActivation.speed, 1.0F, 1000.0F, "%.0f");
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

		DWORD oldProt = 0;

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

void nop(void* address, size_t size) {
	DWORD oldProt = 0;
	VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &oldProt);
	memset(address, 0x90, size);
	VirtualProtect(address, size, oldProt, NULL);
}

void restoreoriginalbyte(void* address, const void* value, size_t size) {
	DWORD oldProt = 0;
	VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &oldProt);
	memcpy(address, value, size);
	VirtualProtect(address, size, oldProt, NULL);
}

void UpdateCheat() {
	if (AddressDLL.pCPlayer == 0) {
		unsigned __int64 pCPlayer = *(unsigned __int64*)(AddressDLL.CryAction + 0x465C00);
		if (pCPlayer) {
			pCPlayer = *(unsigned __int64*)(pCPlayer + 0xF0);
			if (pCPlayer)
				pCPlayer = *(unsigned __int64*)(pCPlayer + 0x28);
			pool = pCPlayer;
			if (pCPlayer)
				pCPlayer = *(unsigned __int64*)(pCPlayer);
			if (pCPlayer)
				pCPlayer = *(unsigned __int64*)(pCPlayer + 0xD0);
			if (pCPlayer)
				pCPlayer = *(unsigned __int64*)(pCPlayer + 0x68);
			if (pCPlayer)
				AddressDLL.pCPlayer = *(unsigned __int64*)(pCPlayer + 0x70);
		}
	}
	
	if (CheckPressed)
	{
		if (CActivation.NoReloadingWeapon) {
			nop((void*)(AddressDLL.CryGame + MOVEAMMOINCLIP), 4);
		}
		else {
			restoreoriginalbyte((void*)(AddressDLL.CryGame + MOVEAMMOINCLIP), (PBYTE)oByte[ioBYTE::bMoveAmmoInClip].c_str(), 4);
		}

		if (CActivation.UnlimitedAmmo) {
			nop((void*)(AddressDLL.CryAction + MOVEAMMO), 3);
		}
		else {
			restoreoriginalbyte((void*)(AddressDLL.CryAction + MOVEAMMO), (PBYTE)oByte[ioBYTE::bMoveAmmo].c_str(), 3);
		}

		if (CActivation.GoodMode) {
			nop((void*)(AddressDLL.CryGame + MOVEHEALTH), 5);
		}
		else {
			restoreoriginalbyte((void*)(AddressDLL.CryGame + MOVEHEALTH), (PBYTE)oByte[ioBYTE::bMoveHealth].c_str(), 5);
		}

		if (CActivation.UnlimitedNightVision) {
			nop((void*)(AddressDLL.CryGame + DECREASENIGHTVISION), 8);
		}
		else {
			restoreoriginalbyte((void*)(AddressDLL.CryGame + DECREASENIGHTVISION), (PBYTE)oByte[ioBYTE::bDecreaseNightVision].c_str(), 8);
		}

		if (CActivation.UnlimitedEnergy) {
			nop((void*)(AddressDLL.CryGame + MOVEENERGY[0]), 5);
			nop((void*)(AddressDLL.CryGame + MOVEENERGY[1]), 3);
		}
		else {
			restoreoriginalbyte((void*)(AddressDLL.CryGame + MOVEENERGY[0]), (PBYTE)oByte[ioBYTE::bMoveEnergy1].c_str(), 5);
			restoreoriginalbyte((void*)(AddressDLL.CryGame + MOVEENERGY[1]), (PBYTE)oByte[ioBYTE::bMoveEnergy2].c_str(), 3);
		}

		if (CActivation.NoTimeReloadTB) {
			nop((void*)(AddressDLL.CryGame + INCREASETIMERPOISONDART), 5);
		}
		else { restoreoriginalbyte((void*)(AddressDLL.CryGame + INCREASETIMERPOISONDART), (PBYTE)oByte[ioBYTE::bIncreaseTimerPoisonDart].c_str(), 5); }

		if (CActivation.NoOwerheat) {
			nop((void*)(AddressDLL.CryGame + OWERHEAT[0]), 8);
			nop((void*)(AddressDLL.CryGame + OWERHEAT[1]), 8);
		}
		else {
			restoreoriginalbyte((void*)(AddressDLL.CryGame + OWERHEAT[0]), (PBYTE)oByte[ioBYTE::bOwerheat1].c_str(), 8);
			restoreoriginalbyte((void*)(AddressDLL.CryGame + OWERHEAT[1]), (PBYTE)oByte[ioBYTE::bOwerheat2].c_str(), 8);
		}
		CheckPressed = false;
	}
	if (AddressDLL.pCPlayer > 0) {
		if (CActivation.SpeedHack) {
			*reinterpret_cast<float*>(AddressDLL.pCPlayer + 0x90) = CActivation.speed;
			*reinterpret_cast<float*>(AddressDLL.pCPlayer + 0x94) = CActivation.speed;
		}
	}
}

