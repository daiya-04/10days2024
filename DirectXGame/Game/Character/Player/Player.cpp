#include "Player.h"

void Player::Initialize(){
	modelManager_ = ModelManager::GetInstance();

	bodyObj_ = std::make_unique<Object3d>();

	bodyObj_->Initialize(modelManager_->LoadOBJ("box"));

}

void Player::Update(){
	bodyObj_->worldTransform_.UpdateMatrix();

}

void Player::Draw(const Camera& camera){
	bodyObj_->Draw(camera);
}

void Player::Imgui(){

}
