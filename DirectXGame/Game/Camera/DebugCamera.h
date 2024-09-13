#pragma once
#include "Camera.h"

class DebugCamera {
public:
	DebugCamera();
	~DebugCamera() = default;

	bool Update();
	Vector3 GetCameraTranslate() const {
		return translate_;
	}
	Vector3 GetCameraRotate() const {
		return rotation_;
	}

private:
	float moveSpeed_ = 1.0f;
	float rotSpeed_ = 0.2f;
	float offsetSpeed_ = 0.5f;

	Vector3 translate_;
	Vector3 rotation_;
	bool isUse_ = false;

};
