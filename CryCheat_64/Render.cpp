#include "Render.h"
#include <string>
#include <Windows.h>

D3D10HOOK *d3d10hk = nullptr;
WNDPROC oldWNDPROC = nullptr;

bool gInitialized = false;
bool CheatMenuActive = false;
bool CheckPressed = false;

extern AddressDll AddressDLL;
typedef HRESULT(__stdcall *oPresent)(IDXGISwapChain * pSwap, UINT SyncInterval, UINT Flags);
oPresent Present;

typedef signed __int64(__stdcall *osetHealth)(__int64, int);
osetHealth psetHealth = nullptr;

extern int64_t __stdcall AddConsoleMessage(const std::string& message);
const std::string oByte[9]{ "\x45\x89\x41\x28", "\xF3\x0F\x11\x73\x48", "\x89\x45\x4C",
"\x89\x79\x28","\xF3\x0F\x11\x83\x40\x02\x00\x00", "\xF3\x0F\x11\x45\x30",
"\xF3\x0F\x11\x8B\x18\x06\x00\x00", "\xF3\x0F\x11\x83\x18\x06\x00\x00" };
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
	bMoveAmmo,
	bDecreaseNightVision,
	bIncreaseTimerPoisonDart,
	bOwerheat1,
	bOwerheat2
};

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
		AddConsoleMessage(label + ": " + (*boolean ? "Enable" : "Disable"));
		CheckPressed = true;
	}
}
void Menu(bool* p_open) {
	if (!ImGui::Begin("CryCheat", p_open,ImGuiWindowFlags_AlwaysAutoResize)) {
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

void InjectJmp(__int64 _offset, void* target)
{
	void *pBlock = VirtualAlloc(0, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	unsigned long Protection;
	VirtualProtect((void*)_offset, 5, PAGE_EXECUTE_READWRITE, &Protection);
	__int64 offs = (__int64)pBlock - (_offset + 5);
	*reinterpret_cast<unsigned char*>(_offset) = 0xE9;
	memcpy((LPVOID)(_offset + 1), &offs, 4);
	VirtualProtect((void*)_offset, 5, Protection, 0);
	memcpy(pBlock, "\xFF\x15\x02\x00\x00\x00\xEB\x08", 8);
	memcpy((void*)((__int64)pBlock + 8), &target, sizeof(target));
	memcpy((void*)((__int64)pBlock + 16), "\xFF\x25\x00\x00\x00\x00", 6);
	__int64 retAddr = _offset + 5;
	memcpy((void*)((__int64)pBlock + 22), &retAddr, sizeof(retAddr)); 
}

signed __int64 __fastcall setHealth(__int64 CPlayer, int health) {
	if (*reinterpret_cast<unsigned long long*>(CPlayer + 0x628) == AddressDLL.pCModeCostum) {
		AddressDLL.pCPlayer = CPlayer;
		if (CActivation.GodMode)
			return psetHealth(CPlayer, *reinterpret_cast<int*>(CPlayer + 0x60));
		return psetHealth(CPlayer, health);
	}
	if (CActivation.OneHitKill) return psetHealth(CPlayer, 0);

	return psetHealth(CPlayer, health);
}
//((signed __int64(__stdcall *)(__int64, int))(AddressDLL.CryGame + 0xB9B0))(CPlayer, health);
HRESULT HookedPresentD3D10(IDXGISwapChain *pSwap, UINT SyncInterval, UINT Flags)
{
	if (!gInitialized)
	{
		psetHealth = (osetHealth)(AddressDLL.CryGame + 0xB9B0);
		InjectJmp(AddressDLL.CryGame + 0x60F5D, &setHealth);
		AddConsoleMessage("Hook setHealth Created");
		
		ID3D10Device * pDevice = NULL;
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
	
	if (AddressDLL.pCModeCostum == 0) {
		unsigned __int64 pCModeCostum = *(unsigned __int64*)(AddressDLL.CryAction + 0x465C00);
		if (pCModeCostum) {
			pCModeCostum = *(unsigned __int64*)(pCModeCostum + 0xF0);
			if (pCModeCostum)
				pCModeCostum = *(unsigned __int64*)(pCModeCostum + 0x28);
			if (pCModeCostum)
				pCModeCostum = *(unsigned __int64*)(pCModeCostum);
			if (pCModeCostum)
				pCModeCostum = *(unsigned __int64*)(pCModeCostum + 0xD0);
			if (pCModeCostum)
				pCModeCostum = *(unsigned __int64*)(pCModeCostum + 0x68);
			if (pCModeCostum) {
				AddressDLL.pCModeCostum = *(unsigned __int64*)(pCModeCostum + 0x70);
			}
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

		if (CActivation.NoOverheat) {
			nop((void*)(AddressDLL.CryGame + OWERHEAT[0]), 8);
			nop((void*)(AddressDLL.CryGame + OWERHEAT[1]), 8);
		}
		else {
			restoreoriginalbyte((void*)(AddressDLL.CryGame + OWERHEAT[0]), (PBYTE)oByte[ioBYTE::bOwerheat1].c_str(), 8);
			restoreoriginalbyte((void*)(AddressDLL.CryGame + OWERHEAT[1]), (PBYTE)oByte[ioBYTE::bOwerheat2].c_str(), 8);
		}
		CheckPressed = false;
	}
	if (AddressDLL.pCModeCostum > 0) {
		if (CActivation.SpeedHack) {
			*reinterpret_cast<float*>(AddressDLL.pCModeCostum + 0x90) = CActivation.speed;
			*reinterpret_cast<float*>(AddressDLL.pCModeCostum + 0x94) = CActivation.speed;
		}
	}
}

bool InitHookDX(HWND hwnd) {
	d3d10hk = new D3D10HOOK(hwnd);
	if (!d3d10hk->GetHookStatus())
		return false;

	d3d10hk->SetSwapChainHook(&HookedPresentD3D10, SwapChainIndex::iPresent);

	return true;
}