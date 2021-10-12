#pragma once
#define RVA(addr, size) ((uintptr_t)((UINT_PTR)(addr) + *(PINT)((UINT_PTR)(addr) + ((size) - sizeof(INT))) + (size)))
#define M_PI	3.14159265358979323846264338327950288419716939937510
#define _CRT_SECURE_NO_WARNINGS
#define COBJMACROS
#define WIN32_LEAN_AND_MEAN
#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_INDEX_BUFFER 128 * 1024
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT

#define NK_D3D11_IMPLEMENTATION
#define NK_IMPLEMENTATION
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_internal.h"
#include "vector3d.h"
#include "Defines.h"

#include "Offsets.h"
#include "lazyimporter.h"
#include "xor.h"

#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <time.h>
#include <math.h>
#include <D3D11.h>
#include <d3dx11.h>
#include <codecvt>
#include <Psapi.h>
#include <list>

#include <wchar.h>
#include <tchar.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <ostream>
#include <regex>
#include <winuser.h>
#include <WinReg.h>
#include <winternl.h>

#include <TlHelp32.h>
#include <random>
#include <ctime>
#include <urlmon.h>

#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "ntdll.lib")

float FOVAngle;

uintptr_t UWorld;
uintptr_t BoneMatrix;
uintptr_t FreeFn;
uintptr_t ProjectWorldToScreen;
uintptr_t LineOfS;
uintptr_t GetNameByIndex;







Vector3 CamLoc;
Vector3 CamRot;

namespace detail
{
	extern "C" void* _spoofer_stub();

	template <typename Ret, typename... Args>
	static inline auto shellcode_stub_helper(
		const void* shell,
		Args... args
	) -> Ret
	{
		auto fn = (Ret(*)(Args...))(shell);
		return fn(args...);
	}

	template <std::size_t Argc, typename>
	struct argument_remapper
	{
		template<
			typename Ret,
			typename First,
			typename Second,
			typename Third,
			typename Fourth,
			typename... Pack
		>
			static auto do_call(const void* shell, void* shell_param, First first, Second second,
				Third third, Fourth fourth, Pack... pack) -> Ret
		{
			return shellcode_stub_helper< Ret, First, Second, Third, Fourth, void*, void*, Pack... >(shell, first, second, third, fourth, shell_param, nullptr, pack...);
		}
	};

	template <std::size_t Argc>
	struct argument_remapper<Argc, std::enable_if_t<Argc <= 4>>
	{
		template<
			typename Ret,
			typename First = void*,
			typename Second = void*,
			typename Third = void*,
			typename Fourth = void*
		>
			static auto do_call(
				const void* shell,
				void* shell_param,
				First first = First{},
				Second second = Second{},
				Third third = Third{},
				Fourth fourth = Fourth{}
			) -> Ret
		{
			return shellcode_stub_helper<
				Ret,
				First,
				Second,
				Third,
				Fourth,
				void*,
				void*
			>(
				shell,
				first,
				second,
				third,
				fourth,
				shell_param,
				nullptr
				);
		}
	};
}
uintptr_t Iamgayaddr = (uintptr_t)LI_FN(GetModuleHandleA)(xorstr("FortniteClient-Win64-Shipping.exe"));

template <typename Ret, typename... Args>
static inline auto SpoofCall(Ret(*fn)(Args...), Args... args) -> Ret
{
	static const void* jmprbx = nullptr;
	if (!jmprbx) {
		const auto ntdll = reinterpret_cast<const unsigned char*>(Iamgayaddr);
		const auto dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(ntdll);
		const auto nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(ntdll + dos->e_lfanew);
		const auto sections = IMAGE_FIRST_SECTION(nt);
		const auto num_sections = nt->FileHeader.NumberOfSections;

		constexpr char section_name[5]{ '.', 't', 'e', 'x', 't' };
		const auto     section = std::find_if(sections, sections + num_sections, [&](const auto& s) {
			return std::equal(s.Name, s.Name + 5, section_name);
			});

		constexpr unsigned char instr_bytes[2]{ 0xFF, 0x26 };
		const auto              va = ntdll + section->VirtualAddress;
		jmprbx = std::search(va, va + section->Misc.VirtualSize, instr_bytes, instr_bytes + 2);
	}

	struct shell_params
	{
		const void* trampoline;
		void* function;
		void* rdx;
	};

	shell_params p
	{
		jmprbx,
		reinterpret_cast<void*>(fn)
	};

	using mapper = detail::argument_remapper<sizeof...(Args), void>;
	return mapper::template do_call<Ret, Args...>((const void*)&detail::_spoofer_stub, &p, args...);
}

