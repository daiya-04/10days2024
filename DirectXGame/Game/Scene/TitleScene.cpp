#include "TitleScene.h"

#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "SceneManager.h"
#include "Input.h"

TitleScene::~TitleScene() {
	bgm_->StopSound();
}

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

	bgm_ = AudioManager::Load("BGM/titleBGM.mp3");
	bgm_->SetUniqueVolume(0.3f);
	bgm_->Play();

#ifdef _DEBUG
	debugCamera_ = std::make_unique<DebugCamera>();
#endif // _DEBUG

	levelData_ = LevelLoader::LoadFile("stageTest");

	TextureManager::GetInstance()->LoadTextureMap("white", "white.png");
	TextureManager::GetInstance()->LoadTextureMap("Groundoutline", "groundPieceOutline.png");

	stage_ = std::make_unique<Stage>();
	stage_->Initialize(levelData_);
	stage_->TitleInitialize();

	player_ = std::make_unique<Player>();
	player_->Initialize();
	player_->SetScene(true);

	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->SetTarget(&player_->GetWorldTrnas());
	followCamera_->SetScene(true);

	player_->SetCameraRotate(&followCamera_->GetCameraRotate());
	player_->Update(Vector3(0.0f, 0.0f, 0.0f), Vector2(26.2f, 37.0f));

	modelManager_ = ModelManager::GetInstance();

	sandbag_ = std::make_unique<Sandbag>();

	sandbag_->Initialize();
	sandbag_->Update();

	collisionManager_ = std::make_unique<CollisionManager>();
	collisionManager_->TitleInitialize(player_.get(), sandbag_.get());

	shelfobj_ = std::make_unique<Object3d>();

	shelfobj_->Initialize(modelManager_->LoadGLTF("shelf"));

	shelfobj_->worldTransform_.translation_ = { 0.0f,-3.0f, 20.0f };
	shelfobj_->worldTransform_.scale_ = { 1.5f,1.0f,1.5f };
	shelfobj_->worldTransform_.UpdateMatrix();

	titleobj_ = std::make_unique<Object3d>();

	titleobj_->Initialize(modelManager_->LoadOBJ("title"));

	titleobj_->worldTransform_.translation_ = { -12.0f,4.0f, 0.0f };
	titleobj_->worldTransform_.rotation_.x = -1.57f;
	titleobj_->worldTransform_.scale_ = { 6.0f,6.0f,6.0f };
	titleobj_->worldTransform_.UpdateMatrix();
	oldFallAttack_ = false;

	killCount_ = 0;

	skyBox_.reset(SkyBox::Create(TextureManager::Load("output_image.dds")));

	controlUI_ = std::make_unique<ControlUI>();
	controlUI_->Initialize();

	Update();
}

void TitleScene::Update(){

#ifdef _DEBUG

	if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_2)) {
		SceneManager::GetInstance()->ChangeScene("Game");
	}
	if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_3)) {
		SceneManager::GetInstance()->ChangeScene("Debug");
	}

	DebugGUI();

#endif // _DEBUG

	// debugCamera
	/*if (debugCamera_->Update()) {
		camera_.translation_ = debugCamera_->GetCameraTranslate();
		camera_.rotation_ = debugCamera_->GetCameraRotate();
	}
	else*/ {
		followCamera_->Update();
		camera_.translation_ = followCamera_->GetCameraTranslate();
		camera_.rotation_ = followCamera_->GetCameraRotate();

	}

#ifdef NDEBUG
	followCamera_->Update();
	camera_.translation_ = followCamera_->GetCameraTranslate();
	camera_.rotation_ = followCamera_->GetCameraRotate();
#endif // NDEBUG

	stage_->TitleUpdate();
	stage_->Update();

	static Vector2 minmax = Vector2(26.2f, 37.0f);
#ifdef _DEBUG
	ImGui::DragFloat2("TestMinMax", &minmax.x, 0.1f);
#endif // _DEBUG
	player_->Update(Vector3(0.0f, 0.0f, 0.0f), minmax);

	// playerとstageの当たり判定
	if (!stage_->IsPlayerCollision(player_->GetTransform().GetWorldPosition())) {
		// 床がなかった場合
		player_->SetFloorPosition(stage_->GetNextGroundPosition().y);
		if (stage_->GetIsEnd()) {
			player_->SetShadowDraw(false);
		}
		else {
			player_->SetShadowDraw(true);
		}

		player_->SetFall(true);
		// playerが下層より下に行った場合
		if (stage_->ResetCheck(player_->GetTransform().GetWorldPosition())) {
			SceneManager::GetInstance()->ChangeScene("Game");
		}
	}
	else {
		// 床があった場合
		player_->SetFloorPosition(stage_->GetGroundPosition().y);

		bool nowFallAttack = player_->GetFallingAttack();
		if (nowFallAttack && !oldFallAttack_) {
			stage_->IsCollision(player_->GetTransform().GetWorldPosition(), (player_->GetAttackPower() * 5));
		}
	}

	sandbag_->Update();
	if (sandbag_->GetIsDead() and killCount_ == 0 ){
		sandbag_->SetPosition(Vector3(0.0f, -12.0f, -20.0f));
		shelfobj_->worldTransform_.translation_ = { 0.0f,-12.5f, -20.0f };
		killCount_++;
		stage_->BoardAnimation(killCount_); // 要素1番目の
	}
	else if (sandbag_->GetIsDead() and killCount_ == 1) {
		sandbag_->SetPosition(Vector3(0.0f, -22.5f, 17.0f));
		shelfobj_->worldTransform_.translation_ = { 0.0f,-23.0f, 17.0f };
		killCount_++;
		stage_->BoardAnimation(killCount_); // 要素2番目の
	}
	else if (sandbag_->GetIsDead() and killCount_ == 2) {
		killCount_++;
		stage_->BoardAnimation(killCount_); // 要素3番目の
	}


	collisionManager_->AllCollision();
	oldFallAttack_ = player_->GetFallingAttack();


	shelfobj_->worldTransform_.UpdateMatrix();
	titleobj_->worldTransform_.UpdateMatrix();

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
	stage_->Draw(camera_);

	player_->Draw(camera_);

	shelfobj_->Draw(camera_);

	titleobj_->Draw(camera_);

	skyBox_->Draw(camera_);
}

void TitleScene::DrawParticleModel(){



}

void TitleScene::DrawParticle(){
	GPUParticle::preDraw();
	player_->ParticleDraw(camera_);

}

void TitleScene::DrawUI(){

	player_->DrawUI();

}

void TitleScene::DrawPostEffect() {



}

void TitleScene::DrawRenderTexture() {



}

void TitleScene::DebugGUI(){
#ifdef _DEBUG
	controlUI_->DrawGUI();
	ImGui::Begin("title");
	ImGui::DragFloat3("transform", &titleobj_->worldTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotate", &titleobj_->worldTransform_.rotation_.x, 0.01f);

	ImGui::End();
#endif // _DEBUG
}


