#include "Boss.h"

#include "MeteorManager.h"
#include "ImGuiManager.h"

void Boss::Init(const std::shared_ptr<Model>& model) {

	obj_.reset(Object3d::Create(model));

	hp_ = maxHp_;

	behaviorRequest_ = Behavior::kRoot;

	direction_ = { 0.0f,0.0f,-1.0f };

	rotateMat_ = DirectionToDirection({ 0.0f,0.0f,1.0f }, direction_);

	obj_->worldTransform_.translation_ = { 0.0f,0.0f,0.0f };

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

}

void Boss::RootUpdate() {



}

void Boss::AttackInit() {

	MeteorManager::GetInstance()->AttackStart(obj_->GetWorldPos());

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

	if (ImGui::Button("Attack")) {
		behaviorRequest_ = Behavior::kAttack;
	}
	if (ImGui::Button("Root")) {
		behaviorRequest_ = Behavior::kRoot;
	}

	ImGui::End();

#endif // _DEBUG
}
