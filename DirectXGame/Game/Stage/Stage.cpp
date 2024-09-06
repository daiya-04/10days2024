#include "Stage.h"

void Stage::Initialize(const LevelData* data) {

	std::shared_ptr<Model> stagePiece = ModelManager::LoadOBJ("StagePiece");
	
	WorldTransform transform;
	transform.Init();
	transform.translation_.y = 0.0f;
	transform.UpdateMatrix();
	for (auto& ground : grounds_) {
		ground = std::make_unique<Ground>();
		ground->Initialize(data, { stagePiece }, transform);
		transform.translation_.y += 50.0f;
		transform.UpdateMatrix();
	}
}

void Stage::Draw(const Camera& camera) {
	for (auto& ground : grounds_) {
		ground->Draw(camera);
	}
}
