#include "GroundPiece.h"

void GroundPiece::Initialize(const LevelData::ObjectData& data, const std::vector<std::shared_ptr<Model>>& models) {
	for (uint8_t index = 0u; index < models.size(); index++) {
		models_.push_back(std::make_shared<Object3d>());
		auto& a = models.at(index);
		models_.at(index)->Initialize(a);
	}
	isAlive_ = true;
	for (auto& model : models_) {
		model->worldTransform_.translation_ = data.translation;
		model->worldTransform_.scale_ = data.scaling;
		model->worldTransform_.rotation_ = data.rotation;
		model->worldTransform_.UpdateMatrix();
	}

}

void GroundPiece::Update() {

	UpdateTrans();
}

void GroundPiece::Draw(const Camera& camera) {
	// 早期リターン
	if (!isAlive_) {
		return;
	}
	for (auto& model : models_) {
		model->Draw(camera);
	}
}

void GroundPiece::UpdateTrans() {
	for (auto& model : models_) {
		model->worldTransform_.UpdateMatrix();
	}
}
