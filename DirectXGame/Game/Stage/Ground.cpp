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

void Ground::Initialize(const LevelData* data, const std::vector<std::shared_ptr<Model>>& models, WorldTransform transform, const std::shared_ptr<Model>& boardModel) {
	transform_ = transform;
	transform_.scale_ = Vector3(1.0f, 1.0f, 1.0f);
	transform_.UpdateMatrix();

	// pieceのtag付けと初期化
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

	// 電光板
	electricBoard_ = std::make_unique<Object3d>();
	electricBoard_->Initialize(boardModel);
	electricBoard_->worldTransform_.parent_ = &transform_;
	electricBoard_->worldTransform_.UpdateMatrix();
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
	// 適当に1度ずつ回転
	electricBoard_->worldTransform_.rotation_.y += 0.01f;
	electricBoard_->worldTransform_.UpdateMatrix();
}

void Ground::Draw(const Camera& camera) {
	for (auto& piece : pieces_) {
		piece->Draw(camera);
	}
	electricBoard_->Draw(camera);
}

void Ground::IsCollision(const float& angle) {

	std::string tag;
	float oneRad = std::numbers::pi_v<float> / 2.0f; // 90度
	// 現在のtag判定
	if (0 <= angle && angle < oneRad) {
		tag = "RightUp";
	}
	else if (oneRad <= angle && angle <= oneRad * 2.0f) {
		tag = "RightDown";
	}
	else if (angle < 0.0f && angle >= -oneRad) {
		tag = "LeftUp";
	}
	else if (angle < -oneRad && angle > -(oneRad * 2.0f)) {
		tag = "LeftDown";
	}
#ifdef _DEBUG
	ImGui::Text("Tag %s", tag.c_str());
#endif // _DEBUG

	for (auto& ground : pieces_) {
		// tagの確認
		if (ground->GetTag() == tag) {
			Vector3 rot = ground->GetRotation();
			// 1つずつ調べている
			float maxRad = rot.y + (oneRad * 0.25f);/*90度を4分割*/
			
			// 左側なので-
			if (tag.find("Left") != std::string::npos) {
				maxRad = rot.y + (oneRad * 0.25f);
				// angleがrotよりも大きく、maxRadよりも小さい
				if (maxRad >= angle && angle > rot.y) {
					ground->OnCollision(1.0f);
					return;
				}
			}
			else if (rot.y <= angle && angle < maxRad) {
				ground->OnCollision(1.0f);
				return;
			}
		}
	}
}
