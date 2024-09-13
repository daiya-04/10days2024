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
#include"ResultScene.h"

GameScene::GameScene() {
	
}

GameScene::~GameScene() {
	bgm_->StopSound();
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

	std::shared_ptr<Model> bossModel = ModelManager::LoadOBJ("Kagamimochi");
	std::shared_ptr<Model> meteorModel = ModelManager::LoadOBJ("Meteor");
	std::shared_ptr<Model> cannonModel = ModelManager::LoadOBJ("CannonBall");
	std::shared_ptr<Model> stampModel = ModelManager::LoadOBJ("Stamp");

	TextureManager::GetInstance()->LoadTextureMap("CenterElectricBoard", "CenterElectricBoard.png");

	bgm_ = AudioManager::Load("BGM/Game3.mp3");
	bgm_->SetUniqueVolume(0.3f);
	bgm_->Play();

	reflectionSE_ = AudioManager::Load("SE/居合抜き1.mp3");
	

	meteor_ = MeteorManager::GetInstance();
	meteor_->Init(meteorModel);

	cannon_ = CannonManager::GetInstance();
	cannon_->Init(cannonModel);

	stamp_ = StampManager::GetInstance();
	stamp_->Init(stampModel);

	boss_ = std::make_unique<Boss>();
	boss_->Init(bossModel);
	

	camera_.translation_ = { 0.0f,5.0f,-20.0f };

#ifdef _DEBUG
	debugCamera_ = std::make_unique<DebugCamera>();
#endif // _DEBUG

	

	levelData_ = LevelLoader::LoadFile("stageTest");

	stage_ = std::make_unique<Stage>();
	stage_->Initialize(levelData_);

	player_ = std::make_unique<Player>();
	player_->Initialize();
	player_->SetScene(true);

	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->SetTarget(&player_->GetWorldTrnas());
	followCamera_->SetScene(true);

	player_->SetCameraRotate(&followCamera_->GetCameraRotate());

	modelManager_ = ModelManager::GetInstance();

	collisionManager_ = std::make_unique<CollisionManager>();
	collisionManager_->Initialize(player_.get(), boss_.get());


	skyBox_.reset(SkyBox::Create(TextureManager::Load("output_image.dds")));

	for (size_t i = 0; i < playerLifeTex_.size(); i++){
		playerLifeTex_[i] = Sprite::Create(TextureManager::GetInstance()->Load("plaeyrLife.png"), Vector2(0.0f, 0.0f), 0.1f);
		playerLifeTex_[i]->size_ = { lifeScale_,lifeScale_ };
	}


	Update();
}

void GameScene::Update() {
	DebugGUI();

	boss_->SetAttackMode(stage_->GetLayerNumber());

#ifdef _DEBUG

	if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_1)) {
		SceneManager::GetInstance()->ChangeScene("Title");
	}
	if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_3)) {
		SceneManager::GetInstance()->ChangeScene("Debug");
	}
	if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_4)) {
		SceneManager::GetInstance()->ChangeScene("Result");
	}

#endif // _DEBUG

#ifdef NDEBUG
	followCamera_->Update();
	camera_.translation_ = followCamera_->GetCameraTranslate();
	camera_.rotation_ = followCamera_->GetCameraRotate();
