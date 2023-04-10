#pragma once
#include <Windows.h>
#include <TlHelp32.h>

namespace Memory {
	DWORD GetProcessId(const wchar_t* processName) noexcept {
		DWORD pid = 0;
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if (snapshot) {
			PROCESSENTRY32W process;
			process.dwSize = sizeof(PROCESSENTRY32W);

			while (Process32NextW(snapshot, &process)) {
				if (_wcsicmp(process.szExeFile, processName) == 0) {
					pid = process.th32ProcessID;
					break;
				}
			}

			CloseHandle(snapshot);
		}

		return pid;
	}

	DWORD GetModuleAddress(const DWORD pid, const wchar_t* moduleName) {
		DWORD address = 0;
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);

		if (snapshot) {
			MODULEENTRY32W module;
			module.dwSize = sizeof(MODULEENTRY32W);

			while (Module32NextW(snapshot, &module)) {
				if (_wcsicmp(module.szModule, moduleName) == 0) {
					address = reinterpret_cast<DWORD>(module.modBaseAddr);
					break;
				}
			}

			CloseHandle(snapshot);
		}

		return address;
	}

	template <typename T>
	T Read(const HANDLE process, const DWORD address) noexcept {
		T value = {};
		ReadProcessMemory(process, reinterpret_cast<LPCVOID>(address), &value, sizeof(T), nullptr);
		return value;
	}

	template <typename T>
	T Write(const HANDLE process, const DWORD address, const T& value) noexcept {
		return WriteProcessMemory(process, reinterpret_cast<LPVOID>(address), &value, sizeof(T), nullptr);
	}
}