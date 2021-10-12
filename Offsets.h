#pragma once
#include <Windows.h>

namespace StaticOffsets {
	uintptr_t OwningGameInstance = 0x188;
	uintptr_t LocalPlayers = 0x38;
	uintptr_t PlayerController = 0x30;
	uintptr_t PlayerCameraManager = 0x2B8;
	uintptr_t AcknowledgedPawn = 0x2A0;
	uintptr_t PrimaryPickupItemEntry = 0x2A8;
	uintptr_t ItemDefinition = 0x18;
	uintptr_t DisplayName = 0x88;
	uintptr_t Tier = 0x6C;
	uintptr_t WeaponData = 0x378;
	uintptr_t LastFireTime = 0x9BC;
	uintptr_t LastFireTimeVerified = 0x9C0;
	uintptr_t LastFireAbilityTime = 0xB40;
	uintptr_t CurrentWeapon = 0x600;
	uintptr_t bADSWhileNotOnGround = 0x3AB1;

	uintptr_t Levels = 0x140;
	uintptr_t PersistentLevel = 0x30;
	uintptr_t AActors = 0x98;
	uintptr_t ActorCount = 0xA0;

	uintptr_t RootComponent = 0x130;
	uintptr_t FireStartLoc = 0x878;
	uintptr_t RelativeLocation = 0x11c;
	uintptr_t RelativeRotation = 0x128;
	uintptr_t PlayerState = 0x240;
	uintptr_t Mesh = 0x280;
	uintptr_t TeamIndex = 0xED8;
}
/*
struct AFortWeaponn {
	uintptr_t CurrentWeapon = 0x5F0;
	uintptr_t LastFireTime = 0x9BC;
	uintptr_t LastFireTimeVerified = 0x9C0;
};
struct AFortWeaponn* AFortWeapon = NULL;



struct AFortPlayerPawn {
	uintptr_t ReviveFromDBNOTime = 0x3398;
};
struct AFortPlayerPawn* AFortPlayerPawnAthena = NULL;



struct UObject {
	uintptr_t CustomTimeDilation = 0x98;
	uintptr_t RootComponent = 0x130;
};
struct UObject* AActor = NULL;


struct UActorComponent {
	uintptr_t RelativeLocation = 0x11C;
};
struct UActorComponent* USceneComponent = NULL;
*/

/*
class AFortWeaponn {
public:
	uintptr_t CurrentWeapon = 0x5F0;
	uintptr_t LastFireTime = 0x9BC;
	uintptr_t LastFireTimeVerified = 0x9C0;
};
AFortWeaponn* AFortWeapon;



class AFortPlayerPawn {
public:
	uintptr_t ReviveFromDBNOTime = 0x3398;
};
AFortPlayerPawn* AFortPlayerPawnAthena;



class UObject {
public:
	uintptr_t CustomTimeDilation = 0x98;
	uintptr_t RootComponent = 0x130;
};
UObject* AActor;


class UActorComponent {
public:
	uintptr_t RelativeLocation = 0x11C;
};
UActorComponent* USceneComponent;
*/