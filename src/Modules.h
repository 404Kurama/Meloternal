#pragma once
#include "Memory.hpp"

namespace Modules {
	void MovementThread() noexcept;
	void VisualThread() noexcept;
	void CombatThread() noexcept;
}