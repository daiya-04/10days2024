#include "ControlUI.h"

void ControlUI::Initialize(){
	texManager_ = TextureManager::GetInstance();

	Atex_ = Sprite::Create(texManager_->Load("A.png"), Vector2(0.0f, 0.0f), 0.1f);
	Atex_->size_ = { scale_,scale_ };
	Atex_->position_ = { 924,504 };
	Xtex_ = Sprite::Create(texManager_->Load("X.png"), Vector2(0.0f, 0.0f), 0.1f);
	Xtex_->size_ = { scale_,scale_ };
	Xtex_->position_ = { 924,584 };
	RBtex_ = Sprite::Create(texManager_->Load("RB.png"), Vector2(0.0f, 0.0f), 0.1f);
	RBtex_->size_ = { scale_,scale_ };
	RBtex_->position_ = { 924,664 };
	jumptex_ = Sprite::Create(texManager_->Load("jump.png"), Vector2(0.0f, 0.0f), 0.1f);
	jumptex_->size_ = { scale_,scale_ };
	jumptex_->position_ = { 1130,504 };
	attacktex_ = Sprite::Create(texManager_->Load("Attack.png"), Vector2(0.0f, 0.0f), 0.1f);
	attacktex_->size_ = { scale_,scale_ };
	attacktex_->position_ = { 1130,584 };
	dashtex_ = Sprite::Create(texManager_->Load("dash.png"), Vector2(0.0f, 0.0f), 0.1f);
	dashtex_->size_ = { scale_,scale_ };
	dashtex_->position_ = { 1130,664 };

}

void ControlUI::Update(){
	
}

void ControlUI::Draw(){
	Atex_->Draw();
	Xtex_->Draw();
	RBtex_->Draw();
	jumptex_->Draw();
	attacktex_->Draw();
	dashtex_->Draw();
}

void ControlUI::DrawGUI(){
	ImGui::Begin("UI");

	ImGui::DragFloat2("Atex", &Atex_->position_.x, 1.0f);
	ImGui::DragFloat2("Xtex", &Xtex_->position_.x, 1.0f);
	ImGui::DragFloat2("RBtex", &RBtex_->position_.x, 1.0f);
	ImGui::DragFloat2("jumptex", &jumptex_->position_.x, 1.0f);
	ImGui::DragFloat2("Attacktex", &attacktex_->position_.x, 1.0f);
	ImGui::DragFloat2("dashtex", &dashtex_->position_.x, 1.0f);

	ImGui::DragFloat2("AtexScale", &Atex_->size_.x, 0.1f);
	ImGui::DragFloat2("XtexScale", &Xtex_->size_.x, 0.1f);
	ImGui::DragFloat2("RBtexScale", &RBtex_->size_.x, 0.1f);
	ImGui::DragFloat2("jumptexScale", &jumptex_->size_.x, 0.1f);
	ImGui::DragFloat2("AttacktexScale", &attacktex_->size_.x, 0.1f);
	ImGui::DragFloat2("dashtexScale", &dashtex_->size_.x, 0.1f);

	ImGui::End();
}
