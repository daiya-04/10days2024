#pragma once
#include <cmath>
#include <algorithm>

class Vector3 {
public:
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
public:

	inline Vector3(){}
	inline Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
	inline Vector3(const Vector3& vector) : x(vector.x), y(vector.y), z(vector.z) {}

	inline Vector3 operator-() const { return{ -x,-y,-z }; }

	inline Vector3& operator+=(const Vector3& vector) {
		x += vector.x;
		y += vector.y;
		z += vector.z;
		return *this;
	}

	inline Vector3& operator-=(const Vector3& vector) {
		x -= vector.x;
		y -= vector.y;
		z -= vector.z;
		return *this;
	}

	inline Vector3& operator*=(const float scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	inline Vector3& operator/=(const Vector3& vector) {
		x /= vector.x;
		y /= vector.y;
		z /= vector.z;
		return *this;
	}
	
	friend inline Vector3 operator+(const Vector3& v1, const Vector3& v2) {
		return { v1.x + v2.x,v1.y + v2.y,v1.z + v2.z };
	}

	friend inline Vector3 operator-(const Vector3& v1, const Vector3& v2) {
		return { v1.x - v2.x,v1.y - v2.y,v1.z - v2.z };
	}

	friend inline Vector3 operator*(const Vector3& vector, const float scalar) {
		return { vector.x * scalar,vector.y * scalar,vector.z * scalar };
	}

	friend inline Vector3 operator*(const float scalar, const Vector3& vector) {
		return { scalar * vector.x,scalar * vector.y,scalar * vector.z };
	}

	friend inline Vector3 operator/(const Vector3& vector, const float scalar) {
		return { vector.x / scalar,vector.y / scalar,vector.z / scalar };
	}

	friend inline bool operator==(const Vector3& v1, const Vector3& v2) {
		return { v1.x == v2.x && v1.y == v2.y && v1.z == v2.z };
	}

	friend inline bool operator!=(const Vector3& v1, const Vector3& v2) {
		return !(v1 == v2);
	}

	friend inline float Dot(const Vector3& v1, const Vector3& v2) {
		return { v1.x * v2.x + v1.y * v2.y + v1.z * v2.z };
	}

	friend inline Vector3 Cross(const Vector3& v1, const Vector3& v2) {
		return { v1.y * v2.z - v1.z * v2.y,
			     v1.z * v2.x - v1.x * v2.z,
			     v1.x * v2.y - v1.y * v2.x };
	}

	inline float Length() const {
		return sqrtf(x * x + y * y + z * z);
	}

	inline Vector3 Normalize() const {
		return *this / Length();
	}

	friend inline Vector3 Lerp(float t, const Vector3& start, const Vector3& end) {
		t = std::clamp(t, 0.0f, 1.0f);
		return { (1.0f - t) * start + t * end };
	}

	friend inline Vector3 Project(const Vector3& v1, const Vector3& v2) {
		float t = Dot(v1, v2) / std::powf(v2.Length(), 2);

		t = std::clamp(t, 0.0f, 1.0f);

		return t * v2;
	}

	inline Vector3 Perpendicular() const {
		if (x != 0.0f || y != 0.0f) {
			return { -y,x,0.0f };
		}
		return { 0.0f,-z,y };
	}

	friend inline Vector3 Bazier(const Vector3& p0, const Vector3& p1, const Vector3& p2, float t) {
		Vector3 p0p1 = Lerp(t, p0, p1);
		Vector3 p1p2 = Lerp(t, p1, p2);
		Vector3 p = Lerp(t, p0p1, p1p2);

		return p;
	}

	friend inline Vector3 CatmullRom(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) {
		Vector3 result{};
		result =
			((p0 * -1.0f + p1 * 3.0f + p2 * -3.0f + p3) * std::powf(t, 3.0f) +
				(p0 * 2.0f + p1 * -5.0f + p2 * 4.0f + p3 * -1.0f) * std::powf(t, 2.0f) +
				(p0 * -1.0f + p2) * t + p1 * 2.0f) * 0.5f;

		return result;
	}

	// 2つの
	friend inline float GetRadianVectorToVector(const Vector3& vec1, const Vector3& vec2) {
		float dot = Dot(vec1, vec2);
		float costheta = dot / (vec1.Length() * vec2.Length());

		return std::acosf(costheta);
	}

};