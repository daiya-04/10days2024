#include "ResultScene.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "SceneManager.h"
#include "Input.h"


ResultScene::~ResultScene() {}

void ResultScene::Init() {
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

	camera_.translation_ = { 0.0f,5.0f,-20.0f };

#ifdef _DEBUG
	debugCamera_ = std::make_unique<DebugCamera>();
#endif // _DEBUG

	modelManager_ = ModelManager::GetInstance();

	sandbag_ = std::make_unique<Sandbag>();

	sandbag_->Initialize();

	shelfobj_ = std::make_unique<Object3d>();

	shelfobj_->Initialize(modelManager_->LoadGLTF("shelf"));

	shelfobj_->worldTransform_.translation_ = { 0.0f,-1.0f, -20.0f };
	shelfobj_->worldTransform_.scale_ = { 1.5f,1.0f,1.5f };



}

void ResultScene::Update() {

#ifdef _DEBUG

	if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_2)) {
		SceneManager::GetInstance()->ChangeScene("Game");
	}
	if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_3)) {
		SceneManager::GetInstance()->ChangeScene("Debug");
	}

#endif // _DEBUG

	followCamera_->Update();
	camera_.translation_ = followCamera_->GetCameraTranslate();
	camera_.rotation_ = followCamera_->GetCameraRotate();

	sandbag_->Update();
	if (sandbag_->GetIsDead()) {
		SceneManager::GetInstance()->ChangeScene("Game");
	}

	shelfobj_->worldTransform_.UpdateMatrix();

	camera_.UpdateMatrix();
	camera_.UpdateCameraPos();
	camera_.UpdateMatrix();
	pointLight_.Update();
	spotLight_.Update();
}

void ResultScene::DrawBackGround() {



}

void ResultScene::DrawModel() {
	sandbag_->Draw(camera_);
	shelfobj_->Draw(camera_);

}

void ResultScene::DrawParticleModel() {



}

void ResultScene::DrawParticle() {



}

void ResultScene::DrawUI() {



}

void ResultScene::DrawPostEffect() {



}

void ResultScene::DrawRenderTexture() {



}

void ResultScene::DebugGUI() {
#ifdef _DEBUG




#endif // _DEBUG
}
