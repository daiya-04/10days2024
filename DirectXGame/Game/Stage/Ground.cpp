#include "Ground.h"
#include "Hit.h"
#include "Engine/DebugText/ImGuiManager.h"
#include "Engine/DebugText/GlobalVariables.h"

void Ground::SetGlobalVariables() {
	GlobalVariables* global = GlobalVariables::GetInstance();
	global->CreateGroup(groupName_);
	
	global->AddItem(groupName_, "EndurancePower", endurancePower_);
	global->AddItem(groupName_, "GroundScale", groundScale_);
}

void Ground::ApplyGlobalVariables() {
	GlobalVariables* global = GlobalVariables::GetInstance();
	
	endurancePower_ = global->GetFloatValue(groupName_, "EndurancePower");
	groundScale_ = global->GetFloatValue(groupName_, "GroundScale");
}

void Ground::Initialize(const LevelData* data, const std::vector<std::shared_ptr<Model>>& models, WorldTransform transform) {
	transform_ = transform;
	transform_.scale_ = Vector3(1.0f, 1.0f, 1.0f);
	transform_.UpdateMatrix();

	uint32_t index = 0u;
	
	for (auto& objectdata : data->objectDatas_) {
		if (objectdata.objectName.find("Ground") != std::string::npos) {
			auto& piece = pieces_.emplace_back(std::make_unique<GroundPiece>());
			std::string tag;
			if (objectdata.objectName.find("右上") != std::string::npos) {
				tag = "RightUp";
			}
			else if (objectdata.objectName.find("右下") != std::string::npos) {
				tag = "RightDown";
			}
			else if (objectdata.objectName.find("左上") != std::string::npos) {
				tag = "LeftUp";
			}
			else if (objectdata.objectName.find("左下") != std::string::npos) {
				tag = "LeftDown";
			}

			piece->Initialize(objectdata, models, tag, &transform_);
		}
	}
}

void Ground::Update() {
#ifdef _DEBUG

	ImGui::Begin("Ground");
	static float mastarScale = 30.0f;
	ImGui::DragFloat("MasterScale", &mastarScale, 0.1f, 0.1f);
	transform_.scale_ = Vector3(mastarScale, mastarScale, mastarScale);
	transform_.UpdateMatrix();

	ImGui::End();
#endif // _DEBUG

	for (auto& piece : pieces_) {
		piece->Update();
	}
}

void Ground::Draw(const Camera& camera) {
	for (auto& piece : pieces_) {
	}
	pieces_.at(0)->Draw(camera);
}

void Ground::IsCollision(const float& angle) {
	// min以上maxより小さいで取っているから、ぴったり90度ならRightDownになる

	std::string tag;
	float oneRad = std::numbers::pi_v<float> / 2.0f; // 90度
	// 現在のtag判定
	if (0 <= angle && angle < oneRad) {
		tag = "RightUp";
	}
	else if (oneRad <= angle && angle < oneRad * 2.0f) {
		tag = "RightDown";
	}
	else if (angle < 0.0f && angle >= -oneRad) {
		tag = "LeftUp";
	}
	else if (angle < -oneRad && angle >= -(oneRad * 2.0f)) {
		tag = "LeftDown";
	}

	ImGui::Text("Tag %s", tag.c_str());

	for (auto& ground : pieces_) {
		// tagの確認
		if (ground->GetTag() == tag) {
			Vector3 rot = ground->GetRotation();
			// 1つずつ調べている
			if (angle <= rot.y && rot.y < angle + angle) {
				ground->OnCollision(1.0f);
				return;
			}
		}
	}
}
