#pragma once

bool LootESP(ImDrawList* Renderer, const char* name, uintptr_t CurrentActor, Vector3 LocalRelativeLocation) {

	if (SDK::Utils::CheckItemInScreen(CurrentActor, Renderer_Defines::Width, Renderer_Defines::Height)) {

		uintptr_t ItemRootComponent = read<uintptr_t>(CurrentActor + StaticOffsets::RootComponent);
		Vector3 ItemPosition = read<Vector3>(ItemRootComponent + StaticOffsets::RelativeLocation);
		float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;

		std::string null = xorstr("");


		if (Settings::ChestESP && strstr(name, xorstr("Tiered_Chest"))) {


			if (ItemDist < Settings::MaxESPDistance) {
				Vector3 ChestPosition;
				SDK::Classes::AController::WorldToScreen(ItemPosition, &ChestPosition);

				std::string Text = null + xorstr("Chest [") + std::to_string((int)ItemDist) + xorstr("m]");

				ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());


				Renderer->AddText(ImVec2(ChestPosition.x - TextSize.x / 2, ChestPosition.y - TextSize.y / 2), SettingsColor::ChestESP, Text.c_str());

			}

		}
		else if (Settings::AmmoBoxESP && strstr(name, xorstr("Tiered_Ammo"))) {

			if (ItemDist < Settings::MaxESPDistance) {
				Vector3 AmmoPosition;
				SDK::Classes::AController::WorldToScreen(ItemPosition, &AmmoPosition);

				std::string Text = null + xorstr("AB [") + std::to_string((int)ItemDist) + xorstr("m]");

				ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());


				Renderer->AddText(ImVec2(AmmoPosition.x - TextSize.x / 2, AmmoPosition.y - TextSize.y / 2), SettingsColor::AmmoBox, Text.c_str());

			}

		}
		else if (strstr(name, xorstr("FortPickupAthena")) && Settings::LootESP) {


			if (ItemDist < Settings::MaxESPDistance) {
				auto item = read<uintptr_t>(CurrentActor + StaticOffsets::PrimaryPickupItemEntry + StaticOffsets::ItemDefinition);
				if (!item) return false;

				auto itemName = reinterpret_cast<SDK::Structs::FText*>(read<uintptr_t>(item + StaticOffsets::DisplayName));
				if (!itemName || !itemName->c_str()) return false;
				auto isAmmo = strstr(itemName->c_str(), xorstr("Ammo: "));

				CHAR text[0xFF] = { 0 };
				uintptr_t CurGay = read<uintptr_t>(CurrentActor + StaticOffsets::CurrentWeapon);
				uintptr_t CurRootGay = read<uintptr_t>(CurGay + StaticOffsets::WeaponData);
				uintptr_t bWTier = read<uintptr_t>(CurRootGay + StaticOffsets::Tier);
				wcstombs(text, itemName->c_wstr() + (isAmmo ? 6 : 0), sizeof(text));

				Vector3 LootPosition;

				SDK::Classes::AController::WorldToScreen(ItemPosition, &LootPosition);


				std::string Text = null + text + xorstr(" [") + std::to_string((int)ItemDist) + xorstr("m]");

				ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());


				Renderer->AddText(ImVec2(LootPosition.x - TextSize.x / 2, LootPosition.y - TextSize.y / 2), SettingsColor::LootESP, Text.c_str());
			}
		}

	}
}