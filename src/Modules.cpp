#include "Modules.h"
#include "Gui.h"
#include "Globals.h"
#include "Offsets.hpp"
#include "Vector.h"

#include <iostream>
#include <thread>

struct Color {
	uint8_t r{ }, g{ }, b{ };
};

constexpr Vector3 CalculateAngle(const Vector3& localPosition, const Vector3& enemyPosition, const Vector3& viewAngles) noexcept {
	return ((enemyPosition - localPosition).ToAngle() - viewAngles);
}

float CalculateMagnitude(Vector3 v1, Vector3 v2) {
	float dx = v1.x - v2.x;
	float dy = v1.y - v2.y;
	float dz = v1.z - v2.z;

	return std::sqrt(dx * dx + dy * dy + dz * dz);
}

void Modules::MovementThread() noexcept {
	while (Gui::isRunning) {
		const auto localPlayer = Memory::Read<uintptr_t>(Globals::processHandle, Globals::clientAddress + Offsets::signatures::dwLocalPlayer);

		if (localPlayer) {
			const auto health = Memory::Read<std::int32_t>(Globals::processHandle, localPlayer + Offsets::netvars::m_iHealth);
			const auto lifeState = Memory::Read<std::int32_t>(Globals::processHandle, localPlayer + Offsets::netvars::m_lifeState);
			const auto flags = Memory::Read<uintptr_t>(Globals::processHandle, localPlayer + Offsets::netvars::m_fFlags);

			if (GetAsyncKeyState(VK_SPACE) && health > 0 && lifeState == 0 && Globals::bhop) {
				if (flags & (1 << 0)) {
					Memory::Write<uintptr_t>(Globals::processHandle, Globals::clientAddress + Offsets::signatures::dwForceJump, 6);
				}
				else {
					Memory::Write<uintptr_t>(Globals::processHandle, Globals::clientAddress + Offsets::signatures::dwForceJump, 4);
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}


void Modules::VisualThread() noexcept {
	while (Gui::isRunning) {
		const auto localPlayer = Memory::Read<uintptr_t>(Globals::processHandle, Globals::clientAddress + Offsets::signatures::dwLocalPlayer);

		if (localPlayer) {
			const auto glowObjectManager = Memory::Read<uintptr_t>(Globals::processHandle, Globals::clientAddress + Offsets::signatures::dwGlowObjectManager);

			for (int i = 1; i <= 64; ++i) {
				const auto entity = Memory::Read<uintptr_t>(Globals::processHandle, Globals::clientAddress + Offsets::signatures::dwEntityList + i * 0x10);

				if (entity) {
					const auto entityHealth = Memory::Read<std::int32_t>(Globals::processHandle, entity + Offsets::netvars::m_iHealth);
					const auto entityLifeState = Memory::Read<std::int32_t>(Globals::processHandle, entity + Offsets::netvars::m_lifeState);
						
					if (entityHealth > 0 && entityLifeState == 0) {
						const auto localTeam = Memory::Read<std::int32_t>(Globals::processHandle, localPlayer + Offsets::netvars::m_iTeamNum);
						const auto entityTeam = Memory::Read<std::int32_t>(Globals::processHandle, entity + Offsets::netvars::m_iTeamNum);
						const auto glowIndex = Memory::Read<std::int32_t>(Globals::processHandle, entity + Offsets::netvars::m_iGlowIndex);

						if (entityTeam != localTeam) {
							if (Globals::glow) {
								Memory::Write<float>(Globals::processHandle, glowObjectManager + (glowIndex * 0x38) + 0x8, Globals::glowEnemyColor[0]); // Red
								Memory::Write<float>(Globals::processHandle, glowObjectManager + (glowIndex * 0x38) + 0xC, Globals::glowEnemyColor[1]); // Green
								Memory::Write<float>(Globals::processHandle, glowObjectManager + (glowIndex * 0x38) + 0x10, Globals::glowEnemyColor[2]); // Blue
								Memory::Write<float>(Globals::processHandle, glowObjectManager + (glowIndex * 0x38) + 0x14, Globals::glowEnemyColor[3]); // Alpha

								Memory::Write<bool>(Globals::processHandle, glowObjectManager + (glowIndex * 0x38) + 0x28, true);
								Memory::Write<bool>(Globals::processHandle, glowObjectManager + (glowIndex * 0x38) + 0x29, false);
							}

							if (Globals::radar) {
								Memory::Write<bool>(Globals::processHandle, entity + Offsets::netvars::m_bSpotted, true);
							}

							if (Globals::chams) {
								int r, g, b;

								r = int(Globals::chamsEnemyColor[0] * 255.0f + 0.5f);
								g = int(Globals::chamsEnemyColor[1] * 255.0f + 0.5f);
								b = int(Globals::chamsEnemyColor[2] * 255.0f + 0.5f);

								auto color = Color{ uint8_t(r), uint8_t(g), uint8_t(b) };
								Memory::Write<Color>(Globals::processHandle, entity + Offsets::netvars::m_clrRender, color);
							}
						}
						else {
							if (Globals::glow && Globals::glowTeam) {
								Memory::Write<float>(Globals::processHandle, glowObjectManager + (glowIndex * 0x38) + 0x8, Globals::glowTeamColor[0]); // Red
								Memory::Write<float>(Globals::processHandle, glowObjectManager + (glowIndex * 0x38) + 0xC, Globals::glowTeamColor[1]); // Green
								Memory::Write<float>(Globals::processHandle, glowObjectManager + (glowIndex * 0x38) + 0x10, Globals::glowTeamColor[2]); // Blue
								Memory::Write<float>(Globals::processHandle, glowObjectManager + (glowIndex * 0x38) + 0x14, Globals::glowTeamColor[3]); // Alpha

								Memory::Write<bool>(Globals::processHandle, glowObjectManager + (glowIndex * 0x38) + 0x28, true);
								Memory::Write<bool>(Globals::processHandle, glowObjectManager + (glowIndex * 0x38) + 0x29, false);
							}

							if (Globals::chams) {
								int r, g, b;

								r = int(Globals::chamsTeamColor[0] * 255.0f + 0.5f);
								g = int(Globals::chamsTeamColor[1] * 255.0f + 0.5f);
								b = int(Globals::chamsTeamColor[2] * 255.0f + 0.5f);

								auto color = Color{ uint8_t(r), uint8_t(g), uint8_t(b) };
								Memory::Write<Color>(Globals::processHandle, entity + Offsets::netvars::m_clrRender, color);
							}
						}

						if (Globals::chams) {
							float brightness = 30.f;
							const auto _this = static_cast<uintptr_t>(Globals::engineAddress + Offsets::signatures::model_ambient_min - 0x2C);
							Memory::Write<int32_t>(Globals::processHandle, Globals::engineAddress + Offsets::signatures::model_ambient_min, *reinterpret_cast<uintptr_t*>(&brightness) ^ _this);
						}
					}
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void Modules::CombatThread() noexcept {
	while (Gui::isRunning) {
		const auto localPlayer = Memory::Read<uintptr_t>(Globals::processHandle, Globals::clientAddress + Offsets::signatures::dwLocalPlayer);

		if (localPlayer) {
			const auto localTeam = Memory::Read<std::int32_t>(Globals::processHandle, localPlayer + Offsets::netvars::m_iTeamNum);

			if (GetAsyncKeyState(VK_RBUTTON) && Globals::aimbot) {
				const auto& clientState = Memory::Read<uintptr_t>(Globals::processHandle, Globals::engineAddress + Offsets::signatures::dwClientState);

				const auto localEyePosition = Memory::Read<Vector3>(Globals::processHandle, localPlayer + Offsets::netvars::m_vecOrigin)
					+ Memory::Read<Vector3>(Globals::processHandle, localPlayer + Offsets::netvars::m_vecViewOffset);

				const auto& viewAngles = Memory::Read<Vector3>(Globals::processHandle, clientState + Offsets::signatures::dwClientState_ViewAngles);
				const auto& aimPunch = Memory::Read<Vector3>(Globals::processHandle, localPlayer + Offsets::netvars::m_aimPunchAngle) * 2;

				auto bestFov = 999999.f;
				auto bestDist = 99999.f;
				auto bestAngle = Vector3{};

				for (int i = 1; i <= 64; ++i) {
					const auto entity = Memory::Read<uintptr_t>(Globals::processHandle, Globals::clientAddress + Offsets::signatures::dwEntityList + i * 0x10);

					if (entity) {
						const auto entityTeam = Memory::Read<std::int32_t>(Globals::processHandle, entity + Offsets::netvars::m_iTeamNum);
						const auto entityHealth = Memory::Read<std::int32_t>(Globals::processHandle, entity + Offsets::netvars::m_iHealth);
						const auto entityLifeState = Memory::Read<std::int32_t>(Globals::processHandle, entity + Offsets::netvars::m_lifeState);
						const auto entitySpottedByMask = Memory::Read<bool>(Globals::processHandle, entity + Offsets::netvars::m_bSpottedByMask);
						const auto entityDormant = Memory::Read<bool>(Globals::processHandle, entity + Offsets::signatures::m_bDormant);

						if (entityTeam != localTeam && entityHealth > 0 && entityLifeState == 0) {
							const auto boneMatrix = Memory::Read<uintptr_t>(Globals::processHandle, entity + Offsets::netvars::m_dwBoneMatrix);

							const int aimPart = 8;

							const auto entityPosition = Vector3{
								Memory::Read<float>(Globals::processHandle, boneMatrix + 0x30 * aimPart + 0x0C), // X
								Memory::Read<float>(Globals::processHandle, boneMatrix + 0x30 * aimPart + 0x1C), // Y
								Memory::Read<float>(Globals::processHandle, boneMatrix + 0x30 * aimPart + 0x2C), // Z
							};

							const auto angle = CalculateAngle(localEyePosition, entityPosition, viewAngles + aimPunch);
							const auto fov = std::hypot(angle.x, angle.y);

							if (fov < bestFov) {
								bestFov = fov;
								bestAngle = angle;
							}
						}
					}
				}

				if (!bestAngle.IsZero())
					Memory::Write<Vector3>(Globals::processHandle, clientState + Offsets::signatures::dwClientState_ViewAngles, bestAngle);
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}