#include "Board.h"
#include "ImGuiManager.h"

void Board::Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	model_->worldTransform_.scale_ = scale;
	model_->worldTransform_.rotation_ = rotate;
	model_->worldTransform_.translation_ = translate;
	model_->worldTransform_.UpdateMatrix();
}

void Board::SetModel(std::shared_ptr<Model> model) {
	model_ = std::make_unique<Object3d>();
	model_->Initialize(model);
}

void Board::Update() {
	model_->worldTransform_.UpdateMatrix();
}

void Board::Draw(const Camera& camera) {
	model_->Draw(camera);
}

void Board::ImGuiProc() {
#ifdef _DEBUG
	ImGui::DragFloat3("scale", &model_->worldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &model_->worldTransform_.rotation_.x, 0.1f);
	ImGui::DragFloat3("translate", &model_->worldTransform_.translation_.x, 0.1f);
#endif // _DEBUG
}
