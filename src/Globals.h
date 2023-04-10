#pragma once
#include <cstdint>

namespace Globals {
	// Address
	inline HANDLE processHandle;
	inline std::uintptr_t clientAddress = 0;

	// Modules
	inline bool bhop = false;
	inline bool glow = false;

	inline float glowColor[] = { 1.f, 0.f, 0.f, 1.f };
}