namespace SpoofRuntime {
	inline float acosf_(float x)
	{
		return SpoofCall(acosf, x);
	}


	inline float atan2f_(float x, float y)
	{
		return SpoofCall(atan2f, x, y);
	}

	inline float sqrtf_(float x)
	{
		union { float f; uint32_t i; } z = { x };
		z.i = 0x5f3759df - (z.i >> 1);
		z.f *= (1.5f - (x * 0.5f * z.f * z.f));
		z.i = 0x7EEEEEEE - z.i;
		return z.f;
	}


	double powf_(double x, int y)
	{
		double temp;
		if (y == 0)
			return 1;
		temp = powf_(x, y / 2);
		if ((y % 2) == 0) {
			return temp * temp;
		}
		else {
			if (y > 0)
				return x * temp * temp;
			else
				return (temp * temp) / x;
		}
	}

	inline float cosf_(float x)
	{
		return SpoofCall(cosf, x);
	}

	inline float sinf_(float x)
	{
		return SpoofCall(sinf, x);
	}

}



BOOL valid_pointer(DWORD64 address)
{
	if (!IsBadWritePtr((LPVOID)address, (UINT_PTR)8)) return TRUE;
	else return FALSE;
}

template<typename ReadT>
ReadT read(DWORD_PTR address, const ReadT& def = ReadT())
{
	if (valid_pointer(address)) {
		return *(ReadT*)(address);
	}
}

template<typename WriteT>
bool write(DWORD_PTR address, WriteT value, const WriteT& def = WriteT())
{
	if (valid_pointer(address)) {
		*(WriteT*)(address) = value;
		return true;
	}
	return false;
}

uintptr_t PlayerController;
uintptr_t LocalPawn;




#define get_array_size(array)    (sizeof(array) / sizeof(array[0]))
int random_int(int min, int max)
{
	int range = max - min;
	static bool seed = false;
	if (!seed)
	{
		srand((unsigned int)time(0));
		seed = true;
	}
	return rand() % range + min;
}

float distance(int x1, int y1, int x2, int y2)
{
	return (int)SpoofRuntime::sqrtf_((int)SpoofRuntime::powf_(x2 - x1, 2) +
		(int)SpoofRuntime::powf_(y2 - y1, 2) * 1.0);
}







namespace SDK
{
	namespace Structs
	{
		struct FMatrix
		{
			float M[4][4];
		};
		static FMatrix* myMatrix = new FMatrix();


		typedef struct {
			float X, Y, Z;
		} FVector;
		

		struct FVector2D
		{
			FVector2D() : x(0.f), y(0.f)
			{

			}

			FVector2D(float _x, float _y) : x(_x), y(_y)
			{

			}
			~FVector2D()
			{

			}
			float x, y;
		};

		template<class T>
		struct TArray
		{
			friend struct FString;

		public:
			inline TArray()
			{
				Data = nullptr;
				Count = Max = 0;
			};

			inline int Num() const
			{
				return Count;
			};

			inline T& operator[](int i)
			{
				return Data[i];
			};

			inline const T& operator[](int i) const
			{
				return Data[i];
			};

			inline bool IsValidIndex(int i) const
			{
				return i < Num();
			}

		private:
			T* Data;
			int32_t Count;
			int32_t Max;
		};

