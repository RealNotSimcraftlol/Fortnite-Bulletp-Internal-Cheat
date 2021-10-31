#include "SDK.h"
#include "Loot.h"
#include "detours.h"


typedef int (WINAPI* LPFN_MBA)(DWORD);
static LPFN_MBA NtGetAsyncKeyState;

// ye ye feel free to paste
// Fixed Silent

bool ShowMenu = true;

int Menu_AimBoneInt = 0;

static const char* AimBone_TypeItems[]{
	"   Head",
	"   Neck",
	"   Pelvis",
	"   Bottom"
};


static const char* ESP_Box_TypeItems[]{
	"   Box",
	"   Cornered",
	"   3D Box"
};

// Rainbow shit

float color_red = 1.;
float color_green = 0;
float color_blue = 0;
float color_random = 0.0;
float color_speed = -10.0;

// Dont forget to call this in the menu or the rainbow wont work!

void ColorChange()
{
	static float Color[3];
	static DWORD Tickcount = 0;
	static DWORD Tickcheck = 0;
	ImGui::ColorConvertRGBtoHSV(color_red, color_green, color_blue, Color[0], Color[1], Color[2]);
	if (GetTickCount() - Tickcount >= 1)
	{
		if (Tickcheck != Tickcount)
		{
			Color[0] += 0.001f * color_speed;
			Tickcheck = Tickcount;
		}
		Tickcount = GetTickCount();
	}
	if (Color[0] < 0.0f) Color[0] += 1.0f;
	ImGui::ColorConvertHSVtoRGB(Color[0], Color[1], Color[2], color_red, color_green, color_blue);
}

// Speedhack
#pragma comment(lib, "detours.lib")

template<class DataType>
class Monke {
	DataType time_offset;
	DataType time_last_update;

	double speed_;

public:
	Monke(DataType currentRealTime, double initialSpeed) {
		time_offset = currentRealTime;
		time_last_update = currentRealTime;

		speed_ = initialSpeed;
	}

	void setSpeed(DataType currentRealTime, double speed) {
		time_offset = getCurrentTime(currentRealTime);
		time_last_update = currentRealTime;

		speed_ = speed;
	}

	DataType getCurrentTime(DataType currentRealTime) {
		DataType difference = currentRealTime - time_last_update;

		return (DataType)(speed_ * difference) + time_offset;
	}
};

typedef DWORD(WINAPI* GetTickCountType)(void);
typedef ULONGLONG(WINAPI* GetTickCount64Type)(void);

typedef BOOL(WINAPI* QueryPerformanceCounterType)(LARGE_INTEGER* lpPerformanceCount);

GetTickCountType   g_GetTickCountOriginal;
GetTickCount64Type g_GetTickCount64Original;
GetTickCountType   g_TimeGetTimeOriginal;  

QueryPerformanceCounterType g_QueryPerformanceCounterOriginal;


const double kInitialSpeed = 1.0;

Monke<DWORD>     g_Monke(GetTickCount(), kInitialSpeed);
Monke<ULONGLONG> g_MonkeULL(GetTickCount64(), kInitialSpeed);
Monke<LONGLONG>  g_MonkeLL(0, kInitialSpeed);

DWORD     WINAPI GetTickCountHacked(void);
ULONGLONG WINAPI GetTickCount64Hacked(void);

BOOL      WINAPI QueryPerformanceCounterHacked(LARGE_INTEGER* lpPerformanceCount);

DWORD     WINAPI KeysThread(LPVOID lpThreadParameter);

// Dont forget to call this at the start of the cheat or it wont work!

void Gay()
{
	HMODULE kernel32 = GetModuleHandleA(xorstr("Kernel32.dll"));
	HMODULE winmm = GetModuleHandleA(xorstr("Winmm.dll"));

	g_GetTickCountOriginal = (GetTickCountType)GetProcAddress(kernel32, xorstr("GetTickCount"));
	g_GetTickCount64Original = (GetTickCount64Type)GetProcAddress(kernel32, xorstr("GetTickCount64"));

	g_TimeGetTimeOriginal = (GetTickCountType)GetProcAddress(winmm, xorstr("timeGetTime"));

	g_QueryPerformanceCounterOriginal = (QueryPerformanceCounterType)GetProcAddress(kernel32, xorstr("QueryPerformanceCounter"));

	LARGE_INTEGER performanceCounter;
	g_QueryPerformanceCounterOriginal(&performanceCounter);

	g_MonkeLL = Monke<LONGLONG>(performanceCounter.QuadPart, kInitialSpeed);

	DetourTransactionBegin();

	DetourAttach((PVOID*)&g_GetTickCountOriginal, (PVOID)GetTickCountHacked);
	DetourAttach((PVOID*)&g_GetTickCount64Original, (PVOID)GetTickCount64Hacked);

	DetourAttach((PVOID*)&g_TimeGetTimeOriginal, (PVOID)GetTickCountHacked);

	DetourAttach((PVOID*)&g_QueryPerformanceCounterOriginal, (PVOID)QueryPerformanceCounterHacked);

	DetourTransactionCommit();
}

void setAllToSpeed(double speed) {
	g_Monke.setSpeed(g_GetTickCountOriginal(), speed);

	g_MonkeULL.setSpeed(g_GetTickCount64Original(), speed);

	LARGE_INTEGER performanceCounter;
	g_QueryPerformanceCounterOriginal(&performanceCounter);

	g_MonkeLL.setSpeed(performanceCounter.QuadPart, speed);
}

DWORD WINAPI GetTickCountHacked(void) {
	return g_Monke.getCurrentTime(g_GetTickCountOriginal());
}

ULONGLONG WINAPI GetTickCount64Hacked(void) {
	return g_MonkeULL.getCurrentTime(g_GetTickCount64Original());
}

BOOL WINAPI QueryPerformanceCounterHacked(LARGE_INTEGER* lpPerformanceCount) {
	LARGE_INTEGER performanceCounter;

	BOOL result = g_QueryPerformanceCounterOriginal(&performanceCounter);

	lpPerformanceCount->QuadPart = g_MonkeLL.getCurrentTime(performanceCounter.QuadPart);

	return result;
}

// Watermark and Speed

void DrawWaterMark(ImGuiWindow& windowshit, std::string str, ImVec2 loc, ImU32 colr, bool centered = false)
{
	ImVec2 size = { 0,0 };
	float minx = 0;
	float miny = 0;
	if (centered)
	{
		size = ImGui::GetFont()->CalcTextSizeA(windowshit.DrawList->_Data->FontSize, 0x7FFFF, 0, str.c_str());
		minx = size.x / 2.f;
		miny = size.y / 2.f;
	}

	windowshit.DrawList->AddText(ImVec2((loc.x - 1) - minx, (loc.y - 1) - miny), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), str.c_str());
	windowshit.DrawList->AddText(ImVec2((loc.x + 1) - minx, (loc.y + 1) - miny), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), str.c_str());
	windowshit.DrawList->AddText(ImVec2((loc.x + 1) - minx, (loc.y - 1) - miny), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), str.c_str());
	windowshit.DrawList->AddText(ImVec2((loc.x - 1) - minx, (loc.y + 1) - miny), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), str.c_str());
	windowshit.DrawList->AddText(ImVec2(loc.x - minx, loc.y - miny), colr, str.c_str());
}

// Dont forget to call this at the start of the cheat or it wont work!

void gaybow(ImGuiWindow& window) {
	auto RGB = ImGui::GetColorU32({ color_red, color_green, color_blue, 255 });
	DrawWaterMark(window, "Text or Discord invite lol", ImVec2(50, 100), RGB, false);
	//DrawWaterMark(window, "Second text", ImVec2(50, 120), FpsColor, false);

	if (Settings::Speed && NtGetAsyncKeyState(VK_SHIFT)) {
		setAllToSpeed(Settings::SpeedValue);
	}
	else
	{
		setAllToSpeed(1.0);
	}
}


PVOID TargetPawn = nullptr;

namespace HookFunctions {
	bool Init(bool NoSpread, bool CalcShot);
	bool NoSpreadInitialized = false;
	bool CalcShotInitialized = false;
}


ID3D11Device* device = nullptr;
ID3D11DeviceContext* immediateContext = nullptr;
ID3D11RenderTargetView* renderTargetView = nullptr;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;



WNDPROC oWndProc = NULL;


typedef HRESULT(*present_fn)(IDXGISwapChain*, UINT, UINT);
inline present_fn present_original{ };

typedef HRESULT(*resize_fn)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
inline resize_fn resize_original{ };



//HRESULT(*present_original)(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) = nullptr;
//HRESULT(*resize_original)(IDXGISwapChain* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) = nullptr;



extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void Draw2DBoundingBox(Vector3 StartBoxLoc, float flWidth, float Height, ImColor color)
{
	StartBoxLoc.x = StartBoxLoc.x - (flWidth / 2);
	ImDrawList* Renderer = ImGui::GetOverlayDrawList();
	Renderer->AddLine(ImVec2(StartBoxLoc.x, StartBoxLoc.y), ImVec2(StartBoxLoc.x + flWidth, StartBoxLoc.y), color, 1); //bottom
	Renderer->AddLine(ImVec2(StartBoxLoc.x, StartBoxLoc.y), ImVec2(StartBoxLoc.x, StartBoxLoc.y + Height), color, 1); //left
	Renderer->AddLine(ImVec2(StartBoxLoc.x + flWidth, StartBoxLoc.y), ImVec2(StartBoxLoc.x + flWidth, StartBoxLoc.y + Height), color, 1); //right
	Renderer->AddLine(ImVec2(StartBoxLoc.x, StartBoxLoc.y + Height), ImVec2(StartBoxLoc.x + flWidth, StartBoxLoc.y + Height), color, 1); //up
}

void DrawCorneredBox(int X, int Y, int W, int H, ImColor color, int thickness) {
	float lineW = (W / 3);
	float lineH = (H / 3);
	ImDrawList* Renderer = ImGui::GetOverlayDrawList();
	Renderer->AddLine(ImVec2(X, Y), ImVec2(X, Y + lineH), color, thickness);

	Renderer->AddLine(ImVec2(X, Y), ImVec2(X + lineW, Y), color, thickness);

	Renderer->AddLine(ImVec2(X + W - lineW, Y), ImVec2(X + W, Y), color, thickness);

	Renderer->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + lineH), color, thickness);

	Renderer->AddLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H), color, thickness);

	Renderer->AddLine(ImVec2(X, Y + H), ImVec2(X + lineW, Y + H), color, thickness);

	Renderer->AddLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W, Y + H), color, thickness);

	Renderer->AddLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H), color, thickness);

}

bool IsAiming()
{
	return NtGetAsyncKeyState(VK_RBUTTON);
}

auto GetSyscallIndex(std::string ModuleName, std::string SyscallFunctionName, void* Function) -> bool
{
	auto ModuleBaseAddress = LI_FN(GetModuleHandleA)(ModuleName.c_str());
	if (!ModuleBaseAddress)
		ModuleBaseAddress = LI_FN(LoadLibraryA)(ModuleName.c_str());
	if (!ModuleBaseAddress)
		return false;

	auto GetFunctionAddress = LI_FN(GetProcAddress)(ModuleBaseAddress, SyscallFunctionName.c_str());
	if (!GetFunctionAddress)
		return false;

	auto SyscallIndex = *(DWORD*)((PBYTE)GetFunctionAddress + 4);

	*(DWORD*)((PBYTE)Function + 4) = SyscallIndex;

	return true;
}

extern "C"
{
	NTSTATUS _NtUserSendInput(UINT a1, LPINPUT Input, int Size);
};

VOID mouse_event_(DWORD dwFlags, DWORD dx, DWORD dy, DWORD dwData, ULONG_PTR dwExtraInfo)
{
	static bool doneonce;
	if (!doneonce)
	{
		if (!GetSyscallIndex(xorstr("win32u.dll"), xorstr("NtUserSendInput"), _NtUserSendInput))
			return;
		doneonce = true;
	}

	INPUT Input[3] = { 0 };
	Input[0].type = INPUT_MOUSE;
	Input[0].mi.dx = dx;
	Input[0].mi.dy = dy;
	Input[0].mi.mouseData = dwData;
	Input[0].mi.dwFlags = dwFlags;
	Input[0].mi.time = 0;
	Input[0].mi.dwExtraInfo = dwExtraInfo;

	_NtUserSendInput((UINT)1, (LPINPUT)&Input, (INT)sizeof(INPUT));
}


Vector3 head2, neck, pelvis, chest, leftShoulder, rightShoulder, leftElbow, rightElbow, leftHand, rightHand, leftLeg, rightLeg, leftThigh, rightThigh, leftFoot, rightFoot, leftFeet, rightFeet, leftFeetFinger, rightFeetFinger;

bool GetAllBones(uintptr_t CurrentActor) {
	Vector3 chesti, chestatright;

	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 66, &head2);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 65, &neck);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 2, &pelvis);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 36, &chesti);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 8, &chestatright);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 9, &leftShoulder);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 37, &rightShoulder);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 10, &leftElbow);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 38, &rightElbow);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 11, &leftHand);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 39, &rightHand);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 67, &leftLeg);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 74, &rightLeg);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 73, &leftThigh);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 80, &rightThigh);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 68, &leftFoot);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 75, &rightFoot);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 71, &leftFeet);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 78, &rightFeet);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 72, &leftFeetFinger);
	SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 79, &rightFeetFinger);


	SDK::Classes::AController::WorldToScreen(head2, &head2);
	SDK::Classes::AController::WorldToScreen(neck, &neck);
	SDK::Classes::AController::WorldToScreen(pelvis, &pelvis);
	SDK::Classes::AController::WorldToScreen(chesti, &chesti);
	SDK::Classes::AController::WorldToScreen(chestatright, &chestatright);
	SDK::Classes::AController::WorldToScreen(leftShoulder, &leftShoulder);
	SDK::Classes::AController::WorldToScreen(rightShoulder, &rightShoulder);
	SDK::Classes::AController::WorldToScreen(leftElbow, &leftElbow);
	SDK::Classes::AController::WorldToScreen(rightElbow, &rightElbow);
	SDK::Classes::AController::WorldToScreen(leftHand, &leftHand);
	SDK::Classes::AController::WorldToScreen(rightHand, &rightHand);
	SDK::Classes::AController::WorldToScreen(leftLeg, &leftLeg);
	SDK::Classes::AController::WorldToScreen(rightLeg, &rightLeg);
	SDK::Classes::AController::WorldToScreen(leftThigh, &leftThigh);
	SDK::Classes::AController::WorldToScreen(rightThigh, &rightThigh);
	SDK::Classes::AController::WorldToScreen(leftFoot, &leftFoot);
	SDK::Classes::AController::WorldToScreen(rightFoot, &rightFoot);
	SDK::Classes::AController::WorldToScreen(leftFeet, &leftFeet);
	SDK::Classes::AController::WorldToScreen(rightFeet, &rightFeet);
	SDK::Classes::AController::WorldToScreen(leftFeetFinger, &leftFeetFinger);
	SDK::Classes::AController::WorldToScreen(rightFeetFinger, &rightFeetFinger);

	chest.x = chesti.x + ((chestatright.x - chesti.x) / 2);
	chest.y = chesti.y;

	return true;
}

// Simple Fov Fix lmao

bool InFov(uintptr_t CurrentPawn, int FovValue) {
	Vector3 HeadPos; SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentPawn, 66, &HeadPos); SDK::Classes::AController::WorldToScreen(HeadPos, &HeadPos);
	auto dx = HeadPos.x - (Renderer_Defines::Width / 2);
	auto dy = HeadPos.y - (Renderer_Defines::Height / 2);
	auto dist = sqrtf(dx * dx + dy * dy);

	if (dist < FovValue) {
		return true;
	}
	else {
		return false;
	}
}

