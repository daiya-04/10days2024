#include "Boss.h"

#include "MeteorManager.h"
#include "CannonManager.h"
#include "StampManager.h"
#include "ImGuiManager.h"
#include "ShapesDraw.h"

void Boss::Init(const std::shared_ptr<Model>& model) {

	obj_.reset(Object3d::Create(model));

	hp_ = maxHp_;

	behaviorRequest_ = Behavior::kRoot;

	direction_ = { 0.0f,0.0f,-1.0f };

	rotateMat_ = DirectionToDirection({ 0.0f,0.0f,1.0f }, direction_);

	collider_[AttackMode::kHigh] = { Vector3(0.0f,10.0f,0.0f),4.0f };
	collider_[AttackMode::kMiddle] = { Vector3(0.0f,0.0f,0.0f),15.0f };
	collider_[AttackMode::kUnder] = { Vector3(0.0f,-12.0f,0.0f),23.0f };

	obj_->worldTransform_.translation_ = { 0.0f,-23.0f,0.0f };
	obj_->worldTransform_.scale_ = Vector3(1.0f, 1.0f, 1.0f) * 1.5f;

}

void Boss::Update() {
	DebugGUI();

	if (behaviorRequest_) {

		behavior_ = behaviorRequest_.value();

		behaviorInitTable_[behavior_]();

		behaviorRequest_ = std::nullopt;
	}

	behaviorUpdateTable_[behavior_]();


	obj_->worldTransform_.UpdateMatrixRotate(rotateMat_);
}

void Boss::Draw(const Camera& camera) {
#ifdef _DEBUG

	ShapesDraw::DrawSphere(collider_[attackMode_], camera, Vector4(1.0f, 0.0f, 0.0f, 1.0f));

#endif // _DEBUG


	obj_->Draw(camera);
}

void Boss::RootInit() {

	MeteorManager::GetInstance()->AttackFinish();
	CannonManager::GetInstance()->AttackFinish();
	StampManager::GetInstance()->AttackFinish();

}

void Boss::RootUpdate() {



}

void Boss::AttackInit() {

	if (attackMode_ == AttackMode::kHigh) {
		MeteorManager::GetInstance()->AttackStart(collider_[AttackMode::kHigh].center);
	}else if (attackMode_ == AttackMode::kMiddle) {
		CannonManager::GetInstance()->AttackStart(collider_[AttackMode::kMiddle].center);
	}else if (attackMode_ == AttackMode::kUnder) {
		StampManager::GetInstance()->AttackStart(collider_[AttackMode::kUnder].center);
	}
	

}

void Boss::AttackUpdate() {



}

void Boss::DeadInit() {



}

void Boss::DeadUpdate() {



}

void Boss::DownInit() {



}

void Boss::DownUpdate() {



}

void Boss::DebugGUI() {
#ifdef _DEBUG

	ImGui::Begin("Boss");

	
	const char* attackModeNames[] = { "High", "Middle", "Under" };
	int currentMode = static_cast<int>(attackMode_);

	if (ImGui::Combo("Attack Mode", &currentMode, attackModeNames, IM_ARRAYSIZE(attackModeNames))) {
		attackMode_ = static_cast<AttackMode>(currentMode);
		behaviorRequest_ = Behavior::kRoot;
	}


	if (ImGui::Button("Attack")) {
		behaviorRequest_ = Behavior::kAttack;
	}
	if (ImGui::Button("Root")) {
		behaviorRequest_ = Behavior::kRoot;
	}
	
	ImGui::DragFloat3("scale", &obj_->worldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("tranlation", &obj_->worldTransform_.translation_.x, 0.01f);

	ImGui::DragFloat3("High Collider center", &collider_[AttackMode::kHigh].center.x, 0.01f);
	ImGui::DragFloat("High Collider radius", &collider_[AttackMode::kHigh].radius, 0.01f);

	ImGui::DragFloat3("Middle Collider center", &collider_[AttackMode::kMiddle].center.x, 0.01f);
	ImGui::DragFloat("Middle Collider radius", &collider_[AttackMode::kMiddle].radius, 0.01f);

	ImGui::DragFloat3("Under Collider center", &collider_[AttackMode::kUnder].center.x, 0.01f);
	ImGui::DragFloat("Under Collider radius", &collider_[AttackMode::kUnder].radius, 0.01f);


	ImGui::End();

#endif // _DEBUG
}