		class FText {
		private:
			char _padding_[0x28];
			PWCHAR Name;
			DWORD Length;
		public:
			

			inline PWCHAR c_wstr() {
				return Name;
			}

			inline char* c_str()
			{

				char sBuff[255];
				wcstombs((char*)sBuff, (const wchar_t*)this->Name, sizeof(sBuff));
				return sBuff;
			}
		};

		struct FString : private TArray<wchar_t>
		{
			inline FString()
			{
			};

			FString(const wchar_t* other)
			{
				Max = Count = *other ? std::wcslen(other) + 1 : 0;

				if (Count)
				{
					Data = const_cast<wchar_t*>(other);
				}
			};

			inline bool IsValid() const
			{
				return Data != nullptr;
			}

			inline const wchar_t* c_str() const
			{
				return Data;
			}

			std::string ToString() const
			{
				auto length = std::wcslen(Data);

				std::string str(length, '\0');

				std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);

				return str;
			}
		};
	}

	//Functions
	//typedef bool(__fastcall* WorldToScreen_t)(std::uint64_t PlayerController, Vector3 vWorldPos, Vector3* vScreenPos, char);
	//WorldToScreen_t fnWorldToScreen;

	namespace Classes
	{
		class APlayerCameraManager
		{
		public:
			static float GetFOVAngle(uintptr_t PlayerCameraManager)
			{
				auto GetFOVAngle = reinterpret_cast<float(*)(UINT64, char*)>(*(ULONG_PTR*)(*(ULONG_PTR*)PlayerCameraManager + 0x6D0));
				return SpoofCall(GetFOVAngle, (ULONG_PTR)PlayerCameraManager, (char*)0);
			}

			static Vector3 GetCameraLocation(uintptr_t PlayerCameraManager)
			{
				auto GetCameraLocation = reinterpret_cast<Vector3(*)(UINT64, char*)>(*(ULONG_PTR*)(*(ULONG_PTR*)PlayerCameraManager + 0x718));
				return SpoofCall(GetCameraLocation, (ULONG_PTR)PlayerCameraManager, (char*)0);
			}

			static Vector3 GetCameraRotation(uintptr_t PlayerCameraManager)
			{
				auto GetCameraRotation = reinterpret_cast<Vector3(*)(UINT64, char*)>(*(ULONG_PTR*)(*(ULONG_PTR*)PlayerCameraManager + 0x710));
				return SpoofCall(GetCameraRotation, (ULONG_PTR)PlayerCameraManager, (char*)0);
			}

			
			static BOOLEAN LineOfSightTo(PVOID PlayerController, PVOID Actor, Vector3* ViewPoint) {

				auto LOSTo = reinterpret_cast<bool(__fastcall*)(PVOID PlayerController, PVOID Actor, Vector3 * ViewPoint)>(LineOfS);

				return SpoofCall(LOSTo, PlayerController, Actor, ViewPoint);
			}

			static void FirstPerson(int h) {

				auto ClientSetCameraMode = (*(void(__fastcall**)(uintptr_t Controller, int h))(*(uintptr_t*)PlayerController + 0x940));                
				return SpoofCall(ClientSetCameraMode, (uintptr_t)PlayerController, h);
			}
			
			static bool GetPlayerViewPoint(uintptr_t PlayerController, Vector3* vCameraPos, Vector3* vCameraRot)
			{
				if (!PlayerController) return false;

				static uintptr_t pGetPlayerViewPoint = 0;
				if (!pGetPlayerViewPoint)
				{
					uintptr_t VTable = *(uintptr_t*)PlayerController;
					if (!VTable)  return false;

					pGetPlayerViewPoint = *(uintptr_t*)(VTable + 0x708);
					if (!pGetPlayerViewPoint)  return false;
				}

				auto GetPlayerViewPoint = reinterpret_cast<void(__fastcall*)(uintptr_t, Vector3*, Vector3*)>(pGetPlayerViewPoint);

				SpoofCall(GetPlayerViewPoint, (uintptr_t)PlayerController, vCameraPos, vCameraRot);

				return true;
			}
		};

		class UObject
		{
		public:
			static void FreeObjName(__int64 address)
			{
				auto func = reinterpret_cast<__int64(__fastcall*)(__int64 a1)>(FreeFn);

				SpoofCall(func, address);
			}

			static const char* GetObjectName(uintptr_t Object)
			{
				if (Object == NULL)
					return ("");

				auto fGetObjName = reinterpret_cast<SDK::Structs::FString * (__fastcall*)(int* index, SDK::Structs::FString * res)>(GetNameByIndex);

				//auto heheh = reinterpret_cast<SDK::Structs::FString * (__fastcall*)(int* index, SDK::Structs::FString * res)>(GetNameByIndex);


				int index = *(int*)(Object + 0x18);

				SDK::Structs::FString result;
				SpoofCall(fGetObjName, &index, &result);

				if (result.c_str() == NULL)
					return ("");

				auto result_str = result.ToString();

				if (result.c_str() != NULL)
					FreeObjName((__int64)result.c_str());

				return result_str.c_str();
			}

		};

		class USkeletalMeshComponent
		{
		public:
			static bool GetBoneLocation(uintptr_t CurrentActor, int id, Vector3* out)
			{
				uintptr_t mesh = read<uintptr_t>(CurrentActor + StaticOffsets::Mesh);
				if (!mesh) return false;

				auto fGetBoneMatrix = ((Structs::FMatrix * (__fastcall*)(uintptr_t, Structs::FMatrix*, int))(BoneMatrix));
				SpoofCall(fGetBoneMatrix, mesh, Structs::myMatrix, id);

				out->x = Structs::myMatrix->M[3][0];
				out->y = Structs::myMatrix->M[3][1];
				out->z = Structs::myMatrix->M[3][2];

				return true;
			}

			static Vector3 GetBoneDebug(uintptr_t CurrentActor, int id)
			{
				uintptr_t mesh = read<uintptr_t>(CurrentActor + StaticOffsets::Mesh);
				if (!mesh) return Vector3(0, 0, 0);

				auto fGetBoneMatrix = ((Structs::FMatrix * (__fastcall*)(uintptr_t, Structs::FMatrix*, int))(BoneMatrix));
				SpoofCall(fGetBoneMatrix, mesh, Structs::myMatrix, id);

				Vector3 out;

				out.x = Structs::myMatrix->M[3][0];
				out.y = Structs::myMatrix->M[3][1];
				out.z = Structs::myMatrix->M[3][2];

				return out;
			}

		};



		class AController
		{
		public:
			static bool WorldToScreen(Vector3 WorldLocation, Vector3* out)
			{
				
				//if (!fnWorldToScreen) {
				//	fnWorldToScreen = reinterpret_cast<WorldToScreen_t>(ProjectWorldToScreen);
				//}
				auto WorldToScreen = reinterpret_cast<bool(__fastcall*)(uintptr_t pPlayerController, Vector3 vWorldPos, Vector3 * vScreenPosOut, char)>(ProjectWorldToScreen);

				SpoofCall(WorldToScreen, (uintptr_t)PlayerController, WorldLocation, out, (char)0);

				return true;
			}

			static void SetControlRotation(Vector3 NewRotation, bool bResetCamera = false)
			{
				auto SetControlRotation_ = (*(void(__fastcall**)(uintptr_t Controller, Vector3 NewRotation, bool bResetCamera))(*(uintptr_t*)PlayerController + 0x688));
				SpoofCall(SetControlRotation_, PlayerController, NewRotation, bResetCamera);
			}
		};
	}

	namespace Utils
	{
		double GetCrossDistance(double x1, double y1, double x2, double y2)
		{
			return SpoofRuntime::sqrtf_(SpoofRuntime::powf_((float)(x1 - x2), (float)2) + SpoofRuntime::powf_((float)(y1 - y2), (float)2));
		}

		inline float GetDistLength(Vector3 from, Vector3 to)
		{
			return float(SpoofRuntime::sqrtf_(SpoofRuntime::powf_(to.x - from.x, 2.0) + SpoofRuntime::powf_(to.y - from.y, 2.0) + SpoofRuntime::powf_(to.z - from.z, 2.0)));
		}

		Vector3 AimbotPrediction(float bulletVelocity, float bulletGravity, float targetDistance, Vector3 targetPosition, Vector3 targetVelocity) {
			Vector3 recalculated = targetPosition;
			float gravity = fabs(bulletGravity);
			float time = targetDistance / fabs(bulletVelocity);
			float bulletDrop = (gravity / 250) * time * time;
			recalculated.z += bulletDrop * 120;
			recalculated.x += time * (targetVelocity.x);
			recalculated.y += time * (targetVelocity.y);
			recalculated.z += time * (targetVelocity.z);
			return recalculated;
		}

		bool CheckInScreen(uintptr_t CurrentActor, int Width, int Height) {
			Vector3 Pos;
			Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, 66, &Pos);
			Classes::AController::WorldToScreen(Pos, &Pos);
			if (CurrentActor)
			{
				if (((Pos.x <= 0 or Pos.x > Width) and (Pos.y <= 0 or Pos.y > Height)) or ((Pos.x <= 0 or Pos.x > Width) or (Pos.y <= 0 or Pos.y > Height))) {
					return false;
				}
				else {
					return true;
				}

			}
		}

		bool CheckItemInScreen(uintptr_t CurrentActor, int Width, int Height) {
			Vector3 Pos;
			//MessageBoxA(NULL, "Before RootComponent", "", MB_OK);
			uintptr_t RootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
			//std::cout << "TheOmegaLul: " << RootComponent << "\n";
			//MessageBoxA(NULL, "Before RelativeLocation", "", MB_OK);
			//if (!RootComponent) return false;
			Vector3 RelativeLocation = read<Vector3>(RootComponent + StaticOffsets::RelativeLocation);
			//MessageBoxA(NULL, "Before WorldToScreen", "", MB_OK);
			Classes::AController::WorldToScreen(RelativeLocation, &Pos);
			if (CurrentActor)
			{
				//MessageBoxA(NULL, "Before THE OMEGALUL IF", "", MB_OK);
				if (((Pos.x <= 0 or Pos.x > Width) and (Pos.y <= 0 or Pos.y > Height)) or ((Pos.x <= 0 or Pos.x > Width) or (Pos.y <= 0 or Pos.y > Height))) {
					return false;
				}
				else {
					return true;
				}

			}
		}

		

		bool CheckIfInFOV(uintptr_t CurrentActor, float& max)
		{
			Vector3 Pos;
			Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, Settings::aimbone, &Pos);
			Classes::AController::WorldToScreen(Pos, &Pos);
			if (CurrentActor)
			{
				float Dist = GetCrossDistance(Pos.x, Pos.y, (Renderer_Defines::Width / 2), (Renderer_Defines::Height / 2));

				if (Dist < max)
				{
					float Radius = (Settings::FovCircle_Value * Renderer_Defines::Width / FOVAngle) / 2;

					if (Pos.x <= ((Renderer_Defines::Width / 2) + Radius) &&
						Pos.x >= ((Renderer_Defines::Width / 2) - Radius) &&
						Pos.y <= ((Renderer_Defines::Height / 2) + Radius) &&
						Pos.y >= ((Renderer_Defines::Height / 2) - Radius))
					{
						max = Dist;
						return true;
					}

					return false;
				}
			}

			return false;
		}

		Vector3 CalculateNewRotation(uintptr_t CurrentActor, Vector3 LocalRelativeLocation, bool prediction)
		{
			Vector3 RetVector = { 0,0,0 };

			Vector3 rootHead;
			Vector3 Headbox;
			Classes::USkeletalMeshComponent::GetBoneLocation(CurrentActor, Settings::aimbone, &rootHead);
			Classes::AController::WorldToScreen(Vector3(rootHead.x, rootHead.y, rootHead.z + 20), &Headbox);


			Vector3 calculated;


			if (prediction) {
				float distance = Utils::GetDistLength(LocalRelativeLocation, Headbox) / 250;
				uint64_t CurrentActorRootComponent = read<uint64_t>(CurrentActor + 0x130);
				Vector3 vellocity = read<Vector3>(CurrentActorRootComponent + 0x140);
				Vector3 Predicted = Utils::AimbotPrediction(30000, -504, distance, rootHead, vellocity);

				calculated = Predicted;

			}
			else {
				calculated = rootHead;
			}



			if (calculated.x == 0 && calculated.y == 0) return Vector3(0, 0, 0);

			Vector3 VectorPos = calculated - CamLoc;

			

			float distance = VectorPos.Length();
			
			RetVector.x = -((SpoofRuntime::acosf_(VectorPos.z / distance) * (float)(180.0f / M_PI)) - 90.f);
			RetVector.y = SpoofRuntime::atan2f_(VectorPos.y, VectorPos.x) * (float)(180.0f / M_PI);

			return RetVector;
		}

		Vector3 SmoothAngles(Vector3 rot1, Vector3 rot2)
		{
			Vector3 ret;
			auto currentRotation = rot1;

			ret.x = (rot2.x - rot1.x) / Settings::smooth + rot1.x;
			ret.y = (rot2.y - rot1.y) / Settings::smooth + rot1.y;

			return ret;
		}
	}
}
/*
namespace DiscordHelper
{
	uintptr_t module = (uintptr_t)LI_FN(GetModuleHandleA)(xorstr("DiscordHook64.dll"));

	std::vector<uintptr_t> pCreatedHooksArray;
	bool CreateHook(uintptr_t pOriginal, uintptr_t pHookedFunction, uintptr_t pOriginalCall)
	{
		static uintptr_t addrCreateHook = NULL;

		if (!addrCreateHook)
			addrCreateHook = MemoryHelper::PatternScanW(module, xorstr("41 57 41 56 56 57 55 53 48 83 EC 68 4D 89 C6 49 89 D7"));

		if (!addrCreateHook)
			return false;

		using CreateHook_t = uint64_t(__fastcall*)(LPVOID, LPVOID, LPVOID*);
		auto fnCreateHook = (CreateHook_t)addrCreateHook;

		return SpoofCall(fnCreateHook, (void*)pOriginal, (void*)pHookedFunction, (void**)pOriginalCall) == 0 ? true : false;
	}

	bool EnableHookQue()
	{
		static uintptr_t addrEnableHookQueu = NULL;

		if (!addrEnableHookQueu)
			addrEnableHookQueu = MemoryHelper::PatternScanW(module, xorstr("41 57 41 56 41 55 41 54 56 57 55 53 48 83 EC 38 48 ? ? ? ? ? ? 48 31 E0 48 89 44 24 30 BE 01 00 00 00 31 C0 F0 ? ? ? ? ? ? ? 74 2B"));

		if (!addrEnableHookQueu)
			return false;

		using EnableHookQueu_t = uint64_t(__stdcall*)(VOID);
		auto fnEnableHookQueu = (EnableHookQueu_t)addrEnableHookQueu;

		return SpoofCall(fnEnableHookQueu) == 0 ? true : false;
	}

	short GetAsyncKeyState(int key)
	{
		static uintptr_t GetAsyncKeyState_addr;
		if (!GetAsyncKeyState_addr)
			GetAsyncKeyState_addr = MemoryHelper::PatternScanW(module, xorstr("48 FF ? ? ? ? ? CC CC CC CC CC CC CC CC CC 48 FF ? ? ? ? ? CC CC CC CC CC CC CC CC CC 48 83 EC 28 48 ? ? ? ? ? ? 48 85 C9"));
		if (!GetAsyncKeyState_addr)
			return false;

		auto queuehook = ((short(__fastcall*)(int))(GetAsyncKeyState_addr));
		return SpoofCall(queuehook, key);
	}

	bool EnableHook(uintptr_t pTarget, bool bIsEnabled)
	{
		static uintptr_t addrEnableHook = NULL;

		if (!addrEnableHook)
			addrEnableHook = MemoryHelper::PatternScanW(module, xorstr("41 56 56 57 53 48 83 EC 28 49 89 CE BF 01 00 00 00 31 C0 F0 ? ? ? ? ? ? ? 74"));

		if (!addrEnableHook)
			return false;

		using EnableHook_t = uint64_t(__fastcall*)(LPVOID, bool);
		auto fnEnableHook = (EnableHook_t)addrEnableHook;

		return SpoofCall(fnEnableHook, (void*)pTarget, bIsEnabled) == 0 ? true : false;
	}


	short SetCursorPos(int x, int y)
	{
		static uintptr_t addrSetCursorPos = NULL;

		if (!addrSetCursorPos)
		{
			addrSetCursorPos = MemoryHelper::PatternScanW(module,
				xorstr("8A 05 ? ? ? ? 84 C0 74 12"));
		}

		if (!addrSetCursorPos)
			return false;

		using SetCursorPos_t = short(__fastcall*)(int, int);
		auto fnSetCursorPos = (SetCursorPos_t)addrSetCursorPos;

		return fnSetCursorPos(x, y);
	}

	bool InsertHook(uintptr_t pOriginal, uintptr_t pHookedFunction, uintptr_t pOriginalCall)
	{
		bool bAlreadyCreated = false;
		for (auto _Hook : pCreatedHooksArray)
		{
			if (_Hook == pOriginal)
			{
				bAlreadyCreated = true;
				break;
			}
		}

		if (!bAlreadyCreated)
			bAlreadyCreated = CreateHook(pOriginal, pHookedFunction, pOriginalCall);

		if (bAlreadyCreated)
			if (EnableHook(pOriginal, true))
				if (EnableHookQue())
					return true;

		return false;
	}
}*/