bool EntitiyLoop()
{


	ImDrawList* Renderer = ImGui::GetOverlayDrawList();


	if (Settings::crosshair)
	{
		Renderer->AddLine(ImVec2(Renderer_Defines::Width / 2 - 7, Renderer_Defines::Height / 2), ImVec2(Renderer_Defines::Width / 2 + 1, Renderer_Defines::Height / 2), ImColor(255, 0, 0, 255), 1.0);
		Renderer->AddLine(ImVec2(Renderer_Defines::Width / 2 + 8, Renderer_Defines::Height / 2), ImVec2(Renderer_Defines::Width / 2 + 1, Renderer_Defines::Height / 2), ImColor(255, 0, 0, 255), 1.0);
		Renderer->AddLine(ImVec2(Renderer_Defines::Width / 2, Renderer_Defines::Height / 2 - 7), ImVec2(Renderer_Defines::Width / 2, Renderer_Defines::Height / 2), ImColor(255, 0, 0, 255), 1.0);
		Renderer->AddLine(ImVec2(Renderer_Defines::Width / 2, Renderer_Defines::Height / 2 + 8), ImVec2(Renderer_Defines::Width / 2, Renderer_Defines::Height / 2), ImColor(255, 0, 0, 255), 1.0);
	}

	if (Settings::ShowFovCircle and !Settings::fov360)
		Renderer->AddCircle(ImVec2(Renderer_Defines::Width / 2, Renderer_Defines::Height / 2), Settings::FovCircle_Value, SettingsColor::FovCircle, 124);











	try
	{
		float FOVmax = 9999.f;

		float closestDistance = FLT_MAX;
		PVOID closestPawn = NULL;
		bool closestPawnVisible = false;


		uintptr_t MyTeamIndex = 0, EnemyTeamIndex = 0;
		uintptr_t GWorld = read<uintptr_t>(UWorld); if (!GWorld) return false;

		uintptr_t Gameinstance = read<uint64_t>(GWorld + StaticOffsets::OwningGameInstance); if (!Gameinstance) return false;

		uintptr_t LocalPlayers = read<uint64_t>(Gameinstance + StaticOffsets::LocalPlayers); if (!LocalPlayers) return false;

		uintptr_t LocalPlayer = read<uint64_t>(LocalPlayers); if (!LocalPlayer) return false;

		PlayerController = read<uint64_t>(LocalPlayer + StaticOffsets::PlayerController); if (!PlayerController) return false;

		uintptr_t PlayerCameraManager = read<uint64_t>(PlayerController + StaticOffsets::PlayerCameraManager); if (!PlayerCameraManager) return false;

		FOVAngle = SDK::Classes::APlayerCameraManager::GetFOVAngle(PlayerCameraManager);
		SDK::Classes::APlayerCameraManager::GetPlayerViewPoint(PlayerCameraManager, &CamLoc, &CamRot);

		LocalPawn = read<uint64_t>(PlayerController + StaticOffsets::AcknowledgedPawn);

		uintptr_t Ulevel = read<uintptr_t>(GWorld + StaticOffsets::PersistentLevel); if (!Ulevel) return false;

		uintptr_t AActors = read<uintptr_t>(Ulevel + StaticOffsets::AActors); if (!AActors) return false;

		uintptr_t ActorCount = read<int>(Ulevel + StaticOffsets::ActorCount); if (!ActorCount) return false;


		uintptr_t LocalRootComponent;
		Vector3 LocalRelativeLocation;

		if (valid_pointer(LocalPawn)) {
			LocalRootComponent = read<uintptr_t>(LocalPawn + 0x130);
			LocalRelativeLocation = read<Vector3>(LocalRootComponent + 0x11C);
		}

		for (int i = 0; i < ActorCount; i++) {

			auto CurrentActor = read<uintptr_t>(AActors + i * sizeof(uintptr_t));

			auto name = SDK::Classes::UObject::GetObjectName(CurrentActor);

			bool IsVisible = false;

			if (valid_pointer(LocalPawn))
			{


				if (Settings::VehiclesESP && strstr(name, xorstr("Vehicl")) || strstr(name, xorstr("Valet_Taxi")) || strstr(name, xorstr("Valet_BigRig")) || strstr(name, xorstr("Valet_BasicTr")) || strstr(name, xorstr("Valet_SportsC")) || strstr(name, xorstr("Valet_BasicC")))
				{
					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;

					if (ItemDist < Settings::MaxESPDistance) {
						Vector3 VehiclePosition;
						SDK::Classes::AController::WorldToScreen(ItemPosition, &VehiclePosition);
						std::string null = xorstr("");
						std::string Text = null + xorstr("Vehicle [") + std::to_string((int)ItemDist) + xorstr("m]");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());

						Renderer->AddText(ImVec2(VehiclePosition.x, VehiclePosition.y), ImColor(255, 255, 255, 255), Text.c_str());
					}

				}
				else if (Settings::LLamaESP && strstr(name, xorstr("AthenaSupplyDrop_Llama"))) {
					uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
					Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
					float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;

					if (ItemDist < Settings::MaxESPDistance) {
						Vector3 LLamaPosition;
						SDK::Classes::AController::WorldToScreen(ItemPosition, &LLamaPosition);

						std::string null = xorstr("");
						std::string Text = null + xorstr("LLama [") + std::to_string((int)ItemDist) + xorstr("m]");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());

						Renderer->AddText(ImVec2(LLamaPosition.x, LLamaPosition.y), SettingsColor::LLamaESP, Text.c_str());
					}
				}
				else if (strstr(name, xorstr("PlayerPawn"))) {
					Vector3 HeadPos, Headbox, bottom;

					SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 66, &HeadPos);
					SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 0, &bottom);

					SDK::Classes::AController::WorldToScreen(Vector3(HeadPos.x, HeadPos.y, HeadPos.z + 20), &Headbox);
					SDK::Classes::AController::WorldToScreen(bottom, &bottom);

					if (Headbox.x == 0 && Headbox.y == 0) continue;
					if (bottom.x == 0 && bottom.y == 0) continue;



					


					uintptr_t MyState = read<uintptr_t>(LocalPawn + StaticOffsets::PlayerState);
					if (!MyState) continue;

					MyTeamIndex = read<uintptr_t>(MyState + StaticOffsets::TeamIndex);
					if (!MyTeamIndex) continue;

					uintptr_t EnemyState = read<uintptr_t>(CurrentActor + StaticOffsets::PlayerState);
					if (!EnemyState) continue;

					EnemyTeamIndex = read<uintptr_t>(EnemyState + StaticOffsets::TeamIndex);
					if (!EnemyTeamIndex) continue;

					if (CurrentActor == LocalPawn) continue;

					Vector3 viewPoint;

					if (Settings::VisibleCheck) {
						IsVisible = SDK::Classes::APlayerCameraManager::LineOfSightTo((PVOID)PlayerController, (PVOID)CurrentActor, &viewPoint);
					}


					if (Settings::SnapLines) {
						ImColor col;
						if (IsVisible) {
							col = SettingsColor::Snaplines;
						}
						else {
							col = SettingsColor::Snaplines_notvisible;
						}
						Vector3 LocalPelvis;
						SDK::Classes::USkeletalMeshComponent::GetBoneLocation(LocalPawn, 2, &LocalPelvis);
						SDK::Classes::AController::WorldToScreen(LocalPelvis, &LocalPelvis);

						Renderer->AddLine(ImVec2(LocalPelvis.x, LocalPelvis.y), ImVec2(pelvis.x, pelvis.y), col, 1.f);
					}

					if (Settings::DistanceESP && SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height)) {
						Vector3 HeadNotW2SForDistance;
						SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 66, &HeadNotW2SForDistance);
						float distance = LocalRelativeLocation.Distance(HeadNotW2SForDistance) / 100.f;

						std::string null = "";
						std::string finalstring = null + xorstr(" [") + std::to_string((int)distance) + xorstr("m]");

						ImVec2 DistanceTextSize = ImGui::CalcTextSize(finalstring.c_str());

						ImColor col;
						if (IsVisible) {
							col = SettingsColor::Distance;
						}
						else {
							col = SettingsColor::Distance_notvisible;
						}

						Renderer->AddText(ImVec2(bottom.x - DistanceTextSize.x / 2, bottom.y + DistanceTextSize.y / 2), col, finalstring.c_str());

					}


					if (Settings::NoSpread) {
						if (!HookFunctions::NoSpreadInitialized) {
							HookFunctions::Init(true, false);
						}
					}

					// bullettp / silent

					if (Settings::Bullettp || Settings::SilentAim) {
						if (!HookFunctions::CalcShotInitialized) {
							HookFunctions::Init(false, true);
						}

						if (!Settings::fov360) {
							auto dx = Headbox.x - (Renderer_Defines::Width / 2);
							auto dy = Headbox.y - (Renderer_Defines::Height / 2);
							auto dist = SpoofRuntime::sqrtf_(dx * dx + dy * dy);

							if (dist < Settings::FovCircle_Value && dist < closestDistance) {
								closestDistance = dist;
								closestPawn = (PVOID)CurrentActor;
							}
						}
						else {
							closestPawn = (PVOID)CurrentActor;
						}


					}
					else if (Settings::Bullettp || Settings::SilentAim and !IsVisible) {
						closestPawn = nullptr;
					}

				//	if (Settings::MouseAim and IsAiming() and (MyTeamIndex != EnemyTeamIndex)) {


					if (Settings::Aim and IsAiming() and (MyTeamIndex != EnemyTeamIndex))
					{


						if (Settings::fov360) {
							if (Settings::VisibleCheck and IsVisible) {

								auto NewRotation = SDK::Utils::CalculateNewRotation(CurrentActor, LocalRelativeLocation, Settings::AimPrediction);
								SDK::Classes::AController::SetControlRotation(NewRotation, false);
								if (IsVisible and Settings::trigger) {
									mouse_event_(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
									mouse_event_(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
								}
							}
							else if (!Settings::VisibleCheck) {

								auto NewRotation = SDK::Utils::CalculateNewRotation(CurrentActor, LocalRelativeLocation, Settings::AimPrediction);
								SDK::Classes::AController::SetControlRotation(NewRotation, false);
								if (IsVisible and Settings::trigger) {
									mouse_event_(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
									mouse_event_(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
								}
							}
						}
						else if (!Settings::fov360 and InFov(CurrentActor, Settings::FovCircle_Value)) {

							if (Settings::VisibleCheck and IsVisible) {
								auto NewRotation = SDK::Utils::CalculateNewRotation(CurrentActor, LocalRelativeLocation, Settings::AimPrediction);
								SDK::Classes::AController::SetControlRotation(NewRotation, false);
								if (IsVisible and Settings::trigger) {
									mouse_event_(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
									mouse_event_(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
								}
							}
							else if (!Settings::VisibleCheck) {

								auto NewRotation = SDK::Utils::CalculateNewRotation(CurrentActor, LocalRelativeLocation, Settings::AimPrediction);
								SDK::Classes::AController::SetControlRotation(NewRotation, false);
								if (IsVisible and Settings::trigger) {
									mouse_event_(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
									mouse_event_(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
								}
							}
						}

					}

					if (Settings::InstantRevive) {


						write<float>(LocalPawn + 0x3398, 1); // AFortPlayerPawnAthena->ReviveFromDBNOTime
						Settings::InstantRevive = false;
					}

					if (Settings::AirStuck) {
						if (NtGetAsyncKeyState(VK_MENU)) { //alt
							write<float>(LocalPawn + 0x98, 0); // AActor->CustomTimeDilation
						}
						else {
							write<float>(LocalPawn + 0x98, 1); // AActor->CustomTimeDilation
						}
					}

					if (Settings::AimWhileJumping) {
						*(bool*)(LocalPawn + StaticOffsets::bADSWhileNotOnGround) = true;
					}
					else {
						*(bool*)(LocalPawn + StaticOffsets::bADSWhileNotOnGround) = false;
					}
				}
			}




			if (strstr(name, xorstr("BP_PlayerPawn")) || strstr(name, xorstr("PlayerPawn")))
			{

				if (SDK::Utils::CheckInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height)) {

					Vector3 HeadPos, Headbox, bottom;

					SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 66, &HeadPos);
					SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 0, &bottom);

					SDK::Classes::AController::WorldToScreen(Vector3(HeadPos.x, HeadPos.y, HeadPos.z + 20), &Headbox);
					SDK::Classes::AController::WorldToScreen(bottom, &bottom);


					if (Settings::BoxTypeSelected == 0 or Settings::BoxTypeSelected == 1 or Settings::Skeleton) {
						GetAllBones(CurrentActor);
					}

					//int MostRightBone, MostLeftBone;
					int array[20] = { head2.x, neck.x, pelvis.x, chest.x, leftShoulder.x, rightShoulder.x, leftElbow.x, rightElbow.x, leftHand.x, rightHand.x, leftLeg.x, rightLeg.x, leftThigh.x, rightThigh.x, leftFoot.x, rightFoot.x, leftFeet.x, rightFeet.x, leftFeetFinger.x, rightFeetFinger.x };
					int MostRightBone = array[0];
					int MostLeftBone = array[0];

					for (int mostrighti = 0; mostrighti < 20; mostrighti++)
					{
						if (array[mostrighti] > MostRightBone)
							MostRightBone = array[mostrighti];
					}

					for (int mostlefti = 0; mostlefti < 20; mostlefti++)
					{
						if (array[mostlefti] < MostLeftBone)
							MostLeftBone = array[mostlefti];
					}



					float ActorHeight = (Headbox.y - bottom.y);
					if (ActorHeight < 0) ActorHeight = ActorHeight * (-1.f);

					int ActorWidth = MostRightBone - MostLeftBone;

					

					if (Settings::Skeleton)
					{

						ImColor col;
						if (IsVisible) {
							col = SettingsColor::Skeleton;
						}
						else {
							col = SettingsColor::Skeleton_notvisible;
						}


						Renderer->AddLine(ImVec2(head2.x, head2.y), ImVec2(neck.x, neck.y), col, 1.f);
						Renderer->AddLine(ImVec2(neck.x, neck.y), ImVec2(chest.x, chest.y), col, 1.f);
						Renderer->AddLine(ImVec2(chest.x, chest.y), ImVec2(pelvis.x, pelvis.y), col, 1.f);
						Renderer->AddLine(ImVec2(chest.x, chest.y), ImVec2(leftShoulder.x, leftShoulder.y), col, 1.f);
						Renderer->AddLine(ImVec2(chest.x, chest.y), ImVec2(rightShoulder.x, rightShoulder.y), col, 1.f);
						Renderer->AddLine(ImVec2(leftShoulder.x, leftShoulder.y), ImVec2(leftElbow.x, leftElbow.y), col, 1.f);
						Renderer->AddLine(ImVec2(rightShoulder.x, rightShoulder.y), ImVec2(rightElbow.x, rightElbow.y), col, 1.f);
						Renderer->AddLine(ImVec2(leftElbow.x, leftElbow.y), ImVec2(leftHand.x, leftHand.y), col, 1.f);
						Renderer->AddLine(ImVec2(rightElbow.x, rightElbow.y), ImVec2(rightHand.x, rightHand.y), col, 1.f);
						Renderer->AddLine(ImVec2(pelvis.x, pelvis.y), ImVec2(leftLeg.x, leftLeg.y), col, 1.f);
						Renderer->AddLine(ImVec2(pelvis.x, pelvis.y), ImVec2(rightLeg.x, rightLeg.y), col, 1.f);
						Renderer->AddLine(ImVec2(leftLeg.x, leftLeg.y), ImVec2(leftThigh.x, leftThigh.y), col, 1.f);
						Renderer->AddLine(ImVec2(rightLeg.x, rightLeg.y), ImVec2(rightThigh.x, rightThigh.y), col, 1.f);
						Renderer->AddLine(ImVec2(leftThigh.x, leftThigh.y), ImVec2(leftFoot.x, leftFoot.y), col, 1.f);
						Renderer->AddLine(ImVec2(rightThigh.x, rightThigh.y), ImVec2(rightFoot.x, rightFoot.y), col, 1.f);
						Renderer->AddLine(ImVec2(leftFoot.x, leftFoot.y), ImVec2(leftFeet.x, leftFeet.y), col, 1.f);
						Renderer->AddLine(ImVec2(rightFoot.x, rightFoot.y), ImVec2(rightFeet.x, rightFeet.y), col, 1.f);
						Renderer->AddLine(ImVec2(leftFeet.x, leftFeet.y), ImVec2(leftFeetFinger.x, leftFeetFinger.y), col, 1.f);
						Renderer->AddLine(ImVec2(rightFeet.x, rightFeet.y), ImVec2(rightFeetFinger.x, rightFeetFinger.y), col, 1.f);




					}

					if (Settings::Box and Settings::BoxTypeSelected == 2) {

						Vector3 BottomNoW2S;
						Vector3 HeadNoW2S;

						SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 66, &HeadNoW2S);
						SDK::Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 0, &BottomNoW2S);


						Vector3 bottom1;
						Vector3 bottom2;
						Vector3 bottom3;
						Vector3 bottom4;

						SDK::Classes::AController::WorldToScreen(Vector3(BottomNoW2S.x + 30, BottomNoW2S.y - 30, BottomNoW2S.z), &bottom1);
						SDK::Classes::AController::WorldToScreen(Vector3(BottomNoW2S.x - 30, BottomNoW2S.y - 30, BottomNoW2S.z), &bottom2);
						SDK::Classes::AController::WorldToScreen(Vector3(BottomNoW2S.x - 30, BottomNoW2S.y + 30, BottomNoW2S.z), &bottom3);
						SDK::Classes::AController::WorldToScreen(Vector3(BottomNoW2S.x + 30, BottomNoW2S.y + 30, BottomNoW2S.z), &bottom4);



						Vector3 top1;
						Vector3 top2;
						Vector3 top3;
						Vector3 top4;

						SDK::Classes::AController::WorldToScreen(Vector3(HeadNoW2S.x + 30, HeadNoW2S.y - 30, HeadNoW2S.z), &top1);
						SDK::Classes::AController::WorldToScreen(Vector3(HeadNoW2S.x - 30, HeadNoW2S.y - 30, HeadNoW2S.z), &top2);
						SDK::Classes::AController::WorldToScreen(Vector3(HeadNoW2S.x - 30, HeadNoW2S.y + 30, HeadNoW2S.z), &top3);
						SDK::Classes::AController::WorldToScreen(Vector3(HeadNoW2S.x + 30, HeadNoW2S.y + 30, HeadNoW2S.z), &top4);


						ImColor col;
						if (IsVisible) {
							col = SettingsColor::Box;
						}
						else {
							col = SettingsColor::Box_notvisible;
						}

						Renderer->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(top1.x, top1.y), col, 1.f);
						Renderer->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(top2.x, top2.y), col, 1.f);
						Renderer->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(top3.x, top3.y), col, 1.f);
						Renderer->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(top4.x, top4.y), col, 1.f);


						Renderer->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(bottom2.x, bottom2.y), col, 1.f);
						Renderer->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(bottom3.x, bottom3.y), col, 1.f);
						Renderer->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(bottom4.x, bottom4.y), col, 1.f);
						Renderer->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(bottom1.x, bottom1.y), col, 1.f);


						Renderer->AddLine(ImVec2(top1.x, top1.y), ImVec2(top2.x, top2.y), col, 1.f);
						Renderer->AddLine(ImVec2(top2.x, top2.y), ImVec2(top3.x, top3.y), col, 1.f);
						Renderer->AddLine(ImVec2(top3.x, top3.y), ImVec2(top4.x, top4.y), col, 1.f);
						Renderer->AddLine(ImVec2(top4.x, top4.y), ImVec2(top1.x, top1.y), col, 1.f);




					}

					

					if (Settings::Box and Settings::BoxTypeSelected == 0) {


						ImColor col;
						if (IsVisible) {
							col = SettingsColor::Box;
						}
						else {
							col = SettingsColor::Box_notvisible;
						}


						Draw2DBoundingBox(Headbox, ActorWidth, ActorHeight, col);


					}
					if (Settings::Box and Settings::BoxTypeSelected == 1) {



						ImColor col;
						if (IsVisible) {
							col = SettingsColor::Box;
						}
						else {
							col = SettingsColor::Box_notvisible;
						}

						DrawCorneredBox(Headbox.x - (ActorWidth / 2), Headbox.y, ActorWidth, ActorHeight, col, 1.5);
					}


				}




			}
			
		}


		if (Settings::Bullettp || Settings::SilentAim) {
			if (closestPawn && NtGetAsyncKeyState(VK_RBUTTON)) {
				TargetPawn = closestPawn;
			}
			else {
				TargetPawn = nullptr;
			}
		}
		else {
			TargetPawn = nullptr;
		}


		if (!LocalPawn) return false;


		int AtLeastOneBool = 0;
		if (Settings::ChestESP) AtLeastOneBool++; if (Settings::AmmoBoxESP) AtLeastOneBool++; if (Settings::LootESP) AtLeastOneBool++;

		if (AtLeastOneBool == 0) return false;


		for (auto Itemlevel_i = 0UL; Itemlevel_i < read<DWORD>(GWorld + (StaticOffsets::Levels + sizeof(PVOID))); ++Itemlevel_i) {
			uintptr_t ItemLevels = read<uintptr_t>(GWorld + StaticOffsets::Levels);
			if (!ItemLevels) return false;

			uintptr_t ItemLevel = read<uintptr_t>(ItemLevels + (Itemlevel_i * sizeof(uintptr_t)));
			if (!ItemLevel) return false;

			for (int i = 0; i < read<DWORD>(ItemLevel + (StaticOffsets::AActors + sizeof(PVOID))); ++i) {


				uintptr_t ItemsPawns = read<uintptr_t>(ItemLevel + StaticOffsets::AActors);
				if (!ItemsPawns) return false;

				uintptr_t CurrentItemPawn = read<uintptr_t>(ItemsPawns + (i * sizeof(uintptr_t)));

				auto CurrentItemPawnName = SDK::Classes::UObject::GetObjectName(CurrentItemPawn);

				if (LocalPawn) {
					//Loot ESP
					LootESP(Renderer, CurrentItemPawnName, CurrentItemPawn, LocalRelativeLocation);
				}



			}
		}



	}
	catch (...) {}


}


