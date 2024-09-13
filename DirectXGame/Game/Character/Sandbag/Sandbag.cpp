#include "Sandbag.h"
#include"Engine/Collision/ShapesDraw.h"

void Sandbag::Initialize(){
	modelManager_ = ModelManager::GetInstance();

	bodyObj_ = std::make_unique<Object3d>();

	bodyObj_->Initialize(modelManager_->LoadOBJ("Kagamimochi"));

	bodyObj_->worldTransform_.translation_ = { 0.0f,-2.5f, 20.0f };
	bodyObj_->worldTransform_.scale_ = { 0.1f,0.1f,0.1f };
	bodyObj_->worldTransform_.UpdateMatrix();
	collider_.center = bodyObj_->worldTransform_.translation_;
	collider_.radius = 6.0f;

}

void Sandbag::Update(){
	if (hp_ < 0) {
		isDead_ = true;
	}
	collider_.center = bodyObj_->worldTransform_.translation_;

	bodyObj_->worldTransform_.UpdateMatrix();
}

void Sandbag::Draw(const Camera& camera){
	if (isDead_){
		return;
	}

	bodyObj_->Draw(camera);
}

void Sandbag::OnCollision(const int32_t power){
	hp_ -= power;
}