namespace MemoryHelper {



	uintptr_t PatternScanW(uintptr_t pModuleBaseAddress, const char* sSignature, size_t nSelectResultIndex = 0)
	{
		static auto patternToByte = [](const char* pattern)
		{
			auto       bytes = std::vector<int>{};
			const auto start = const_cast<char*>(pattern);
			const auto end = const_cast<char*>(pattern) + SpoofCall(strlen, pattern);

			for (auto current = start; current < end; ++current)
			{
				if (*current == '?')
				{
					++current;
					if (*current == '?')
						++current;
					bytes.push_back(-1);
				}
				else
					bytes.push_back(strtoul(current, &current, 16));
			}
			return bytes;
		};

		const auto dosHeader = (PIMAGE_DOS_HEADER)pModuleBaseAddress;
		const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)pModuleBaseAddress + dosHeader->e_lfanew);

		const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
		auto       patternBytes = patternToByte(sSignature);
		const auto scanBytes = reinterpret_cast<std::uint8_t*>(pModuleBaseAddress);

		const auto s = patternBytes.size();
		const auto d = patternBytes.data();

		size_t nFoundResults = 0;

		for (auto i = 0ul; i < sizeOfImage - s; ++i)
		{
			bool found = true;

			for (auto j = 0ul; j < s; ++j)
			{
				if (scanBytes[i + j] != d[j] && d[j] != -1)
				{
					found = false;
					break;
				}
			}

			if (found)
			{
				if (nSelectResultIndex != 0)
				{
					if (nFoundResults < nSelectResultIndex)
					{
						nFoundResults++;
						found = false;
					}
					else
						return reinterpret_cast<uintptr_t>(&scanBytes[i]);
				}
				else
					return reinterpret_cast<uintptr_t>(&scanBytes[i]);
			}
		}

		return NULL;
	}

	uintptr_t PatternScan(const char* sSignature, size_t nSelectResultIndex = 0)
	{
		static bool bIsSetted = false;

		static MODULEINFO info = { 0 };

		if (!bIsSetted)
		{

			LI_FN(GetModuleInformation)(LI_FN(GetCurrentProcess)(), LI_FN(GetModuleHandleA)(xorstr("FortniteClient-Win64-Shipping.exe")), &info, sizeof(info));
			bIsSetted = true;
		}
		return SpoofCall(PatternScanW, (uintptr_t)info.lpBaseOfDll, sSignature, nSelectResultIndex);
	}
}