void ColorAndStyle() {
	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2(4.5f, 2.5f);
	style->FrameRounding = 0.0f;
	style->ItemSpacing = ImVec2(12, 8);

	style->WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style->IndentSpacing = 25.0f;


	//Tabs
	style->ItemInnerSpacing = ImVec2(18, 6);
	//style->TabRounding = 0.0f;

	style->ScrollbarSize = 0.0f;
	style->ScrollbarRounding = 0.0f;

	//Sliders
	style->GrabMinSize = 6.0f;
	style->GrabRounding = 0.0f;


	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImColor(27, 26, 35, 255);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0, 0, 0, 0);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);
	style->Colors[ImGuiCol_FrameBg] = ImColor(37, 38, 51, 255);
	style->Colors[ImGuiCol_FrameBgHovered] = ImColor(42, 43, 56, 255);
	style->Colors[ImGuiCol_FrameBgActive] = ImColor(37, 38, 51, 255);


	//style->Colors[ImGuiCol_Tab] = ImColor(29, 28, 37, 255);
	//style->Colors[ImGuiCol_TabActive] = ImColor(79, 79, 81, 255);
	//style->Colors[ImGuiCol_TabHovered] = ImColor(62, 62, 66, 255);

	style->Colors[ImGuiCol_TitleBg] = ImColor(27, 26, 35, 255);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImColor(37, 38, 51, 40);
	style->Colors[ImGuiCol_TitleBgActive] = ImColor(37, 38, 51, 255);

	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImColor(255, 255, 255, 255);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImColor(79, 79, 81, 255);
	style->Colors[ImGuiCol_ButtonHovered] = ImColor(79, 79, 81, 255);
	style->Colors[ImGuiCol_ButtonActive] = ImColor(84, 84, 86, 255);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
}