#endif // NDEBUG

	stage_->Update();

	meteor_->SetPieceAlives(stage_->GetPieceAlive());
	cannon_->SetPieceAlives(stage_->GetPieceAlive());
	stamp_->SetPieceAlives(stage_->GetPieceAlive());

	boss_->Update();
	meteor_->Update();
	cannon_->Update();
	stamp_->Update();

	player_->Update(Vector3(0.0f, 0.0f, 0.0f), Vector2(26.2f, 37.0f));
	
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
			// playerのparameterをReset
			player_->Reset();
		}
	}
	else {
		// 床があった場合
		player_->SetFloorPosition(stage_->GetGroundPosition().y);

		bool nowFallAttack = player_->GetFallingAttack();
		if (nowFallAttack && !oldFallAttack_) {
			stage_->IsCollision(player_->GetTransform().GetWorldPosition(), (player_->GetAttackPower() * (stage_->GetLayerNumber() + 1)));
		}
	}

	// 隕石の衝突判定
	for (uint32_t index = 0; index < 16; index++) {
		
		if (!meteor_->IsLife(index) && !meteor_->GetIsHit(index)) { continue; }

		// 跳ね返した時用 ボスとの衝突判定
		if (IsCollision(meteor_->GetCollider(index), boss_->GetCollider(Boss::AttackMode::kHigh))) {
			boss_->AttackHit();
			meteor_->Hit(index);
		}
		else if (IsCollision(player_->GetReflectionCollider(), meteor_->GetCollider(index))) {
			// playerと当たって跳ね返した瞬間
			if (player_->IsCharge()) {
				meteor_->Reflection(index);
				reflectionSE_->Play();
			}
			else {
				meteor_->Hit(index);
				player_->HitEnemyAttackCollision();
			}
		}
		// そのまま床に当たった時の衝突判定
		else if (meteor_->GetIsHit(index)) {
			if (stage_->IsCollision(meteor_->GetCollider(index).center, meteor_->GetDamage(index))) {
				// メテオのAttackUpdate内で消えているから正直意味はない
				meteor_->Hit(index);
			}
		}
		else if (IsCollision(player_->GetCollider(), meteor_->GetCollider(index))) {
			 {
				meteor_->Hit(index);
				player_->HitEnemyAttackCollision();
			}

		}
	}
	//餅弾とボスの衝突判定
	for (uint32_t index = 0; index < 16; index++) {
		if (!cannon_->IsLife(index) && !cannon_->GetIsHit(index)) { continue; }
		if (IsCollision(player_->GetReflectionCollider(), cannon_->GetCollider(index))) {
			if (player_->IsCharge()) {
				cannon_->Reflection(index);
				reflectionSE_->Play();
			}
			else {
				player_->HitEnemyAttackCollision();
				cannon_->Hit(index);
			}
		}
		else if (IsCollision(player_->GetCollider(), cannon_->GetCollider(index))) {
			 {
				player_->HitEnemyAttackCollision();
				cannon_->Hit(index);
			}
		}
		// そのまま床に当たった時の衝突判定
		else if (cannon_->GetIsHit(index)) {
			if (stage_->IsCollision(cannon_->GetCollider(index).center, cannon_->GetDamage(index))) {
				// メテオのAttackUpdate内で消えているから正直意味はない
				//cannon_->Hit(index);
			}
		}
		if (!cannon_->IsBossHit(index)) { continue; }
		if (IsCollision(boss_->GetCollider(Boss::AttackMode::kMiddle), cannon_->GetCollider(index))) {
			boss_->AttackHit();
			cannon_->Hit(index);
		}
	}

	for (uint32_t index = 0; index < 16; index++) {
		if (!stamp_->IsLife(index) && !stamp_->GetIsHit(index)) { continue; }
		if (IsCollision(stamp_->GetCollider(index), player_->GetCollider())) {
			player_->HitEnemyAttackCollision();
			stamp_->Hit(index);
		}
		// そのまま床に当たった時の衝突判定
		else if (stamp_->GetIsHit(index)) {
			if (stage_->IsCollision(stamp_->GetCollider(index).center, stamp_->GetDamage(index))) {
				// メテオのAttackUpdate内で消えているから正直意味はない
				//stamp_->Hit(index);
			}
		}
	}

	// stageのlayerが切り替わった時に全て初期化する
	if (stage_->GetChangeLayer()) {
		meteor_->Reset();
		cannon_->Reset();
		stamp_->Reset();
	}
	if (boss_->GetIsDead()){
		SceneManager::GetInstance()->ChangeScene("Result");
	}

	if (player_->GetIsDead()){
		ResultScene::isLose_ = true;
		SceneManager::GetInstance()->ChangeScene("Result");
	}

	collisionManager_->AllCollision();
	oldFallAttack_ = player_->GetFallingAttack();

	// Camera
	if (debugCamera_->Update()) {
		camera_.translation_ = debugCamera_->GetCameraTranslate();
		camera_.rotation_ = debugCamera_->GetCameraRotate();
	}
	else {

		int32_t index = stage_->GetLayerNumber() - 1;
		if (index <= 0) { index = 0; }
		Vector3 pos = stage_->GetGroundPosition(index);

		followCamera_->Update();
		camera_.translation_ = followCamera_->GetCameraTranslate();
		camera_.rotation_ = followCamera_->GetCameraRotate();
	}

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
	cannon_->Draw(camera_);
	stamp_->Draw(camera_);

	stage_->Draw(camera_);
	player_->Draw(camera_);

	skyBox_->Draw(camera_);

}

void GameScene::DrawParticleModel(){



}

void GameScene::DrawParticle(){

	GPUParticle::preDraw();
	meteor_->DrawParticle(camera_);
	cannon_->DrawParticle(camera_);
	stamp_->DrawParticle(camera_);
	player_->ParticleDraw(camera_);

}

void GameScene::DrawUI(){

	for (size_t i = 0; i < player_->GetLife(); i++) {
		playerLifeTex_[i]->position_ = { lifeTexBasePos_.x + lifeTexLength_ * i,lifeTexBasePos_.y };
		playerLifeTex_[i]->Draw();
	}
	player_->DrawUI();
	boss_->DrawUI();
	
}

void GameScene::DrawPostEffect() {

	

}

void GameScene::DrawRenderTexture() {
	
}



void GameScene::DebugGUI(){
#ifdef _DEBUG
  
	player_->Imgui();

	ImGui::Begin("playerLife");
	ImGui::DragFloat("length", &lifeTexLength_, 0.1f, 0.0f, 1000.0f);
	ImGui::DragFloat2("basePos", &lifeTexBasePos_.x, 1.0f);
	ImGui::End();


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