namespace HookHelper {

	void* memcpy_(void* _Dst, void const* _Src, size_t _Size)
	{
		auto csrc = (char*)_Src;
		auto cdest = (char*)_Dst;

		for (int i = 0; i < _Size; i++)
		{
			cdest[i] = csrc[i];
		}
		return _Dst;
	}
	
	uintptr_t DiscordModule = (uintptr_t)LI_FN(GetModuleHandleA)(xorstr("DiscordHook64.dll"));
	std::vector<uintptr_t> pCreatedHooksArray;
	bool CreateHook(uintptr_t pOriginal, uintptr_t pHookedFunction, uintptr_t pOriginalCall)
	{
		static uintptr_t addrCreateHook = NULL;

		if (!addrCreateHook)
			addrCreateHook = MemoryHelper::PatternScanW(DiscordModule, xorstr("41 57 41 56 56 57 55 53 48 83 EC 68 4D 89 C6 49 89 D7"));

		if (!addrCreateHook)
			return false;

		using CreateHook_t = uint64_t(__fastcall*)(LPVOID, LPVOID, LPVOID*);
		auto fnCreateHook = (CreateHook_t)addrCreateHook;

		return SpoofCall(fnCreateHook, (void*)pOriginal, (void*)pHookedFunction, (void**)pOriginalCall) == 0 ? true : false;
	}