namespace ImGui
{
	IMGUI_API bool Tab(unsigned int index, const char* label, int* selected, float width = 46, float height = 17)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImColor color = ImColor(27, 26, 35, 255)/*style.Colors[ImGuiCol_Button]*/;
		ImColor colorActive = ImColor(79, 79, 81, 255); /*style.Colors[ImGuiCol_ButtonActive]*/;
		ImColor colorHover = ImColor(62, 62, 66, 255)/*style.Colors[ImGuiCol_ButtonHovered]*/;


		if (index > 0)
			ImGui::SameLine();

		if (index == *selected)
		{
			style.Colors[ImGuiCol_Button] = colorActive;
			style.Colors[ImGuiCol_ButtonActive] = colorActive;
			style.Colors[ImGuiCol_ButtonHovered] = colorActive;
		}
		else
		{
			style.Colors[ImGuiCol_Button] = color;
			style.Colors[ImGuiCol_ButtonActive] = colorActive;
			style.Colors[ImGuiCol_ButtonHovered] = colorHover;
		}

		if (ImGui::Button(label, ImVec2(width, height)))
			*selected = index;

		style.Colors[ImGuiCol_Button] = color;
		style.Colors[ImGuiCol_ButtonActive] = colorActive;
		style.Colors[ImGuiCol_ButtonHovered] = colorHover;

		return *selected == index;
	}
}

/*
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	//if (msg == WM_QUIT && ShowMenu) {
	//	ExitProcess(0);
	//}

	if (ShowMenu) {
		//ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
		return TRUE;
	}

	return CallWindowProc(oWndProc, hWnd, msg, wParam, lParam);
}
*/


ImGuiWindow& CreateScene() {
	ImGui_ImplDX11_NewFrame();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::Begin(xorstr("##mainscenee"), nullptr, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar);

	auto& io = ImGui::GetIO();
	ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGuiCond_Always);

	return *ImGui::GetCurrentWindow();
}

