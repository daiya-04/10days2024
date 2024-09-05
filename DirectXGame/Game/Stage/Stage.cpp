#include "Stage.h"

void Stage::Initialize(const LevelData* data) {

	std::shared_ptr<Model> stagePiece = ModelManager::LoadOBJ("StagePiece");
	
	for (auto& ground : grounds_) {
		ground = std::make_unique<Ground>();
		ground->Initialize(data, { stagePiece });
	}
}

void Stage::Draw(const Camera& camera) {
	for (auto& ground : grounds_) {
		ground->Draw(camera);
	}
}
