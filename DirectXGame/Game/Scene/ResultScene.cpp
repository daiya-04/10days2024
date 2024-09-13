#include "ResultScene.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "SceneManager.h"
#include "Input.h"

bool ResultScene::isLose_ = false;

ResultScene::~ResultScene() {
	bgm_->StopSound();
}

void ResultScene::LoseInitialize(){

	fade_.reset(Sprite::Create(texManager_->Load("Black.png"), {640.0f,360.0f}));
	fade_->SetSize({ 1280.0f,720.0f });
	fade_->color_ = Vector4(1.0f, 1.0f, 1.0f, alpha_);

	loseTex_.reset(Sprite::Create(texManager_->Load("Lose.png"), { 640.0f,200.0f }));
	loseTex_->SetSize({ 22.5f,15.0f });
}

void ResultScene::LoseUpdate(){
	camera_.translation_ = { 1.2f,5.0f,-1.2f };
	camera_.rotation_ = { 1.2f,-0.8f,0.0f };

	RHandTransform_.translation_ = { 2.0f,0.0f,3.5f };
	LHandTransform_.translation_ = { -2.0f,0.0f,-1.0f };

	ShadowTransform_.translation_.y = -0.7f;
	ShadowTransform_.scale_ = { 2.0f,0.005f,2.0f };

	floor_->worldTransform_.scale_ = { 10.0f,1.0f,10.0f };
	PLTransform_.translation_.y = 0.0f;
	//行列更新
	bodyObj_->worldTransform_ = PLTransform_;
	bodyObj_->worldTransform_.UpdateMatrixRotate(playerRotateMatX_);

}

void ResultScene::LoseDrawModel(){
	bodyObj_->Draw(camera_);
	rightHandObj_->Draw(camera_);
	leftHandObj_->Draw(camera_);
	shadowObj_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 0.7f));
	shadowObj_->Draw(camera_);
	floor_->SetColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	floor_->Draw(camera_);
}

void ResultScene::LoseDrawUI(){
	loseTex_->Draw();
	//fade_->Draw();

}

void ResultScene::WinInitialize(){
	sandbag_ = std::make_unique<Sandbag>();

	sandbag_->Initialize();

	shelfobj_ = std::make_unique<Object3d>();

	shelfobj_->Initialize(modelManager_->LoadGLTF("shelf"));

	shelfobj_->worldTransform_.translation_ = { 0.0f,-1.0f, -20.0f };
	shelfobj_->worldTransform_.scale_ = { 1.5f,1.0f,1.5f };

	winTex_.reset(Sprite::Create(texManager_->Load("Win.png"), { 640.0f,200.0f }));
	winTex_->SetSize({ 22.5f,15.0f });
}

void ResultScene::WinUpdate(){
	camera_.translation_ = { 0.0f,0.3f,-5.0f };
	camera_.rotation_ = { -0.2f,0.0f,0.0f };

	sandbag_->SetPosition(Vector3(6.9f, -0.9f, 10.8f));
	sandbag_->SetRotate(Vector3(0.0f, 0.0f, -0.8f));
	sandbag_->Update();
	sandbag_->DrawGUI();

	RHandTransform_.translation_ = { 2.0f,2.2f,0.0f };
	LHandTransform_.translation_ = { -2.0f,2.2f,0.0f };

	PLTransform_.translation_.y = 1.0f;
	playerRotateMatY_ = MakeRotateYMatrix(float(std::numbers::pi));

	floor_->worldTransform_.scale_ = { 30.0f,1.0f,30.0f };

	shelfobj_->worldTransform_.translation_ = { 0.0f,-0.3f,0.0f };
	shelfobj_->worldTransform_.UpdateMatrix();

	//行列更新
	bodyObj_->worldTransform_ = PLTransform_;
	bodyObj_->worldTransform_.UpdateMatrixRotate(playerRotateMatX_ * playerRotateMatY_);
}

void ResultScene::WinDrawModel(){
	bodyObj_->Draw(camera_);
	rightHandObj_->Draw(camera_);
	leftHandObj_->Draw(camera_);
	floor_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	floor_->Draw(camera_);

	sandbag_->Draw(camera_);
	shelfobj_->Draw(camera_);
}

void ResultScene::WinDrawUI(){
	winTex_->Draw();
}

