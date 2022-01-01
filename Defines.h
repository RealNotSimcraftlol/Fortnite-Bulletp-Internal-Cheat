#pragma once


namespace Renderer_Defines {
	float Width;
	float Height;
}



namespace Settings
{
	//Aim
	static bool Aim = 1;
	static bool AimPrediction = 0;
	static bool MouseAim = 0;
	int aimbone = 66;
	int smooth = 3;
	static bool SilentAim = 0;
	static bool Bullettp = 0;
	int FovCircle_Value = 130;
	
	

	//Player ESP
	static bool Box = 1;
	static int  BoxTypeSelected = 1;
	static bool Skeleton = 1;
	static bool DistanceESP = 1;
	static bool SnapLines = 0;
	
	//Loot ESP
	static bool ChestESP = 0;
	static bool LLamaESP = 0;
	static bool WeaponESP = 0;
	static bool LootESP = 0;
	static bool AmmoBoxESP = 0;
	static bool VehiclesESP = 0;


	//Exploits
	static bool Speed = 0;
	static float SpeedValue = 2.0;
	static bool NoSpread = 0;
	static bool InstantRevive = 0;
	static bool AimWhileJumping = 0;
	static bool AirStuck = 0;
	static bool RapidFire = 0;
	static float RapidFireValue = 0.001f;
	static bool fov360 = 0;
	static bool FirstCamera = 0;
	static bool PlayerFly = 0;
	static bool FOVChanger = 0;
	static float FOV = 88.0f;

	//Misc
	static bool Gaybow = 1;
	static int MaxESPDistance = 300;
	static bool crosshair = 0;
	static bool ShowFovCircle = 1;
	static bool trigger = 0;
	static bool VisibleCheck = 1;
	


}

namespace SettingsColor {
	float FovCircle_float[4] = { 255, 231, 94, 255 };
	ImColor FovCircle = ImColor(255, 231, 94, 255);



	//Players ESP VISIBLE
	float Box_float[4] = { 255, 231, 94, 255 };
	ImColor Box = ImColor(255, 231, 94, 255);

	float Skeleton_float[4] = { 255, 231, 94, 255 };
	ImColor Skeleton = ImColor(255, 231, 94, 255);

	float Distance_float[4] = { 255, 231, 94, 255 };
	ImColor Distance = ImColor(255, 231, 94, 255);

	float Snaplines_float[4] = { 255, 231, 94, 255 };
	ImColor Snaplines = ImColor(255, 231, 94, 255);

	float Box_notvisible_float[4] = { 255, 231, 94, 255 };
	ImColor Box_notvisible = ImColor(255, 231, 94, 255);

	float Skeleton_notvisible_float[4] = { 255, 231, 94, 255 };
	ImColor Skeleton_notvisible = ImColor(255, 231, 94, 255);

	float Distance_notvisible_float[4] = { 255, 231, 94, 255 };
	ImColor Distance_notvisible = ImColor(255, 231, 94, 255);

	float Snaplines_notvisible_float[4] = { 255, 231, 94, 255 };
	ImColor Snaplines_notvisible = ImColor(255, 231, 94, 255);


	//Loot ESP

	float ChestESP_float[4] = { 255, 231, 94, 255 };
	ImColor ChestESP = ImColor(255, 231, 94, 255);

	float AmmoBox_float[4] = { 255, 231, 94, 255 };
	ImColor AmmoBox = ImColor(255, 231, 94, 255);

	float LootESP_float[4] = { 255, 231, 94, 255 };
	ImColor LootESP = ImColor(255, 231, 94, 255);

	float LLamaESP_float[4] = { 255, 231, 94, 255 };
	ImColor LLamaESP = ImColor(255, 231, 94, 255);

	float VehicleESP_float[4] = { 255, 231, 94, 255 };
	ImColor VehicleESP = ImColor(255, 231, 94, 255);
}