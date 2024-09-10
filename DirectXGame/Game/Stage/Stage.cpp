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
	static float distance = 15.0f;
	ImGui::DragFloat("Distance", &distance, 0.1f);
	ImGui::End();
	float lDist = distance;
	for (auto& ground : grounds_) {
		ground->transform_.translation_.y = lDist;
		lDist -= distance;
	}

#endif // _DEBUG


	for (auto& ground : grounds_) {
		ground->Update();
	}
}

void Stage::Draw(const Camera& camera) {
	for (auto& ground : grounds_) {
		ground->Draw(camera);
	}
}

void Stage::IsCollision(const Vector3& position) {
	float angle = GetRadianVectorToVector(Vector3(0.0f, 0.0f, 1.0f), position.Normalize());
	if (position.x < 0.0f) {
		angle = -angle;
	}
	float oneRad = 16.0f / 360.0f;

	for (auto& ground : grounds_) {
		ground->IsCollision(angle);
	}
}

bool Stage::IsPlayerCollision(const Vector3& position) {
	float angle = GetRadianVectorToVector(Vector3(0.0f, 0.0f, 1.0f).Normalize(), Vector3(position.x, 0.0f, position.z).Normalize());
	if (position.x < 0.0f) {
		angle = -angle;
	}
	float oneRad = 16.0f / 360.0f;

	std::string lLayer = LayerCheck(position.y);

	// playerと同じ階層が更新される
	return grounds_.at(nowLayerNumber_)->IsCollision(angle, 1.0f);
}

Vector3 Stage::GetGroundPosition() const {
	return grounds_.at(nowLayerNumber_)->GetTransform().GetWorldPosition();
}

Vector3 Stage::GetNextGroundPosition() const {
	uint32_t index = nowLayerNumber_ + 1u;
	if (index >= layer.size()) {
		index = 0;
		for (auto& ground : grounds_) {
			ground->Initialize();
		}
	}
	return grounds_.at(index)->GetTransform().GetWorldPosition();
}

std::string Stage::LayerCheck(const float& playerPositionY) {
	nowLayerNumber_ = 0;
	for (auto& ground : grounds_) {
		if (playerPositionY >= ground->transform_.GetWorldPosition().y) {
			return layer.at(nowLayerNumber_);
		}
		nowLayerNumber_++;
	}
	return "None";
}