	bool EnableHookQue()
	{
		static uintptr_t addrEnableHookQueu = NULL;

		if (!addrEnableHookQueu)
			addrEnableHookQueu = MemoryHelper::PatternScanW(DiscordModule, xorstr("41 57 41 56 41 55 41 54 56 57 55 53 48 83 EC 38 48 ? ? ? ? ? ? 48 31 E0 48 89 44 24 30 BE 01 00 00 00 31 C0 F0 ? ? ? ? ? ? ? 74 2B"));

		if (!addrEnableHookQueu)
			return false;

		using EnableHookQueu_t = uint64_t(__stdcall*)(VOID);
		auto fnEnableHookQueu = (EnableHookQueu_t)addrEnableHookQueu;

		return SpoofCall(fnEnableHookQueu) == 0 ? true : false;
	}


	bool EnableHook(uintptr_t pTarget, bool bIsEnabled)
	{
		static uintptr_t addrEnableHook = NULL;

		if (!addrEnableHook)
			addrEnableHook = MemoryHelper::PatternScanW(DiscordModule, xorstr("41 56 56 57 53 48 83 EC 28 49 89 CE BF 01 00 00 00 31 C0 F0 ? ? ? ? ? ? ? 74"));

		if (!addrEnableHook)
			return false;

		using EnableHook_t = uint64_t(__fastcall*)(LPVOID, bool);
		auto fnEnableHook = (EnableHook_t)addrEnableHook;

		return SpoofCall(fnEnableHook, (void*)pTarget, bIsEnabled) == 0 ? true : false;
	}

	bool InsertHook(uintptr_t pOriginal, uintptr_t pHookedFunction, uintptr_t pOriginalCall)
	{
		bool bAlreadyCreated = false;
		for (auto _Hook : pCreatedHooksArray)
		{
			if (_Hook == pOriginal)
			{
				bAlreadyCreated = true;
				break;
			}
		}

		if (!bAlreadyCreated)
			bAlreadyCreated = CreateHook(pOriginal, pHookedFunction, pOriginalCall);

		if (bAlreadyCreated)
			if (EnableHook(pOriginal, true))
				if (EnableHookQue())
					return true;

		return false;
	}


}

