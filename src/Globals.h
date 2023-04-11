#pragma once
#include <cstdint>

namespace Globals {
	// Address
	inline HANDLE processHandle;
	inline std::uintptr_t clientAddress = 0;
	inline std::uintptr_t engineAddress = 0;

	// Modules
	inline bool bhop = false;
	inline bool glow = false;
	inline bool radar = false;

	// Modules Settings
	inline bool glowTeam = true;
	inline float glowTeamColor[] = { 0.f, 1.f, 0.f, 1.f };
	inline float glowEnemyColor[] = { 1.f, 0.f, 0.f, 1.f };
}