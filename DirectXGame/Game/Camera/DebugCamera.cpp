#include "DebugCamera.h"
#include "Input.h"
#include "ImGuiManager.h"

DebugCamera::DebugCamera() {

}

bool DebugCamera::Update() {
	auto input = Input::GetInstance();

	if (input->TriggerKey(DIK_1) && input->PushKey(DIK_RSHIFT)) {
		isUse_ = !isUse_;
	}
	if (!isUse_) { return false; }
	
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