VOID MenuAndDestroy(ImGuiWindow& window) {


	// Call rainbow here!

	ColorChange();

	window.DrawList->PushClipRectFullScreen();
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);


	if (ShowMenu) {
		ColorAndStyle();
		ImGui::SetNextWindowSize({ 709, 470 });
		ImGuiStyle* style = &ImGui::GetStyle();
		static int maintabs = 0;
		static int esptabs = 0;
		if (ImGui::Begin(xorstr("Updated sex internal | NotSimcraftLOL#0001"), &ShowMenu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse )) {


			ImGui::Tab(0, xorstr("Aimbot"), &maintabs);
			ImGui::Tab(1, xorstr("Visuals"), &maintabs);
			ImGui::Tab(2, xorstr("Mods"), &maintabs);
			ImGui::Tab(3, xorstr("Misc"), &maintabs);

			ImGui::Separator(8);


			if (maintabs == 0) {
				ImGui::Columns(2);
				ImGui::Text(xorstr("Aimbot"));
				ImGui::Checkbox(xorstr("Memory Aimbot"), &Settings::Aim);
				ImGui::SameLine();
				ImGui::TextColored(ImColor(255, 231, 94, 255), xorstr("[?]"));
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text(xorstr("Directly locks on your enemy"));
					ImGui::EndTooltip();
				}
				
				ImGui::Checkbox(xorstr("Silent Aimbot"), &Settings::SilentAim);

				ImGui::Combo(xorstr("Aimbone"), &Menu_AimBoneInt, AimBone_TypeItems, sizeof(AimBone_TypeItems) / sizeof(*AimBone_TypeItems));

				if (Menu_AimBoneInt == 0) Settings::aimbone = 66;
				if (Menu_AimBoneInt == 1) Settings::aimbone = 65;
				if (Menu_AimBoneInt == 2) Settings::aimbone = 2;
				if (Menu_AimBoneInt == 3) Settings::aimbone = 0;
				
				ImGui::Checkbox(xorstr("Aimbot Prediction"), &Settings::AimPrediction);
				ImGui::SameLine();
				ImGui::TextColored(ImColor(255, 231, 94, 255), xorstr("[?]"));
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text(xorstr("It predicts where the enemy is going when you have a sniper"));
					ImGui::EndTooltip();
				}
				ImGui::PushItemWidth(190);
				style->GrabMinSize = 2.0f;
				ImGui::SliderInt(xorstr("Aimbot Smooth"), &Settings::smooth, 1, 20);
				style->GrabMinSize = 6.0f;
				ImGui::SameLine();
				ImGui::TextColored(ImColor(255, 231, 94, 255), xorstr("[?]"));
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text(xorstr("Aimbot Smooth: It makes the aimbot smooth."));
					ImGui::EndTooltip();
				}

				ImGui::SliderInt(xorstr("Fov Circle Value"), &Settings::FovCircle_Value, 30, 800);
				ImGui::NextColumn();
				ImGui::Text(xorstr("Colors"));
				ImGui::ColorEdit4(xorstr("Fov Circle"), SettingsColor::FovCircle_float, ImGuiColorEditFlags_NoInputs);

				ImGui::NextColumn();






			}

			else if (maintabs == 1) {


				ImGui::Tab(0, xorstr("Players Visuals"), &esptabs, 65);
				ImGui::Tab(1, xorstr("Loot Visuals"), &esptabs, 65);

				ImGui::Separator(8);

				if (esptabs == 0) {
					ImGui::Columns(2);
					ImGui::Text(xorstr("Players"));
					ImGui::Checkbox(xorstr("Box ESP"), &Settings::Box);
					if (Settings::Box) {
						ImGui::Combo(xorstr("Box ESP Type"), &Settings::BoxTypeSelected, ESP_Box_TypeItems, sizeof(ESP_Box_TypeItems) / sizeof(*ESP_Box_TypeItems));
					}
					ImGui::Checkbox(xorstr("Skeleton ESP"), &Settings::Skeleton);
					ImGui::Checkbox(xorstr("Distance ESP"), &Settings::DistanceESP);
					ImGui::Checkbox(xorstr("Snaplines"), &Settings::SnapLines);
					ImGui::SliderInt(xorstr("ESP Max Distance"), &Settings::MaxESPDistance, 10, 500);

					ImGui::NextColumn();

					ImGui::Text(xorstr("Players Colors"));
					ImGui::ColorEdit4(xorstr("##BoxESPvis"), SettingsColor::Box_float, ImGuiColorEditFlags_NoInputs); ImGui::SameLine(); ImGui::ColorEdit4(xorstr("##BoxESPnotvis"), SettingsColor::Box_notvisible_float, ImGuiColorEditFlags_NoInputs); ImGui::SameLine(); ImGui::Text("Box Visible / Not Visible");
					ImGui::ColorEdit4(xorstr("##SkeletonESPvis"), SettingsColor::Skeleton_float, ImGuiColorEditFlags_NoInputs); ImGui::SameLine(); ImGui::ColorEdit4(xorstr("##SkeletonESPnotvis"), SettingsColor::Skeleton_notvisible_float, ImGuiColorEditFlags_NoInputs); ImGui::SameLine(); ImGui::Text("Skeleton Visible / Not Visible");
					ImGui::ColorEdit4(xorstr("##DistanceESPvis"), SettingsColor::Distance_float, ImGuiColorEditFlags_NoInputs); ImGui::SameLine(); ImGui::ColorEdit4(xorstr("##DistanceESPnotvis"), SettingsColor::Distance_notvisible_float, ImGuiColorEditFlags_NoInputs); ImGui::SameLine(); ImGui::Text("Distance Visible / Not Visible");
					ImGui::ColorEdit4(xorstr("##Snaplinesvis"), SettingsColor::Snaplines_float, ImGuiColorEditFlags_NoInputs); ImGui::SameLine(); ImGui::ColorEdit4(xorstr("##Snaplinesnotvis"), SettingsColor::Snaplines_notvisible_float, ImGuiColorEditFlags_NoInputs); ImGui::SameLine(); ImGui::Text("Snaplines Visible / Not Visible");
	

					ImGui::NextColumn();
				}
				else if (esptabs == 1) {
					ImGui::Columns(2);
					ImGui::Text(xorstr("Loot"));
					ImGui::Checkbox(xorstr("Chest ESP"), &Settings::ChestESP);
					ImGui::Checkbox(xorstr("AmmoBox ESP"), &Settings::AmmoBoxESP);
					ImGui::Checkbox(xorstr("Loot ESP"), &Settings::LootESP);
					ImGui::Checkbox(xorstr("LLama ESP"), &Settings::LLamaESP);
					ImGui::Checkbox(xorstr("Vehicle ESP"), &Settings::VehiclesESP);

					ImGui::NextColumn();

					ImGui::Text(xorstr("Loot Colors"));
					ImGui::ColorEdit4(xorstr("Chest ESP"), SettingsColor::ChestESP_float, ImGuiColorEditFlags_NoInputs);
					ImGui::ColorEdit4(xorstr("AmmoBox ESP"), SettingsColor::AmmoBox_float, ImGuiColorEditFlags_NoInputs);
					ImGui::ColorEdit4(xorstr("Loot ESP"), SettingsColor::LootESP_float, ImGuiColorEditFlags_NoInputs);
					ImGui::ColorEdit4(xorstr("LLama ESP"), SettingsColor::LLamaESP_float, ImGuiColorEditFlags_NoInputs);
					ImGui::ColorEdit4(xorstr("Vehicle ESP"), SettingsColor::VehicleESP_float, ImGuiColorEditFlags_NoInputs);



					ImGui::NextColumn();
				}




			}
			else if (maintabs == 2) {
				ImGui::Text(xorstr("Mods"));
				ImGui::Checkbox(xorstr("No Spread [RISK]"), &Settings::NoSpread);
				ImGui::SameLine();
				ImGui::TextColored(ImColor(255, 231, 94, 255), xorstr("[?]"));
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text(xorstr("Makes your bullets not spread"));
					ImGui::EndTooltip();
				}
				ImGui::Checkbox(xorstr("Instant Revive"), &Settings::InstantRevive);
				ImGui::SameLine();
				ImGui::TextColored(ImColor(255, 231, 94, 255), xorstr("[?]"));
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text(xorstr("You can instant revive your team"));
					ImGui::EndTooltip();
				}
				
				ImGui::Checkbox(xorstr("Aim While Jumping"), &Settings::AimWhileJumping);

				// bullettp
				ImGui::Checkbox(xorstr("BulletTP [Only in vehicle!]"), &Settings::Bullettp);

				// Speed hack

				ImGui::Checkbox(xorstr("vehicle boost (speedhack) [SHIFT]##checkbox"), &Settings::Speed);
				if (Settings::Speed) {
					ImGui::SliderFloat(xorstr("speed hack value##slider"), &Settings::SpeedValue, 2.0, 100.0);
				}

			}
			else if (maintabs == 3) {

				ImGui::Text(xorstr("Misc"));
				ImGui::Checkbox(xorstr("Show Fov Circle"), &Settings::ShowFovCircle);
				ImGui::Checkbox(xorstr("Crosshair"), &Settings::crosshair);
				ImGui::Checkbox(xorstr("Visible Check"), &Settings::VisibleCheck);
				ImGui::Checkbox(xorstr("Rainbow Mode"), &Settings::Gaybow);


			}




			ImGui::End();
		}
	}

	// Rainbow renders

	auto RGB = ImGui::GetColorU32({ color_red, color_green, color_blue, 255 });

	if (Settings::Gaybow) {

		SettingsColor::FovCircle = RGB;

		SettingsColor::Box = RGB;
		SettingsColor::Skeleton = RGB;
		SettingsColor::Distance = RGB;
		SettingsColor::Snaplines = RGB;

		SettingsColor::Box_notvisible = RGB;
		SettingsColor::Skeleton_notvisible = RGB;
		SettingsColor::Distance_notvisible = RGB;
		SettingsColor::Snaplines_notvisible = RGB;



		//LootESP colors
		SettingsColor::ChestESP = ImColor(SettingsColor::ChestESP_float[0], SettingsColor::ChestESP_float[1], SettingsColor::ChestESP_float[2], SettingsColor::ChestESP_float[3]);
		SettingsColor::AmmoBox = ImColor(SettingsColor::AmmoBox_float[0], SettingsColor::AmmoBox_float[1], SettingsColor::AmmoBox_float[2], SettingsColor::AmmoBox_float[3]);
		SettingsColor::LootESP = ImColor(SettingsColor::LootESP_float[0], SettingsColor::LootESP_float[1], SettingsColor::LootESP_float[2], SettingsColor::LootESP_float[3]);
		SettingsColor::LLamaESP = ImColor(SettingsColor::LLamaESP_float[0], SettingsColor::LLamaESP_float[1], SettingsColor::LLamaESP_float[2], SettingsColor::LLamaESP_float[3]);
		SettingsColor::VehicleESP = ImColor(SettingsColor::VehicleESP_float[0], SettingsColor::VehicleESP_float[1], SettingsColor::VehicleESP_float[2], SettingsColor::VehicleESP_float[3]);
	}
	else {
		//FovCircle Color
		SettingsColor::FovCircle = ImColor(SettingsColor::FovCircle_float[0], SettingsColor::FovCircle_float[1], SettingsColor::FovCircle_float[2], SettingsColor::FovCircle_float[3]);


		//PlayersESP colors
		SettingsColor::Box = ImColor(SettingsColor::Box_float[0], SettingsColor::Box_float[1], SettingsColor::Box_float[2], SettingsColor::Box_float[3]);
		SettingsColor::Skeleton = ImColor(SettingsColor::Skeleton_float[0], SettingsColor::Skeleton_float[1], SettingsColor::Skeleton_float[2], SettingsColor::Skeleton_float[3]);
		SettingsColor::Distance = ImColor(SettingsColor::Distance_float[0], SettingsColor::Distance_float[1], SettingsColor::Distance_float[2], SettingsColor::Distance_float[3]);
		SettingsColor::Snaplines = ImColor(SettingsColor::Snaplines_float[0], SettingsColor::Snaplines_float[1], SettingsColor::Snaplines_float[2], SettingsColor::Snaplines_float[3]);

		SettingsColor::Box_notvisible = ImColor(SettingsColor::Box_notvisible_float[0], SettingsColor::Box_notvisible_float[1], SettingsColor::Box_notvisible_float[2], SettingsColor::Box_notvisible_float[3]);
		SettingsColor::Skeleton_notvisible = ImColor(SettingsColor::Skeleton_notvisible_float[0], SettingsColor::Skeleton_notvisible_float[1], SettingsColor::Skeleton_notvisible_float[2], SettingsColor::Skeleton_notvisible_float[3]);
		SettingsColor::Distance_notvisible = ImColor(SettingsColor::Distance_notvisible_float[0], SettingsColor::Distance_notvisible_float[1], SettingsColor::Distance_notvisible_float[2], SettingsColor::Distance_notvisible_float[3]);
		SettingsColor::Snaplines_notvisible = ImColor(SettingsColor::Snaplines_notvisible_float[0], SettingsColor::Snaplines_notvisible_float[1], SettingsColor::Snaplines_notvisible_float[2], SettingsColor::Snaplines_notvisible_float[3]);



		//LootESP colors
		SettingsColor::ChestESP = ImColor(SettingsColor::ChestESP_float[0], SettingsColor::ChestESP_float[1], SettingsColor::ChestESP_float[2], SettingsColor::ChestESP_float[3]);
		SettingsColor::AmmoBox = ImColor(SettingsColor::AmmoBox_float[0], SettingsColor::AmmoBox_float[1], SettingsColor::AmmoBox_float[2], SettingsColor::AmmoBox_float[3]);
		SettingsColor::LootESP = ImColor(SettingsColor::LootESP_float[0], SettingsColor::LootESP_float[1], SettingsColor::LootESP_float[2], SettingsColor::LootESP_float[3]);
		SettingsColor::LLamaESP = ImColor(SettingsColor::LLamaESP_float[0], SettingsColor::LLamaESP_float[1], SettingsColor::LLamaESP_float[2], SettingsColor::LLamaESP_float[3]);
		SettingsColor::VehicleESP = ImColor(SettingsColor::VehicleESP_float[0], SettingsColor::VehicleESP_float[1], SettingsColor::VehicleESP_float[2], SettingsColor::VehicleESP_float[3]);
	}
	

	ImGui::Render();
}



