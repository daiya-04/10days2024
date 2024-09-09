#include "GameScene.h"

#include "DirectXCommon.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "Audio.h"
#include "Input.h"
#include "SceneManager.h"
#include "Hit.h"
#include <random>
#include <algorithm>

GameScene::GameScene() {
	
}

GameScene::~GameScene() {
	
}

void GameScene::Init(){
  
	camera_.Init();
	camera_.translation_.y = 3.5f;
	camera_.translation_.z = -10.0f;
	camera_.rotation_.x = 0.15f;

	pointLight_.Init();
	pointLight_.intensity_ = 0.0f;
	spotLight_.Init();
	spotLight_.intensity_ = 0.0f;

	Object3d::SetPointLight(&pointLight_);
	Object3d::SetSpotLight(&spotLight_);

	std::shared_ptr<Model> bossModel = ModelManager::LoadOBJ("Boss");
	std::shared_ptr<Model> meteorModel = ModelManager::LoadOBJ("Meteor");


	boss_ = std::make_unique<Boss>();
	boss_->Init(bossModel);

	meteor_ = MeteorManager::GetInstance();
	meteor_->Init(meteorModel);

	camera_.translation_ = { 0.0f,5.0f,-20.0f };

#ifdef _DEBUG
	debugCamera_ = std::make_unique<DebugCamera>();
#endif // _DEBUG


	levelData_ = LevelLoader::LoadFile("stageTest");

	stage_ = std::make_unique<Stage>();
	stage_->Initialize(levelData_);

	player_ = std::make_unique<Player>();
	player_->Initialize();

	modelManager_ = ModelManager::GetInstance();

	floor_ = std::make_unique<Object3d>();

	floor_->Initialize(modelManager_->LoadOBJ("Floor"));
	floor_->worldTransform_.translation_.y = -2.0f;
	floor_->worldTransform_.scale_ = { 1.0f,1.0f,1.0f };

}

void GameScene::Update() {
	DebugGUI();

#ifdef _DEBUG

	if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_1)) {
		SceneManager::GetInstance()->ChangeScene("Title");
	}
	if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_3)) {
		SceneManager::GetInstance()->ChangeScene("Debug");
	}

	// debugCamera
	if (debugCamera_->Update()) {
		camera_.translation_ = debugCamera_->GetCameraTranslate();
		camera_.rotation_ = debugCamera_->GetCameraRotate();
	}

#endif // _DEBUG
	floor_->worldTransform_.UpdateMatrix();

	stage_->Update();

	boss_->Update();
	meteor_->Update();

	player_->Update();
	player_->StageClampCollision(Vector3(0.0f, 0.0f, 0.0f));

	static Vector3 testPos = Vector3(10.0f, 0.0f, 0.0f);
#ifdef _DEBUG
	ImGui::DragFloat3("MasterTestPosition", &testPos.x, 0.1f);
#endif // _DEBUG
	floor_->worldTransform_.translation_ = testPos;
	floor_->worldTransform_.UpdateMatrix();
	// ボスの攻撃座標を入れる
	stage_->IsCollision(player_->GetTransform().translation_);
	
	camera_.UpdateMatrix();
	camera_.UpdateCameraPos();
	camera_.UpdateMatrix();
	pointLight_.Update();
	spotLight_.Update();
}

void GameScene::DrawBackGround(){

	

}

void GameScene::DrawModel(){


	boss_->Draw(camera_);
	meteor_->Draw(camera_);

	stage_->Draw(camera_);
	floor_->Draw(camera_);
	player_->Draw(camera_);

}

void GameScene::DrawParticleModel(){



}

void GameScene::DrawParticle(){

	GPUParticle::preDraw();
	meteor_->DrawParticle(camera_);

}

void GameScene::DrawUI(){

	

	
}

void GameScene::DrawPostEffect() {

	

}

void GameScene::DrawRenderTexture() {
	
}



void GameScene::DebugGUI(){
#ifdef _DEBUG
  
	player_->Imgui();

	ImGui::Begin("camera");

	ImGui::DragFloat3("trans", &camera_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotate", &camera_.rotation_.x, 0.1f);

	ImGui::End();
	ImGui::Begin("Light");

	if (ImGui::TreeNode("PointLight")) {

		ImGui::ColorEdit4("color", &pointLight_.color_.x);
		ImGui::DragFloat3("position", &pointLight_.position_.x, 0.01f);
		ImGui::SliderFloat("intensity", &pointLight_.intensity_, 0.0f, 1.0f);
		ImGui::SliderFloat("radius", &pointLight_.radius_, 0.0f, 10.0f);
		ImGui::SliderFloat("decay", &pointLight_.decay_, 0.01f, 2.0f);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("SpotLight")) {

		ImGui::ColorEdit4("color", &spotLight_.color_.x);
		ImGui::DragFloat3("position", &spotLight_.position_.x, 0.01f);
		ImGui::SliderFloat("intensity", &spotLight_.intensity_, 0.0f, 1.0f);
		ImGui::SliderFloat3("direction", &spotLight_.direction_.x, -1.0f, 1.0f);
		ImGui::SliderFloat("distance", &spotLight_.distance_, 0.0f, 10.0f);
		ImGui::SliderFloat("decay", &spotLight_.decay_, 0.01f, 2.0f);
		ImGui::SliderFloat("cosAngle", &spotLight_.cosAngle_, 0.0f, spotLight_.cosFalloffStart_ - 0.001f);
		ImGui::SliderFloat("cosFalloffStart", &spotLight_.cosFalloffStart_, spotLight_.cosAngle_ + 0.001f, 1.0f);

		ImGui::TreePop();
	}

	ImGui::End();

#endif // _DEBUG
}

