#include "Ground.h"
#include "Hit.h"
#include "Engine/DebugText/ImGuiManager.h"
#include "Engine/DebugText/GlobalVariables.h"
#include "TextureManager.h"

void Ground::SetGlobalVariables() {
	GlobalVariables* global = GlobalVariables::GetInstance();
	global->CreateGroup(groupName_);
	
	global->AddItem(groupName_, "EndurancePower", endurancePower_);
	global->AddItem(groupName_, "GroundScale", groundScale_);
}

void Ground::ApplyGlobalVariables() {
	GlobalVariables* global = GlobalVariables::GetInstance();
	
	endurancePower_ = global->GetIntValue(groupName_, "EndurancePower");
	groundScale_ = global->GetFloatValue(groupName_, "GroundScale");
}

void Ground::Initialize(const LevelData* data, const std::vector<std::shared_ptr<Model>>& models, WorldTransform transform, const std::shared_ptr<Model>& boardModel, const std::string& layer) {
	transform_ = transform;

	transform_.scale_ = Vector3(1.0f, 1.0f, 1.0f) * 38.0f;

	transform_.UpdateMatrix();
	layer_ = layer;

	// pieceのtag付けと初期化
	for (auto& objectdata : data->objectDatas_) {
		if (objectdata.objectName.find("Ground") != std::string::npos) {
			auto& piece = pieces_.emplace_back(std::make_unique<GroundPiece>(models));
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

			piece->Initialize(objectdata, tag, &transform_);
		}
	}

	// 電光板
	electricBoard_ = std::make_unique<Object3d>();
	electricBoard_->Initialize(boardModel);
	electricBoard_->worldTransform_.parent_ = &transform_;
	electricBoard_->worldTransform_.UpdateMatrix();
}

void Ground::Initialize() {
	for (auto& piece : pieces_) {
		piece->Initialize();
	}
	isAlive_ = true;
}

void Ground::Update() {
#ifdef _DEBUG

	ImGui::Begin("Stage");
	if (ImGui::TreeNode(layer_.c_str())) {
		ImGui::DragFloat("MasterScale", &transform_.scale_.x, 0.1f, 0.1f);
		ImGui::TreePop();
	}
	transform_.scale_ = Vector3(transform_.scale_.x, transform_.scale_.x, transform_.scale_.x);

	transform_.UpdateMatrix();

	ImGui::End();
#endif // _DEBUG

	for (auto& piece : pieces_) {
		piece->Update();
	}
	CheckAlive();
	// 適当に1度ずつ回転
	electricBoard_->worldTransform_.rotation_.y += 0.01f;
	electricBoard_->worldTransform_.UpdateMatrix();
}

void Ground::Draw(const Camera& camera) {
	if (!isAlive_) { return; }

	for (auto& piece : pieces_) {
		piece->Draw(camera);
	}
	
	if (layer_ == "Center") {
		uint32_t handle = TextureManager::GetInstance()->FindTextureHandle("CenterElectricBoard");
		electricBoard_->Draw(camera, handle);
	}else if (layer_ == "Down") {
		electricBoard_->Draw(camera);
	}
	else {
		electricBoard_->Draw(camera);
	}
}

bool Ground::IsCollision(const float& angle, const int32_t& damage) {

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
	else if (angle < -oneRad && angle >= -(oneRad * 2.0f)) {
		tag = "LeftDown";
	}

	for (auto& ground : pieces_) {
		// 床が壊れている場合は早期リターン
		if (!ground->GetIsAlive()) { continue; }
		
		// tagの確認
		if (ground->GetTag() == tag) {
			Vector3 rot = ground->GetRotation();
			// 1つずつ調べている
			float maxRad = rot.y + (oneRad * 0.25f);/*90度を4分割*/
			// 左側なので-
			if (tag.find("Left") != std::string::npos) {
				maxRad = rot.y + (oneRad * 0.25f);
				// angleがrotよりも大きく、maxRadよりも小さい
				if (maxRad >= angle && angle >= rot.y) {
					// ダメージが0なら通らない
					if (damage != 0) {
						ground->OnCollision(damage);
					}
					return true;
				}
			}
			else if (rot.y <= angle && angle < maxRad) {
				// ダメージが0なら通らない
				if (damage != 0) {
					ground->OnCollision(damage);
				}
				return true;
			}
		}
	}
	return false;
}

void Ground::CheckAlive() {
	for (uint32_t index = 0u; index < pieces_.size(); index++) {
		pieceAlives_.at(index) = pieces_.at(index)->GetIsAlive();
	}
}