HRESULT present_hooked(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags)
{
	static float width = 0;
	static float height = 0;
	static HWND hWnd = 0;
	if (!device)
	{

		//MessageBoxA(NULL, "IM INTO THE PRESENT HOOOOKED!!", "", MB_OK);

		swapChain->GetDevice(__uuidof(device), reinterpret_cast<PVOID*>(&device));
		device->GetImmediateContext(&immediateContext);
		//MessageBoxA(NULL, "Before RenderTargetDefine and release", "", MB_OK);
		ID3D11Texture2D* renderTarget = nullptr;
		swapChain->GetBuffer(0, __uuidof(renderTarget), reinterpret_cast<PVOID*>(&renderTarget));
		device->CreateRenderTargetView(renderTarget, nullptr, &renderTargetView);
		renderTarget->Release();
		//MessageBoxA(NULL, "Before backBuffer", "", MB_OK);
		ID3D11Texture2D* backBuffer = 0;
		swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (PVOID*)&backBuffer);
		D3D11_TEXTURE2D_DESC backBufferDesc = { 0 };
		backBuffer->GetDesc(&backBufferDesc);
		//MessageBoxA(NULL, "Before FindWindow", "", MB_OK);
		HWND hWnd = LI_FN(FindWindowA)(xorstr("UnrealWindow"), xorstr("Fortnite  "));
		//if (!width) {
			//oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc)));
			//oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)&WndProc)); // Hook
		//}

		width = (float)backBufferDesc.Width;
		height = (float)backBufferDesc.Height;
		backBuffer->Release();

		ImGui::GetIO().Fonts->AddFontFromFileTTF(xorstr("C:\\Windows\\Fonts\\Tahoma.ttf"), 13.0f);

		ImGui_ImplDX11_Init(hWnd, device, immediateContext);
		ImGui_ImplDX11_CreateDeviceObjects();

	}
	immediateContext->OMSetRenderTargets(1, &renderTargetView, nullptr);
	//MessageBoxA(NULL, "Before CreateScene", "", MB_OK);
	auto& window = CreateScene();

	if (ShowMenu) {
		ImGuiIO& io = ImGui::GetIO();

		POINT p;
		SpoofCall(GetCursorPos, &p);
		io.MousePos.x = p.x;
		io.MousePos.y = p.y;


		//MessageBoxA(NULL, "Before NtGetAsyncKeyState", "", MB_OK);
		if (NtGetAsyncKeyState(VK_LBUTTON)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].y = io.MousePos.y;
		}
		else {
			io.MouseDown[0] = false;
		}
	}

	// Call this one here!
	gaybow(window);
	EntitiyLoop();
	//MessageBoxA(NULL, "Before second NtGetAsyncKeyState", "", MB_OK);
	if (NtGetAsyncKeyState(VK_INSERT) & 1)
	{
		ShowMenu = !ShowMenu;
	}

	//MessageBoxA(NULL, "Before MenuAndDestroy", "", MB_OK);
	MenuAndDestroy(window);
	//MessageBoxA(NULL, "Before TheReturn", "", MB_OK);
	return SpoofCall(present_original, swapChain, syncInterval, flags);
}



HRESULT resize_hooked(IDXGISwapChain* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) {
	ImGui_ImplDX11_Shutdown();
	renderTargetView->Release();
	immediateContext->Release();
	device->Release();
	device = nullptr;

	return SpoofCall(resize_original, swapChain, bufferCount, width, height, newFormat, swapChainFlags);
}




PVOID SpreadCaller = nullptr;
BOOL(*Spread)(PVOID a1, float* a2, float* a3);
BOOL SpreadHook(PVOID a1, float* a2, float* a3)
{
	if (Settings::NoSpread && _ReturnAddress() == SpreadCaller && IsAiming()) {
		return 0;
	}

	return SpoofCall(Spread, a1, a2, a3);
}

float* (*CalculateShot)(PVOID, PVOID, PVOID) = nullptr;

