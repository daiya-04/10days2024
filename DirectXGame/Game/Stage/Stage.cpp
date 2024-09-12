#include "Stage.h"
#include "ImGuiManager.h"

void Stage::Initialize(const LevelData* data) {

	std::shared_ptr<Model> stagePiece = ModelManager::LoadOBJ("StagePiece");
	std::shared_ptr<Model> electricBoardModel = ModelManager::LoadOBJ("ElectricBoard");
	
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
}

void Stage::Update() {
#ifdef _DEBUG
	ImGui::Begin("Stage");
	
	ImGui::DragFloat("Distance", &distance_, 0.1f);
  
	ImGui::End();
	float lDist = 0.0f;
	for (auto& ground : grounds_) {
		ground->transform_.translation_.y = lDist;
		lDist -= distance_;
	}

#endif // _DEBUG


	for (auto& ground : grounds_) {
		ground->Update();
	}
	pieceAlives_ = grounds_.at(nowLayerNumber_)->GetPieceAlive();
}

void Stage::Draw(const Camera& camera) {
	for (auto& ground : grounds_) {
		ground->Draw(camera);
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
	static bool fall = false;
	ImGui::Checkbox("fall", &fall);
	int32_t damage = 0;
	if (fall) {
		damage = 1;
	}
	// playerと同じ階層が更新される
	return grounds_.at(nowLayerNumber_)->IsCollision(angle, damage);
}

bool Stage::ResetCheck(const Vector3& position) {
	float groundPos = grounds_.at(layer.size() - 1u)->GetTransform().GetWorldPosition().y;
	if (position.y <= groundPos - distance_) {
		for (auto& ground : grounds_) {
			ground->Initialize();
		}
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
