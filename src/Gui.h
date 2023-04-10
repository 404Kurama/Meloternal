#pragma once
#include <d3d9.h>

namespace Gui {
	// Windows Size
	constexpr int WIDTH = 500;
	constexpr int HEIGHT = 300;

	inline bool isRunning = true;

	// Winapi Window Variables
	inline HWND window = nullptr;
	inline WNDCLASSEXA windowClass = {};

	// Window Movement
	inline POINTS position = {};

	// DirectX State Variable
	inline PDIRECT3D9 d3d = nullptr;
	inline LPDIRECT3DDEVICE9 device = nullptr;
	inline D3DPRESENT_PARAMETERS presentParameters = {};

	// Window Creation & Destruction
	void CreateHWindow(const char* windowName, const char* className) noexcept;
	void DestroyHWindow() noexcept;

	// Device Creation & Destruction
	bool CreateDevice() noexcept;
	void ResetDevice() noexcept;
	void DestroyDevice() noexcept;

	// ImGui Creation & Destruction
	void CreateImGui() noexcept;
	void DestroyImGui() noexcept;

	// Render
	void BeginRender() noexcept;
	void EndRender() noexcept;
	void Render() noexcept;
}