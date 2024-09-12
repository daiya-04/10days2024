#include "Stage.h"
#include "ImGuiManager.h"
#include <GlobalVariables.h>

void Stage::Initialize(const LevelData* data) {

	std::shared_ptr<Model> stagePiece = ModelManager::LoadOBJ("StagePiece");
	std::shared_ptr<Model> electricBoardModel = ModelManager::LoadOBJ("electricBoard");
	
	WorldTransform transform;
	transform.Init();
	transform.translation_.y = 0.0f;
	transform.UpdateMatrix();
	
	for (uint32_t i = 0u; i < grounds_.size(); i++) {
		grounds_.at(i) = std::make_unique<Ground>();
		grounds_.at(i)->Initialize(data, { stagePiece }, transform, electricBoardModel, layer.at(i));
		transform.translation_.y -= 15.0f;
		transform.UpdateMatrix();
	}

	float lDist = 0.0f;
	for (auto& ground : grounds_) {
		ground->transform_.translation_.y = lDist;
		ground->transform_.UpdateMatrix();
		lDist -= distance_;
	}
}

void Stage::Update() {
#ifdef _DEBUG
	ImGui::Begin("Stage");
	
	ImGui::DragFloat("Distance", &distance_, 0.1f);
  
	ImGui::End();
#endif // _DEBUG

	preLayerNumber_ = nowLayerNumber_;

	for (uint32_t index = 0; index < layer.size(); index++) {
		if (index < nowLayerNumber_) {
			grounds_.at(index)->isAlive_ = false;
		}
		else {
			grounds_.at(index)->Update();
		}
	}
	pieceAlives_ = grounds_.at(nowLayerNumber_)->GetPieceAlive();
}

void Stage::Draw(const Camera& camera) {
	for (auto& ground : grounds_) {
		ground->Draw(camera);
	}
	for (auto& board : boards_) {
		board->Draw(camera);
	}
}

bool Stage::IsCollision(const Vector3& position, const int32_t& damage) {
	float angle = GetRadianVectorToVector(Vector3(0.0f, 0.0f, 1.0f), Vector3(position.x, 0.0f, position.z).Normalize());
	if (position.x < 0.0f) {
		angle = -angle;
	}
	float oneRad = 16.0f / 360.0f;

	return grounds_.at(nowLayerNumber_)->IsCollision(angle, damage);
}

bool Stage::IsPlayerCollision(const Vector3& position) {
	float angle = GetRadianVectorToVector(Vector3(0.0f, 0.0f, 1.0f).Normalize(), Vector3(position.x, 0.0f, position.z).Normalize());
	if (position.x < 0.0f) {
		angle = -angle;
	}
	if (angle >= 3.14f) {
		angle = 3.13f;
	}
	else if (angle <= -3.14f) {
		angle = -3.13f;
	}

	float oneRad = 16.0f / 360.0f;

	if (LayerCheck(position.y) == "None") {
		assert(true);
	}
	
	// playerと同じ階層が更新される
	return grounds_.at(nowLayerNumber_)->IsCollision(angle);
}

bool Stage::ResetCheck(const Vector3& position) {
	float groundPos = grounds_.at(layer.size() - 1u)->GetTransform().GetWorldPosition().y;
	if (position.y <= groundPos - distance_) {
		for (auto& ground : grounds_) {
			ground->Initialize();
		}
		nowLayerNumber_ = 0;
		return true;
	}
	return false;
}

Vector3 Stage::GetGroundPosition() const {
	return grounds_.at(nowLayerNumber_)->GetTransform().GetWorldPosition();
}

Vector3 Stage::GetNextGroundPosition()  {
	uint32_t index = nowLayerNumber_ + 1u;
	if (index >= layer.size()) {
		index = static_cast<uint32_t>(layer.size()) - 1u;
		Vector3 result = grounds_.at(index)->GetTransform().GetWorldPosition();
		result.y -= distance_;
		isEnd_ = true;
		return result;
	}
	isEnd_ = false;
	return grounds_.at(index)->GetTransform().GetWorldPosition();
}

