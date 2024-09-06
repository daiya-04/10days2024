#pragma once
#include "Camera.h"

class DebugCamera {
public:
	DebugCamera();
	~DebugCamera() = default;

private:
	float moveSpeed_ = 0.01f;
	float rotSpeed_ = 0.2f;
	float offsetSpeed_ = 0.5f;

};
