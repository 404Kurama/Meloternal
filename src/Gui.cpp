#include "Gui.h"
#include "Globals.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx9.h>
#include <imgui/imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter);

long __stdcall WindowProcess(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter) {
	if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
		return true;

	switch (message) {

	case WM_SIZE: {
		if (Gui::device && wideParameter != SIZE_MINIMIZED) {
			Gui::presentParameters.BackBufferWidth = LOWORD(longParameter);
			Gui::presentParameters.BackBufferHeight = HIWORD(longParameter);
			Gui::ResetDevice();
		}
	} return 0;

	case WM_SYSCOMMAND: {
		if ((wideParameter & 0xFFF0) == SC_KEYMENU)
			return 0;
	} break;

	case WM_DESTROY: {
		PostQuitMessage(0);
	} return 0;

	case WM_LBUTTONDOWN: {
		Gui::position = MAKEPOINTS(longParameter);
	} return 0;

	case WM_MOUSEMOVE: {
		if (wideParameter == MK_LBUTTON) {
			const auto points = MAKEPOINTS(longParameter);
			auto rect = ::RECT{};

			GetWindowRect(Gui::window, &rect);

			rect.left += points.x - Gui::position.x;
			rect.top += points.y - Gui::position.y;

			if (Gui::position.x >= 0 && Gui::position.x <= Gui::WIDTH && Gui::position.y >= 0 && Gui::position.y <= 19) {
				SetWindowPos(Gui::window, HWND_TOPMOST, rect.left, rect.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER);
			}
		}
	} return 0;

	}

	return DefWindowProc(window, message, wideParameter, longParameter);
}

// Window Creation & Destruction
void Gui::CreateHWindow(const char* windowName, const char* className) noexcept {
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = WindowProcess;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hIcon = 0;
	windowClass.hIconSm = 0;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = 0;
	windowClass.lpszMenuName = 0;
	windowClass.lpszClassName = className;
	
	RegisterClassEx(&windowClass);

	window = CreateWindowEx(0, className, windowName, WS_POPUP, 100, 100, WIDTH, HEIGHT, 0, 0, windowClass.hInstance, 0);

	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);
}

void Gui::DestroyHWindow() noexcept {
	DestroyWindow(window);
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

// Device Creation & Destruction
bool Gui::CreateDevice() noexcept {
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d)
		return false;

	ZeroMemory(&presentParameters, sizeof(presentParameters));

	presentParameters.Windowed = TRUE;
	presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
	presentParameters.EnableAutoDepthStencil = TRUE;
	presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	
	if (d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &presentParameters, &device) < 0)
		return false;

	return true;
}

void Gui::ResetDevice() noexcept {
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = device->Reset(&presentParameters);

	if (result == D3DERR_INVALIDCALL)
		IM_ASSERT(0);

	ImGui_ImplDX9_CreateDeviceObjects();
}

void Gui::DestroyDevice() noexcept {
	if (device) {
		device->Release();
		device = nullptr;
	}

	if (d3d) {
		d3d->Release();
		d3d = nullptr;
	}
}

// ImGui Creation & Destruction
void Gui::CreateImGui() noexcept {
	IMGUI_CHECKVERSION();

	ImGui::CreateContext();

	ImGuiIO& io = ::ImGui::GetIO();
	io.IniFilename = NULL;

	ImGui::StyleColorsDark();
	
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
}

void Gui::DestroyImGui() noexcept {
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();
}

// Render
void Gui::BeginRender() noexcept {
	MSG message;

	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessage(&message);

		if (message.message == WM_QUIT) {
			isRunning = !isRunning;
			return;
		}
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Gui::EndRender() noexcept {
	ImGui::EndFrame();

	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	if (device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	const auto result = device->Present(0, 0, 0, 0);

	if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		ResetDevice();
}

void Gui::Render() noexcept {
	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize({ WIDTH, HEIGHT });

	ImGui::Begin("Meloternal", &isRunning, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
 
	ImGui::Text(">> Combat Modules <<");
	ImGui::Checkbox("Aimbot", &Globals::aimbot);

	ImGui::Text(">> Movement Modules <<");
	ImGui::Checkbox("BHop", &Globals::bhop);

	ImGui::Text(">> Visuals Modules <<");
	ImGui::Checkbox("Glow", &Globals::glow);
	ImGui::Checkbox("Radar", &Globals::radar);
	ImGui::Checkbox("Chams", &Globals::chams);

	ImGui::Text(">> Visuals Settings <<");
	ImGui::Checkbox("Glow Team", &Globals::glowTeam);
	ImGui::ColorEdit4("Glow Team Color", Globals::glowTeamColor);
	ImGui::ColorEdit4("Glow Enemy Color", Globals::glowEnemyColor);
	ImGui::ColorEdit3("Chams Team Color", Globals::chamsTeamColor);
	ImGui::ColorEdit3("Chams Enemy Color", Globals::chamsEnemyColor);

	ImGui::End();
}