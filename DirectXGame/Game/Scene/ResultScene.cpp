#include "ResultScene.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "SceneManager.h"
#include "Input.h"

bool ResultScene::isLose_ = false;

ResultScene::~ResultScene() {}

void ResultScene::LoseInitialize(){

	fade_.reset(Sprite::Create(texManager_->Load("Black.png"), {640.0f,360.0f}));
	fade_->SetSize({ 1280.0f,720.0f });
	fade_->color_ = Vector4(1.0f, 1.0f, 1.0f, alpha_);

	loseTex_.reset(Sprite::Create(texManager_->Load("Lose.png"), { 640.0f,200.0f }));
	loseTex_->SetSize({ 22.5f,15.0f });
}

void ResultScene::LoseUpdate(){

}

void ResultScene::LoseDrawModel(){

}

void ResultScene::LoseDrawUI(){
	loseTex_->Draw();
	fade_->Draw();

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
	sandbag_->Update();


	shelfobj_->worldTransform_.UpdateMatrix();
}

void ResultScene::WinDrawModel(){
	sandbag_->Draw(camera_);
	shelfobj_->Draw(camera_);
}

void ResultScene::WinDrawUI(){
	winTex_->Draw();
}

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
#endif // _DEBUG

	modelManager_ = ModelManager::GetInstance();

	texManager_ = TextureManager::GetInstance();

	skyBox_.reset(SkyBox::Create(TextureManager::Load("output_image.dds")));

	
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
#endif // _DEBUG
}
