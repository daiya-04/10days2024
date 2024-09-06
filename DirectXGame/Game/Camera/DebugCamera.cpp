#include "DebugCamera.h"
#include "Input.h"

DebugCamera::DebugCamera() {

}

bool DebugCamera::Update() {
	auto input = Input::GetInstance();

	if (input->TriggerKey(DIK_1)) {
		isUse_ = !isUse_;
	}
	if (!isUse_) { return false; }

	float rad = 1.0f/* * (std::numbers::pi_v<float>)*/;
	bool angle = false;
	
	if (input->PushKey(DIK_LCONTROL)) {
		angle = true;
	}

	if (input->PushKey(DIK_LEFTARROW) && angle) {
		rotation_.y -= rad * rotSpeed_;
	}
	else if (input->PushKey(DIK_LEFTARROW)) {
		translate_.x -= moveSpeed_;
	}

	if (input->PushKey(DIK_RIGHTARROW) && angle) {
		rotation_.y += rad * rotSpeed_;
	}
	else if (input->PushKey(DIK_RIGHTARROW)) {
		translate_.x += moveSpeed_;
	}

	if (input->PushKey(DIK_UPARROW) && angle) {
		rotation_.x += rad * rotSpeed_;
	}
	else if (input->PushKey(DIK_UPARROW)) {
		translate_.y += moveSpeed_;
	}


	if (input->PushKey(DIK_DOWNARROW) && angle) {
		rotation_.x -= rad * rotSpeed_;
	}
	else if (input->PushKey(DIK_DOWNARROW)) {
		translate_.y -= moveSpeed_;
	}

	return true;
}
