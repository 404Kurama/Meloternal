#include "Modules.h"
#include "Gui.h"
#include "Globals.h"
#include "Offsets.hpp"

#include <thread>

void Modules::MovementThread() noexcept {
	while (Gui::isRunning) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}