void Stage::ApplyGlobalVariables() {
	boardModel_ = ModelManager::LoadOBJ("Board");
	GlobalVariables* global = GlobalVariables::GetInstance();
	//項目の追加
	boardParam_.number = global->GetIntValue(groupName_, "boardNum");
	for (uint32_t index = 0u; index < static_cast<uint32_t>(boardParam_.number); index++) {
		// 要素の追加
		if (boards_.size() >= index) {
			auto& board = boards_.emplace_back(std::make_unique<Board>());
			board->SetModel(boardModel_);
		}
		Vector3 scale = global->GetVec3Value(groupName_, ("board" + std::to_string(index) + " : scale").c_str());
		Vector3 rotate = global->GetVec3Value(groupName_, ("board" + std::to_string(index) + " : rotate").c_str());
		Vector3 translate = global->GetVec3Value(groupName_, ("board" + std::to_string(index) + " : translate").c_str());
		boards_.at(index)->Initialize(scale, rotate, translate);
	}
}

void Stage::SetGlobalVariables() {
	GlobalVariables* global = GlobalVariables::GetInstance();
	//グループの追加
	global->CreateGroup(groupName_);
	//項目の追加
	global->AddItem(groupName_, "boardNum", boardParam_.number);

	Vector3 handle;
	for (uint32_t index = 0u; index < static_cast<uint32_t>(boardParam_.number); index++) {
		global->AddItem(groupName_, ("board" + std::to_string(index) + " : scale").c_str(), handle);
		global->AddItem(groupName_, ("board" + std::to_string(index) + " : rotate").c_str(), handle);
		global->AddItem(groupName_, ("board" + std::to_string(index) + " : translate").c_str(), handle);
	}

	ApplyGlobalVariables();
}

void Stage::SaveGlobalVariables() const {
	GlobalVariables* global = GlobalVariables::GetInstance();
	//項目の追加
	global->SetValue(groupName_, "boardNum", boardParam_.number);

	
	for (uint32_t index = 0u; index < static_cast<uint32_t>(boardParam_.number); index++) {
		global->SetValue(groupName_, ("board" + std::to_string(index) + " : scale").c_str(), boards_.at(index)->GetTransform().scale_);
		global->SetValue(groupName_, ("board" + std::to_string(index) + " : rotate").c_str(), boards_.at(index)->GetTransform().rotation_);
		global->SetValue(groupName_, ("board" + std::to_string(index) + " : translate").c_str(), boards_.at(index)->GetTransform().translation_);
	}
}

void Stage::TitleInitialize() {
	SetGlobalVariables();
	

}

void Stage::TitleUpdate() {
#ifdef _DEBUG
	ImGuiProc();
#endif // _DEBUG

	for (auto& board : boards_) {
		board->Update();
	}

}

void Stage::ImGuiProc() {
#ifdef _DEBUG
	ImGui::Begin("TitleStage");
	if (ImGui::Button("Create")) {
		boardParam_.number++;
		auto& board = boards_.emplace_back(std::make_unique<Board>());
		board->SetModel(boardModel_);
		board->Initialize(Vector3(1.0f, 1.0f, 1.0f), Vector3(), Vector3());
	}
	for (uint32_t index = 0u; index < boards_.size(); index++) {
		if (ImGui::TreeNode(("board" + std::to_string(index)).c_str())) {
			boards_.at(index)->ImGuiProc();
			ImGui::TreePop();
		}
	}
	if (ImGui::Button("Save")) {
		SaveGlobalVariables();
		GlobalVariables::GetInstance()->SaveFile(groupName_);
	}
	ImGui::End();
#endif // _DEBUG
}

std::string Stage::LayerCheck(const float& playerPositionY) {
	nowLayerNumber_ = 0;
	for (auto& ground : grounds_) {
		if (playerPositionY >= ground->transform_.GetWorldPosition().y) {
			return layer.at(nowLayerNumber_);
		}
		if (++nowLayerNumber_ >= layer.size()) {
			nowLayerNumber_ = static_cast<uint32_t>(layer.size()) - 1u;
		}
	}
	return "None";
}
