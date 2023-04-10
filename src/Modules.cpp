#include "Modules.h"
#include "Gui.h"
#include "Globals.h"
#include "Offsets.hpp"

#include <iostream>
#include <thread>

void Modules::MovementThread() noexcept {
	while (Gui::isRunning) {
		const auto localPlayer = Memory::Read<uintptr_t>(Globals::processHandle, Globals::clientAddress + Offsets::signatures::dwLocalPlayer);

		if (localPlayer) {
			const auto health = Memory::Read<std::int32_t>(Globals::processHandle, localPlayer + Offsets::netvars::m_iHealth);
			const auto lifeState = Memory::Read<std::int32_t>(Globals::processHandle, localPlayer + Offsets::netvars::m_lifeState);
			const auto flags = Memory::Read<uintptr_t>(Globals::processHandle, localPlayer + Offsets::netvars::m_fFlags);

			if (GetAsyncKeyState(VK_SPACE) && health > 0 && lifeState == 0 && Globals::bhop) {
				if (flags & (1 << 0)) {
					Memory::Write<uintptr_t>(Globals::processHandle, Globals::clientAddress + Offsets::signatures::dwForceJump, 6);
				}
				else {
					Memory::Write<uintptr_t>(Globals::processHandle, Globals::clientAddress + Offsets::signatures::dwForceJump, 4);
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}


void Modules::VisualThread() noexcept {
	while (Gui::isRunning) {
		const auto localPlayer = Memory::Read<uintptr_t>(Globals::processHandle, Globals::clientAddress + Offsets::signatures::dwLocalPlayer);

		if (localPlayer) {
			const auto glowObjectManager = Memory::Read<uintptr_t>(Globals::processHandle, Globals::clientAddress + Offsets::signatures::dwGlowObjectManager);

			for (int i = 1; i <= 64; ++i) {
				const auto entity = Memory::Read<uintptr_t>(Globals::processHandle, Globals::clientAddress + Offsets::signatures::dwEntityList + i * 0x10);

				if (entity) {
					const auto entityHealth = Memory::Read<std::int32_t>(Globals::processHandle, entity + Offsets::netvars::m_iHealth);
					const auto entityLifeState = Memory::Read<std::int32_t>(Globals::processHandle, entity + Offsets::netvars::m_lifeState);
						
					if (entityHealth > 0 && entityLifeState == 0) {
						const auto localTeam = Memory::Read<std::int32_t>(Globals::processHandle, localPlayer + Offsets::netvars::m_iTeamNum);
						const auto entityTeam = Memory::Read<std::int32_t>(Globals::processHandle, entity + Offsets::netvars::m_iTeamNum);
						const auto glowIndex = Memory::Read<std::int32_t>(Globals::processHandle, entity + Offsets::netvars::m_iGlowIndex);

						if (entityTeam != localTeam) {
							if (Globals::glow) {
								Memory::Write<float>(Globals::processHandle, glowObjectManager + (glowIndex * 0x38) + 0x8, Globals::glowEnemyColor[0]); // Red
								Memory::Write<float>(Globals::processHandle, glowObjectManager + (glowIndex * 0x38) + 0xC, Globals::glowEnemyColor[1]); // Green
								Memory::Write<float>(Globals::processHandle, glowObjectManager + (glowIndex * 0x38) + 0x10, Globals::glowEnemyColor[2]); // Blue
								Memory::Write<float>(Globals::processHandle, glowObjectManager + (glowIndex * 0x38) + 0x14, Globals::glowEnemyColor[3]); // Alpha

								Memory::Write<bool>(Globals::processHandle, glowObjectManager + (glowIndex * 0x38) + 0x28, true);
								Memory::Write<bool>(Globals::processHandle, glowObjectManager + (glowIndex * 0x38) + 0x29, false);
							}
						}
						else {
							if (Globals::glow && Globals::glowTeam) {
								Memory::Write<float>(Globals::processHandle, glowObjectManager + (glowIndex * 0x38) + 0x8, Globals::glowTeamColor[0]); // Red
								Memory::Write<float>(Globals::processHandle, glowObjectManager + (glowIndex * 0x38) + 0xC, Globals::glowTeamColor[1]); // Green
								Memory::Write<float>(Globals::processHandle, glowObjectManager + (glowIndex * 0x38) + 0x10, Globals::glowTeamColor[2]); // Blue
								Memory::Write<float>(Globals::processHandle, glowObjectManager + (glowIndex * 0x38) + 0x14, Globals::glowTeamColor[3]); // Alpha

								Memory::Write<bool>(Globals::processHandle, glowObjectManager + (glowIndex * 0x38) + 0x28, true);
								Memory::Write<bool>(Globals::processHandle, glowObjectManager + (glowIndex * 0x38) + 0x29, false);
							}
						}
					}
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}