#pragma once
#include <numbers>
#include <cmath>

struct Vector3
{
	constexpr Vector3(const float x = 0.f, const float y = 0.f, const float z = 0.f) noexcept : x(x), y(y), z(z) { }

	constexpr const Vector3& operator+(const Vector3& vec) const noexcept {
		return Vector3{ x + vec.x, y + vec.y, z + vec.z };
	}

	constexpr const Vector3& operator-(const Vector3& vec) const noexcept {
		return Vector3{ x - vec.x, y - vec.y, z - vec.z };
	}

	constexpr const Vector3& operator*(const float factor) const noexcept {
		return Vector3{ x * factor, y * factor, z * factor };
	}

	constexpr const Vector3& operator/(const float factor) const noexcept {
		return Vector3{ x / factor, y / factor, z / factor };
	}

	constexpr const bool IsZero() const noexcept {
		return x == 0.f && y == 0.f && z == 0.f;
	}

	constexpr const Vector3& ToAngle() const noexcept {
		float pitch, yaw;
		float hypotenuse = std::hypot(x, y);

		pitch = ((double)atan(z / hypotenuse) * 180.0) / std::numbers::pi_v<float>;
		yaw = ((double)atan(y / x) * 180.0) / std::numbers::pi_v<float>;

		if (x >= 0.0) {
			yaw += 180;
		}

		return Vector3{
			pitch,
			yaw,
			0.f
		};
	}

	float x, y, z;
};