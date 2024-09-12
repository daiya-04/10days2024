#include "TitleScene.h"

#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "SceneManager.h"
#include "Input.h"

TitleScene::~TitleScene() {}

void TitleScene::Init(){
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

	player_ = std::make_unique<Player>();
	player_->Initialize();

	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->SetTarget(&player_->GetWorldTrnas());
	followCamera_->SetScene(false);

	player_->SetCameraRotate(&followCamera_->GetCameraRotate());

	modelManager_ = ModelManager::GetInstance();

	sandbag_ = std::make_unique<Sandbag>();

	sandbag_->Initialize();

	collisionManager_ = std::make_unique<CollisionManager>();
	collisionManager_->TitleInitialize(player_.get(), sandbag_.get());

	floor_ = std::make_unique<Object3d>();

	floor_->Initialize(modelManager_->LoadOBJ("Floor"));
	floor_->worldTransform_.translation_.y = -3.0f;
	floor_->worldTransform_.scale_ = { 100.0f,1.0f,100.0f };

	shelfobj_ = std::make_unique<Object3d>();

	shelfobj_->Initialize(modelManager_->LoadGLTF("shelf"));

	shelfobj_->worldTransform_.translation_ = { 0.0f,-1.0f, -20.0f };
	shelfobj_->worldTransform_.scale_ = { 1.5f,1.0f,1.5f };

	

}

void TitleScene::Update(){

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

	floor_->worldTransform_.UpdateMatrix();

	// 床があった場合
	player_->SetFloorPosition(0.0f);
	player_->SetScene(false);
	player_->Update(Vector3(0.0f, 0.0f, 0.0f), Vector2(0.0f, 0.0f));

	sandbag_->Update();
	if (sandbag_->GetIsDead()){
		SceneManager::GetInstance()->ChangeScene("Game");
	}


	collisionManager_->AllCollision();

	shelfobj_->worldTransform_.UpdateMatrix();

	camera_.UpdateMatrix();
	camera_.UpdateCameraPos();
	camera_.UpdateMatrix();
	pointLight_.Update();
	spotLight_.Update();
}

void TitleScene::DrawBackGround(){

	

}

void TitleScene::DrawModel(){
	sandbag_->Draw(camera_);
	floor_->Draw(camera_);
	player_->Draw(camera_);

	shelfobj_->Draw(camera_);

}

void TitleScene::DrawParticleModel(){



}

void TitleScene::DrawParticle(){



}

void TitleScene::DrawUI(){



}

void TitleScene::DrawPostEffect() {



}

void TitleScene::DrawRenderTexture() {



}

void TitleScene::DebugGUI(){
#ifdef _DEBUG




#endif // _DEBUG
}


