#include "Boss.h"

#include "MeteorManager.h"
#include "CannonManager.h"
#include "StampManager.h"
#include "ImGuiManager.h"

void Boss::Init(const std::shared_ptr<Model>& model) {

	obj_.reset(Object3d::Create(model));

	hp_ = maxHp_;

	behaviorRequest_ = Behavior::kRoot;

	direction_ = { 0.0f,0.0f,-1.0f };

	rotateMat_ = DirectionToDirection({ 0.0f,0.0f,1.0f }, direction_);

	obj_->worldTransform_.translation_ = { 0.0f,-5.0f,0.0f };

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
		MeteorManager::GetInstance()->AttackStart(obj_->GetWorldPos());
	}
	else if (attackMode_ == AttackMode::kMiddle) {
		CannonManager::GetInstance()->AttackStart(obj_->GetWorldPos());
	}else if (attackMode_ == AttackMode::kUnder) {
		StampManager::GetInstance()->AttackStart(obj_->GetWorldPos());
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
	



	ImGui::End();

#endif // _DEBUG
}
