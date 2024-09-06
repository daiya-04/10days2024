#include "DebugTestScene.h"

#include "DirectXCommon.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "AnimationManager.h"
#include "Audio.h"
#include "Input.h"
#include "SceneManager.h"
#include "Line.h"
#include "ShapesDraw.h"

void DebugTestScene::Init() {

	camera_.Init();
	pointLight_.Init();
	pointLight_.intensity_ = 0.0f;
	spotLight_.Init();
	spotLight_.intensity_ = 0.0f;

	Object3d::SetPointLight(&pointLight_);
	Object3d::SetSpotLight(&spotLight_);

	particle_.reset(GPUParticle::Create(TextureManager::Load("circle.png"), 10000));

	particle_->isLoop_ = true;

	particle_->emitter_.count = 1000;
	particle_->emitter_.direction = Vector3(0.0f, 1.0f, 0.0f);
	particle_->emitter_.angle = 180.0f;
	particle_->emitter_.color = Vector4(0.89f, 0.27f, 0.03f, 1.0f);
	particle_->emitter_.emit = 0;
	particle_->emitter_.lifeTime = 2.0f;
	particle_->emitter_.speed = 1.0f;
	particle_->emitter_.scale = 0.1f;
	particle_->emitter_.size = { 0.01f,0.01f,0.01f };
	particle_->emitter_.translate = {};
	particle_->emitter_.frequency = 1.0f / 60.0f;

	
}

void DebugTestScene::Update() {
	DebugGUI();

#ifdef _DEBUG

	if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_1)) {
		SceneManager::GetInstance()->ChangeScene("Title");
	}
	if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_2)) {
		SceneManager::GetInstance()->ChangeScene("Game");
	}
	
#endif // _DEBUG
	
	particle_->Update();

	camera_.UpdateViewMatrix();
	camera_.UpdateMatrix();
	pointLight_.Update();
	spotLight_.Update();
}

void DebugTestScene::DrawBackGround() {



}

void DebugTestScene::DrawModel() {

	

}

void DebugTestScene::DrawParticleModel() {



}

void DebugTestScene::DrawParticle() {

	GPUParticle::preDraw();
	particle_->Draw(camera_);

}

void DebugTestScene::DrawUI() {



}

void DebugTestScene::DrawPostEffect() {
	

}

void DebugTestScene::DrawRenderTexture() {

	
}

void DebugTestScene::DebugGUI() {
#ifdef _DEBUG
	ImGui::Begin("camera");

	ImGui::DragFloat3("pos", &camera_.translation_.x, 0.01f);
	ImGui::DragFloat3("rotate", &camera_.rotation_.x, 0.01f);

	ImGui::End();


#endif // _DEBUG
}
