#include "Gui.h"
#include "Memory.hpp"

#include <Windows.h>
#include <thread>
#include <iostream>

INT APIENTRY WinMain(HINSTANCE instance, HINSTANCE previousInstance, PSTR arguments, INT commandShow) {
	// Create Console
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	std::cout << "Welcome to Melonternal." << std::endl;

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
	

	// Free Console
	FreeConsole();
	return 0;
}