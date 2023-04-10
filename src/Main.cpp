#include "Gui.h"

#include <Windows.h>
#include <thread>

INT APIENTRY WinMain(HINSTANCE instance, HINSTANCE previousInstance, PSTR arguments, INT commandShow) {
	// Create Gui
	Gui::CreateHWindow("Meloternal", "Meloternal");
	Gui::CreateDevice();
	Gui::CreateImGui();

	// Render Loop
	while (Gui::isRunning) {
		Gui::BeginRender();
		Gui::Render();
		Gui::EndRender();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	// Destroy Gui
	Gui::DestroyImGui();
	Gui::DestroyDevice();
	Gui::DestroyHWindow();
	
	return 0;
}