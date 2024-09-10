#include "DebugCamera.h"
#include "Input.h"
#include "ImGuiManager.h"

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
#ifdef _DEBUG
	ImGui::Begin("DebugCamera");
	if (ImGui::TreeNode("Transform")) {
		ImGui::DragFloat3("Translate", &translate_.x, 0.1f);
		ImGui::DragFloat3("Rotate", &rotation_.x, 0.1f);
		ImGui::TreePop();
	}

	ImGui::End();
#endif // _DEBUG

	return true;
}
