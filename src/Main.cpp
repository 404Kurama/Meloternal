#include "Gui.h"
#include "Modules.h"
#include "Globals.h"

#include <Windows.h>
#include <thread>
#include <iostream>

INT APIENTRY WinMain(HINSTANCE instance, HINSTANCE previousInstance, PSTR arguments, INT commandShow) {
	// Create Console
	AllocConsole();
	freopen_s((FILE**)stdout, "CONIN&", "r", stdin);
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	freopen_s((FILE**)stdout, "CONOUT$", "w", stderr);

	std::cout << "[X] -->> Welcome to Melonternal <<-- [X]" << std::endl;

	// Getting Game Process
	DWORD processId = Memory::GetProcessId(L"csgo.exe");

	if (!processId) {
		std::cout << "Waiting for CS:GO..." << std::endl;

		while (!processId) {
			processId = Memory::GetProcessId(L"csgo.exe");
			Sleep(200UL);
		}
	}

	DWORD baseAddress = Memory::GetModuleAddress(processId, L"csgo.exe");
	DWORD client = Memory::GetModuleAddress(processId, L"client.dll");
	DWORD engine = Memory::GetModuleAddress(processId, L"engine.dll");

	if (client == 0 || baseAddress == 0 || engine == 0) {
		std::cout << "Waiting for CS:GO to load..." << std::endl;

		while (client == 0 || baseAddress == 0 || engine == 0) {
			baseAddress = Memory::GetModuleAddress(processId, L"csgo.exe");
			client = Memory::GetModuleAddress(processId, L"client.dll");
			engine = Memory::GetModuleAddress(processId, L"engine.dll");
			Sleep(200UL);
		}
	}

	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

	if (!handle) {
		std::cout << "Couldn't get CS:GO handle." << std::endl;
		system("pause");
		return 0;
	}

	Globals::processHandle = handle;
	Globals::clientAddress = client;
	Globals::engineAddress = engine;

	// Printing Info
	std::cout << "----------------------------------------" << std::endl;
	std::cout << "Base Process Id: " << processId << std::endl;
	std::cout << "Base Address: " << baseAddress << std::endl;
	std::cout << "Client Address: " << client << std::endl;
	std::cout << "----------------------------------------" << std::endl;

	std::cout << "Found game process minimize console in 3s" << std::endl;
	Sleep(3000UL);
	ShowWindow(GetConsoleWindow(), SW_MINIMIZE);

	// Modules
	std::thread(Modules::MovementThread).detach();
	std::thread(Modules::VisualThread).detach();

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