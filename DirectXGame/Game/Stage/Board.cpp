#include "Board.h"
#include "ImGuiManager.h"
#include "Engine/Easing/Easing.h"

void Board::Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	model_->worldTransform_.scale_ = scale;
	model_->worldTransform_.rotation_ = rotate;
	model_->worldTransform_.translation_ = translate;
	model_->worldTransform_.UpdateMatrix();
	// 2回転させる
	easeParam_.Initialize(rotate.y, rotate.y + (1440.0f * (std::numbers::pi_v<float> / 180.0f)), 120.0f);
}

void Board::SetModel(std::shared_ptr<Model> model) {
	model_ = std::make_unique<Object3d>();
	model_->Initialize(model);
}

void Board::Update() {
	if (IsAnimation_) {
		Animation();
	}
	model_->worldTransform_.UpdateMatrix();
}

void Board::Draw(const Camera& camera) {
	model_->Draw(camera, handle_);
}

void Board::ImGuiProc() {
#ifdef _DEBUG
	ImGui::DragFloat3("scale", &model_->worldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &model_->worldTransform_.rotation_.x, 0.1f);
	ImGui::DragFloat3("translate", &model_->worldTransform_.translation_.x, 0.1f);
#endif // _DEBUG
}

void Board::Animation() {
	static float oneCount = 1.0f / easeParam_.maxCount_;
	easeParam_.T += oneCount;
	if (easeParam_.T >= 1.0f) {
		easeParam_.T = 0.0f;
		IsAnimation_ = false;
	}
	model_->worldTransform_.rotation_.y = Easing::Ease(Easing::EaseName::EaseOutCubic, easeParam_.start_, easeParam_.end_, easeParam_.T);
}