void ResultScene::Init() {
	camera_.Init();
	
	camera_.translation_.y = 3.5f;
	camera_.translation_.z = 0.0f;
	camera_.rotation_.x = 1.2f;

	pointLight_.Init();
	pointLight_.intensity_ = 0.0f;
	spotLight_.Init();
	spotLight_.intensity_ = 0.0f;

	Object3d::SetPointLight(&pointLight_);
	Object3d::SetSpotLight(&spotLight_);

	camera_.translation_ = { 0.0f,5.0f,-20.0f };

	bgm_ = AudioManager::Load("BGM/Result3.mp3");
	bgm_->SetUniqueVolume(0.3f);
	if (!isLose_) {
		bgm_->Play();
	}

	loseSE_ = AudioManager::Load("SE/playerLose.mp3");
	if (isLose_) {
		loseSE_->Play();
	}
	
	

#ifdef _DEBUG
#endif // _DEBUG

	modelManager_ = ModelManager::GetInstance();

	texManager_ = TextureManager::GetInstance();

	bodyObj_ = std::make_unique<Object3d>();

	bodyObj_->Initialize(modelManager_->LoadGLTF("PlayerFace"));

	rightHandObj_ = std::make_unique<Object3d>();

	rightHandObj_->Initialize(modelManager_->LoadOBJ("PlayerHand"));

	leftHandObj_ = std::make_unique<Object3d>();

	leftHandObj_->Initialize(modelManager_->LoadOBJ("PlayerHand"));

	shadowObj_ = std::make_unique<Object3d>();

	shadowObj_->Initialize(modelManager_->LoadGLTF("shadow"));

	skyBox_.reset(SkyBox::Create(TextureManager::Load("output_image.dds")));

	PLTransform_.Init();

	PLTransform_.scale_ = { 0.3f,0.3f,0.3f };

	PLTransform_.translation_ = Vector3(0.0f, 0.0f, 0.0f);

	RHandTransform_.Init();

	RHandTransform_.scale_ = { 0.5f,0.5f,0.5f };
	RHandTransform_.translation_.x = 2.0f;
	RHandTransform_.parent_ = &bodyObj_->worldTransform_;

	LHandTransform_.Init();

	LHandTransform_.scale_ = { 0.5f,0.5f,0.5f };
	LHandTransform_.translation_.x = -2.0f;
	LHandTransform_.parent_ = &bodyObj_->worldTransform_;

	floor_ = std::make_unique<Object3d>();

	floor_->Initialize(modelManager_->LoadOBJ("Floor"));
	floor_->worldTransform_.translation_.y = -2.0f;
	floor_->worldTransform_.scale_ = { 1.0f,1.0f,1.0f };

	ShadowTransform_.Init();
	ShadowTransform_.scale_ = { 0.5f,0.005f,0.5f };

	
	LoseInitialize();	
	
	WinInitialize();
	

	Update();
	

}

void ResultScene::Update() {
	DebugGUI();
#ifdef _DEBUG

	if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_2)) {
		SceneManager::GetInstance()->ChangeScene("Game");
	}
	if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_3)) {
		SceneManager::GetInstance()->ChangeScene("Debug");
	}

#endif // _DEBUG

	auto input_ = Input::GetInstance();

	if (isLose_){
		LoseUpdate();
	}
	else {
		WinUpdate();
	}
	//どれかのボタンを押したら遷移
	if (input_->TriggerButton(Input::Button::A)|| input_->TriggerButton(Input::Button::B)|| input_->TriggerButton(Input::Button::Y)|| input_->TriggerButton(Input::Button::X)
		|| input_->TriggerButton(Input::Button::RIGHT_SHOULDER)|| input_->TriggerButton(Input::Button::RIGHT_THUMB)|| input_->TriggerButton(Input::Button::LEFT_SHOULDER)|| input_->TriggerButton(Input::Button::LEFT_THUMB)
		|| input_->TriggerButton(Input::Button::BACK)|| input_->TriggerButton(Input::Button::START)){
		SceneManager::GetInstance()->ChangeScene("Title");
	}

	

		
	rightHandObj_->worldTransform_ = RHandTransform_;
	rightHandObj_->worldTransform_.UpdateMatrixRotate(playerRotateMatX_.Inverse());
	
	leftHandObj_->worldTransform_ = LHandTransform_;
	leftHandObj_->worldTransform_.UpdateMatrixRotate(playerRotateMatX_.Inverse());
	
	shadowObj_->worldTransform_ = ShadowTransform_;
	shadowObj_->worldTransform_.UpdateMatrix();

	floor_->worldTransform_.UpdateMatrix();


	camera_.UpdateMatrix();
	camera_.UpdateCameraPos();
	camera_.UpdateMatrix();
	pointLight_.Update();
	spotLight_.Update();
}

void ResultScene::DrawBackGround() {



}

void ResultScene::DrawModel() {
	if (isLose_) {
		LoseDrawModel();
	}
	else {
		WinDrawModel();
	}

	


	skyBox_->Draw(camera_);
}

void ResultScene::DrawParticleModel() {



}

void ResultScene::DrawParticle() {



}

void ResultScene::DrawUI() {
	if (isLose_) {
		LoseDrawUI();
	}
	else {
		WinDrawUI();
	}


}

void ResultScene::DrawPostEffect() {



}

void ResultScene::DrawRenderTexture() {



}

void ResultScene::DebugGUI() {
#ifdef _DEBUG

	ImGui::Begin("Result");
	ImGui::Checkbox("isLose", &isLose_);
	ImGui::End();

	ImGui::Begin("ResultPlayer");
	ImGui::DragFloat3("bodyTrans", &PLTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("RHandTrans", &RHandTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("LHandTrans", &LHandTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("shadowTrans", &ShadowTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("shadowScale", &ShadowTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("floorScale", &floor_->worldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("tanatrans", &shelfobj_->worldTransform_.translation_.x, 0.1f);
	ImGui::End();

	ImGui::Begin("camera");

	ImGui::DragFloat3("trans", &camera_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotate", &camera_.rotation_.x, 0.1f);

	ImGui::End();
#endif // _DEBUG
}
