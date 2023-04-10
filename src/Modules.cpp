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