float* CalculateShotHook(PVOID arg0, PVOID arg1, PVOID arg2) {
	auto ret = CalculateShot(arg0, arg1, arg2);

	//  Fixed Silent

	if (ret && Settings::Bullettp || Settings::SilentAim && TargetPawn && LocalPawn)
	{

		Vector3 headvec3;
		SDK::Classes::USkeletalMeshComponent::GetBoneLocation((uintptr_t)TargetPawn, 66, &headvec3);
		SDK::Structs::FVector head = { headvec3.x, headvec3.y , headvec3.z };

		uintptr_t RootComp = read<uintptr_t>(LocalPawn + StaticOffsets::RootComponent);
		Vector3 RootCompLocationvec3 = read<Vector3>(RootComp + StaticOffsets::RelativeLocation);
		SDK::Structs::FVector RootCompLocation = { RootCompLocationvec3.x, RootCompLocationvec3.y , RootCompLocationvec3.z };
		SDK::Structs::FVector* RootCompLocation_check = &RootCompLocation;
		if (!RootCompLocation_check) return ret;
		auto root = RootCompLocation;

		auto dx = head.X - root.X;
		auto dy = head.Y - root.Y;
		auto dz = head.Z - root.Z;

		if (Settings::Bullettp) {
			ret[4] = head.X;
			ret[5] = head.Y;
			ret[6] = head.Z;
			head.Z -= 16.0f;
			root.Z += 45.0f;

			auto y = SpoofRuntime::atan2f_(head.Y - root.Y, head.X - root.X);

			root.X += SpoofRuntime::cosf_(y + 1.5708f) * 32.0f;
			root.Y += SpoofRuntime::sinf_(y + 1.5708f) * 32.0f;

			auto length = SpoofRuntime::sqrtf_(SpoofRuntime::powf_(head.X - root.X, 2) + SpoofRuntime::powf_(head.Y - root.Y, 2));
			auto x = -SpoofRuntime::atan2f_(head.Z - root.Z, length);
			y = SpoofRuntime::atan2f_(head.Y - root.Y, head.X - root.X);

			x /= 2.0f;
			y /= 2.0f;

			ret[0] = -(SpoofRuntime::sinf_(x) * SpoofRuntime::sinf_(y));
			ret[1] = SpoofRuntime::sinf_(x) * SpoofRuntime::cosf_(y);
			ret[2] = SpoofRuntime::cosf_(x) * SpoofRuntime::sinf_(y);
			ret[3] = SpoofRuntime::cosf_(x) * SpoofRuntime::cosf_(y);
		}

		if (Settings::SilentAim) {
			if (dx * dx + dy * dy + dz * dz < 125000.0f) {
				ret[4] = head.X;
				ret[5] = head.Y;
				ret[6] = head.Z;
			}
			else {
				head.Z -= 16.0f;
				root.Z += 45.0f;

				auto y = SpoofRuntime::atan2f_(head.Y - root.Y, head.X - root.X);

				root.X += SpoofRuntime::cosf_(y + 1.5708f) * 32.0f;
				root.Y += SpoofRuntime::sinf_(y + 1.5708f) * 32.0f;

				auto length = SpoofRuntime::sqrtf_(SpoofRuntime::powf_(head.X - root.X, 2) + SpoofRuntime::powf_(head.Y - root.Y, 2));
				auto x = -SpoofRuntime::atan2f_(head.Z - root.Z, length);
				y = SpoofRuntime::atan2f_(head.Y - root.Y, head.X - root.X);

				x /= 2.0f;
				y /= 2.0f;

				ret[0] = -(SpoofRuntime::sinf_(x) * SpoofRuntime::sinf_(y));
				ret[1] = SpoofRuntime::sinf_(x) * SpoofRuntime::cosf_(y);
				ret[2] = SpoofRuntime::cosf_(x) * SpoofRuntime::sinf_(y);
				ret[3] = SpoofRuntime::cosf_(x) * SpoofRuntime::cosf_(y);
			}
		}

	}


	return ret;
}


bool HookFunctions::Init(bool NoSpread, bool CalcShot) {
	if (!NoSpreadInitialized) {
		if (NoSpread) {
			auto SpreadAddr = MemoryHelper::PatternScan(xorstr("E8 ? ? ? ? 48 8D 4B 28 E8 ? ? ? ? 48 8B C8"));
			SpreadAddr = RVA(SpreadAddr, 5);
			HookHelper::InsertHook(SpreadAddr, (uintptr_t)SpreadHook, (uintptr_t)&Spread);
			SpreadCaller = (PVOID)(MemoryHelper::PatternScan(xorstr("0F 57 D2 48 8D 4C 24 ? 41 0F 28 CC E8 ? ? ? ? 48 8B 4D B0 0F 28 F0 48 85 C9")));
			NoSpreadInitialized = true;
		}
	}
	if (!CalcShotInitialized) {
		if (CalcShot) {
			auto CalcShotAddr = MemoryHelper::PatternScan(xorstr("48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 44 0F 29 40 ? 44 0F 29 48 ? 44 0F 29 90 ? ? ? ? 44 0F 29 98 ? ? ? ? 44 0F 29 A0 ? ? ? ? 44 0F 29 A8 ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B F1"));
			HookHelper::InsertHook(CalcShotAddr, (uintptr_t)CalculateShotHook, (uintptr_t)&CalculateShot);
			CalcShotInitialized = true;
		}
	}
	return true;
}



bool InitializeHack()
{
	//AllocConsole();
	//freopen("CONIN$", "r", stdin);
	//freopen("CONOUT$", "w", stderr);
	//freopen("CONOUT$", "w", stdout);


	Renderer_Defines::Width = LI_FN(GetSystemMetrics)(SM_CXSCREEN);
	Renderer_Defines::Height = LI_FN(GetSystemMetrics)(SM_CYSCREEN);
	UWorld = MemoryHelper::PatternScan("48 8B 05 ? ? ? ? 4D 8B C2");
	UWorld = RVA(UWorld, 7);

	FreeFn = MemoryHelper::PatternScan(xorstr("48 85 C9 0F 84 ? ? ? ? 53 48 83 EC 20 48 89 7C 24 30 48 8B D9 48 8B 3D ? ? ? ? 48 85 FF 0F 84 ? ? ? ? 48 8B 07 4C 8B 40 30 48 8D 05 ? ? ? ? 4C 3B C0"));
	ProjectWorldToScreen = MemoryHelper::PatternScan(xorstr("E8 ? ? ? ? 41 88 07 48 83 C4 30"));
	ProjectWorldToScreen = RVA(ProjectWorldToScreen, 5);


	LineOfS = MemoryHelper::PatternScan(xorstr("E8 ? ? ? ? 48 8B 0D ? ? ? ? 33 D2 40 8A F8"));
	LineOfS = RVA(LineOfS, 5);

	GetNameByIndex = MemoryHelper::PatternScan(xorstr("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 45 33 F6 48 8B F2 44 39 71 04 0F 85 ? ? ? ? 8B 19 0F B7 FB E8 ? ? ? ? 8B CB 48 8D 54 24"));
	BoneMatrix = MemoryHelper::PatternScan(xorstr("E8 ? ? ? ? 48 8B 47 30 F3 0F 10 45"));
	BoneMatrix = RVA(BoneMatrix, 5);



	//auto SpreadAddr = MemoryHelper::PatternScan(xorstr("E8 ? ? ? ? 48 8D 4B 28 E8 ? ? ? ? 48 8B C8"));
	//SpreadAddr = RVA(SpreadAddr, 5);
	//HookHelper::InsertHook(SpreadAddr, (uintptr_t)SpreadHook, (uintptr_t)&Spread);
	//SpreadCaller = (PVOID)(MemoryHelper::PatternScan(xorstr("0F 57 D2 48 8D 4C 24 ? 41 0F 28 CC E8 ? ? ? ? 48 8B 4D B0 0F 28 F0 48 85 C9")));
	

	// CalculateShot
	//auto CalcShotAddr = MemoryHelper::PatternScan(xorstr("48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 48 FE ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 44 0F 29 40 ? 44 0F 29 48 ? 44 0F 29 90 ? ? ? ? 44 0F 29 98 ? ? ? ? 44 0F 29 A0 ? ? ?"));
	//HookHelper::InsertHook(CalcShotAddr, (uintptr_t)CalculateShotHook, (uintptr_t)&CalculateShot);





	NtGetAsyncKeyState = (LPFN_MBA)LI_FN(GetProcAddress)(LI_FN(GetModuleHandleA)(xorstr("win32u.dll")), xorstr("NtUserGetAsyncKeyState"));


	auto level = D3D_FEATURE_LEVEL_11_0;
	DXGI_SWAP_CHAIN_DESC sd;
	{
		ZeroMemory(&sd, sizeof sd);
		sd.BufferCount = 1;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = LI_FN(FindWindowA)(xorstr("UnrealWindow"), xorstr("Fortnite  "));
		sd.SampleDesc.Count = 1;
		sd.Windowed = TRUE;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	}

	IDXGISwapChain* swap_chain = nullptr;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;

	LI_FN(D3D11CreateDeviceAndSwapChain)(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &level, 1, D3D11_SDK_VERSION, &sd, &swap_chain, &device, nullptr, &context);

	auto* swap_chainvtable = reinterpret_cast<uintptr_t*>(swap_chain);
	swap_chainvtable = reinterpret_cast<uintptr_t*>(swap_chainvtable[0]);

	DWORD old_protect;
	present_original = reinterpret_cast<present_fn>(reinterpret_cast<DWORD_PTR*>(swap_chainvtable[8]));
	LI_FN(VirtualProtect)(swap_chainvtable, 0x1000, PAGE_EXECUTE_READWRITE, &old_protect);
	swap_chainvtable[8] = reinterpret_cast<DWORD_PTR>(present_hooked);
	LI_FN(VirtualProtect)(swap_chainvtable, 0x1000, old_protect, &old_protect);

	DWORD old_protect_resize;
	resize_original = reinterpret_cast<resize_fn>(reinterpret_cast<DWORD_PTR*>(swap_chainvtable[13]));
	LI_FN(VirtualProtect)(swap_chainvtable, 0x1000, PAGE_EXECUTE_READWRITE, &old_protect_resize);
	swap_chainvtable[13] = reinterpret_cast<DWORD_PTR>(resize_hooked);
	LI_FN(VirtualProtect)(swap_chainvtable, 0x1000, old_protect_resize, &old_protect_resize);
	
	// Call this here or somewhere else!
	Gay();
	return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//if (InitMain() == false) { return 0; }
		InitializeHack();
	}
	return TRUE;
}
