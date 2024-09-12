#include "GroundPiece.h"

GroundPiece::GroundPiece(const std::vector<std::shared_ptr<Model>>& models) {
	for (uint8_t index = 0u; index < models.size(); index++) {
		models_.push_back(std::make_shared<Object3d>());
		auto& a = models.at(index);
		models_.at(index)->Initialize(a);
	}
}

void GroundPiece::Initialize(const LevelData::ObjectData& data, const std::string& tag, const WorldTransform* parent) {
	tag_ = tag;
	
	isAlive_ = true;
	transform_.Init();
	transform_.parent_ = parent;
	transform_.scale_ = data.scaling;
	transform_.rotation_ = data.rotation;
	transform_.translation_ = data.translation;
	transform_.UpdateMatrix();
	for (auto& model : models_) {
		if (parent) {
			model->worldTransform_.parent_ = &transform_;
		}
		model->worldTransform_.UpdateMatrix();
	}

}

void GroundPiece::Initialize() {
	isAlive_ = true;
	hp_ = 10;
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

void GroundPiece::OnCollision(const int32_t& damage) {
	hp_ -= damage;
	if (hp_ <= 0) {
		isAlive_ = false;
	}
}

void GroundPiece::UpdateTrans() {
	transform_.UpdateMatrix();
	for (auto& model : models_) {
		model->worldTransform_.UpdateMatrix();
	}
}
