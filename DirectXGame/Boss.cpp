#include "Boss.h"

#include "MeteorManager.h"
#include "CannonManager.h"
#include "StampManager.h"
#include "ImGuiManager.h"
#include "ShapesDraw.h"
#include "TextureManager.h"
#include "RandomEngine.h"

void Boss::Init(const std::shared_ptr<Model>& model) {

	obj_.reset(Object3d::Create(model));

	hpBer_.reset(Sprite::Create(TextureManager::Load("Boss_HP.png"), {340.0f,30.0f},1.0f));
	hpBer_->SetAnchorpoint({ 0.0f,0.5f });

	hp_ = maxHp_;

	behaviorRequest_ = Behavior::kRoot;

	direction_ = { 0.0f,0.0f,-1.0f };

	rotateMat_ = DirectionToDirection({ 0.0f,0.0f,1.0f }, direction_);

	collider_[AttackMode::kHigh] = { Vector3(0.0f,1.0f,0.0f),4.0f };
	collider_[AttackMode::kMiddle] = { Vector3(0.0f,-10.0f,0.0f),15.0f };
	collider_[AttackMode::kUnder] = { Vector3(0.0f,-22.0f,0.0f),25.0f };

	MeteorManager::GetInstance()->SetTargetPos(collider_[AttackMode::kHigh].center);
	CannonManager::GetInstance()->SetTargetPos(collider_[AttackMode::kMiddle].center);

	obj_->worldTransform_.translation_ = { 0.0f,-32.0f,0.0f };
	obj_->worldTransform_.scale_ = Vector3(1.0f, 1.0f, 1.0f) * 1.5f;

	basePos_ = obj_->worldTransform_.translation_;

}

void Boss::Update() {
	DebugGUI();
	preAttackMode_ = attackMode_;

	if (behaviorRequest_) {

		behavior_ = behaviorRequest_.value();

		behaviorInitTable_[behavior_]();

		behaviorRequest_ = std::nullopt;
	}

	behaviorUpdateTable_[behavior_]();

	Shakeing();
	
	if (hp_ <= 0) {
		behaviorRequest_ = Behavior::kDead;
	}

	float percent = float(hp_) / (float)maxHp_;

	hpBer_->SetSize({ 25.0f * percent, 5.0f });

	obj_->worldTransform_.UpdateMatrixRotate(rotateMat_);
}

void Boss::Draw(const Camera& camera) {
#ifdef _DEBUG

	ShapesDraw::DrawSphere(collider_[attackMode_], camera, Vector4(1.0f, 0.0f, 0.0f, 1.0f));

#endif // _DEBUG

	if (hp_ > 0) {
		obj_->Draw(camera);
	}
	
}

void Boss::HitPlayerAttackCollision(const int32_t power){
	hp_ -= power;
	hp_ = std::clamp(hp_, 0, maxHp_);
	ShakeInit();
}

void Boss::DrawUI() {
	hpBer_->Draw();

}

void Boss::AttackHit() {
	hp_ -= 15;
	hp_ = std::clamp(hp_, 0, maxHp_);
	ShakeInit();
}

void Boss::ShakeInit() {

	shakeCount_ = 0;
	isShake_ = true;
	basePos_ = obj_->GetWorldPos();

}

void Boss::Shakeing() {

	if (isShake_) {

		shakeCount_++;
		shakeCount_ = std::clamp(shakeCount_, 0, shakeTime_);

		shakePos_.x = RandomEngine::GetFloatRandom(0.0f, 1.0f);
		shakePos_.y = RandomEngine::GetFloatRandom(0.0f, 1.0f);
		shakePos_.z = RandomEngine::GetFloatRandom(0.0f, 1.0f);
		
		if (shakeCount_ >= shakeTime_) {
			isShake_ = false;
			shakePos_ = {};
		}

	}

	obj_->worldTransform_.translation_ = basePos_ + shakePos_;

}

void Boss::RootInit() {

	MeteorManager::GetInstance()->AttackFinish();
	CannonManager::GetInstance()->AttackFinish();
	StampManager::GetInstance()->AttackFinish();

	count_ = 0;

}

void Boss::RootUpdate() {

	if (++count_ >= coolTime_) {
		behaviorRequest_ = Behavior::kAttack;
	}

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

	MeteorManager::GetInstance()->AttackFinish();
	CannonManager::GetInstance()->AttackFinish();
	StampManager::GetInstance()->AttackFinish();

	MeteorManager::GetInstance()->Reset();
	CannonManager::GetInstance()->Reset();
	StampManager::GetInstance()->Reset();


}

void Boss::DeadUpdate() {



}

void Boss::DownInit() {

	MeteorManager::GetInstance()->AttackFinish();
	CannonManager::GetInstance()->AttackFinish();
	StampManager::GetInstance()->AttackFinish();

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
