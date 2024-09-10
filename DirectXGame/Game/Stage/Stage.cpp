#include "Stage.h"

void Stage::Initialize(const LevelData* data) {

	std::shared_ptr<Model> stagePiece = ModelManager::LoadOBJ("StagePiece");
	std::shared_ptr<Model> electricBoardModel = ModelManager::LoadOBJ("ElectricBoard");
	
	WorldTransform transform;
	transform.Init();
	transform.translation_.y = 0.0f;
	transform.UpdateMatrix();
	for (auto& ground : grounds_) {
		ground = std::make_unique<Ground>();
		ground->Initialize(data, { stagePiece }, transform, electricBoardModel);
		transform.UpdateMatrix();
	}
}

void Stage::Update() {
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
