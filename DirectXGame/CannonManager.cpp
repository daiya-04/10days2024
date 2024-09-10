#include "CannonManager.h"


CannonManager* CannonManager::GetInstance() {
	static CannonManager instance;

	return &instance;
}

void CannonManager::Init(const std::shared_ptr<Model>& model) {

	for (auto& cannon : cannons_) {
		cannon = std::make_unique<Cannon>();
		cannon->Init(model);
	}

	float range = 14.0f;

	for (int32_t index = 0; index < 16; index++) {
		float angle = float(index) * (360.0f / 16.0f);
		angle = (angle / 180.0f) * std::numbers::pi_v<float>;
		offsets_[index] = Transform(Vector3(0.0f, 0.0f, -1.0f), MakeRotateAxisAngle(Vector3(0.0f, 1.0f, 0.0f), angle)).Normalize() * range;
	}

	isAttack_ = false;
	preIsAttack_ = false;
}

void CannonManager::Update() {
	preIsAttack_ = isAttack_;

	for (auto& cannon : cannons_) {
		cannon->Update();
	}

	if (!isAttack_) { return; }

	if (!cannons_[cannonIndex_]->IsLife()) {
		if (++count_ >= attackTime_) {
			cannons_[cannonIndex_]->AttackStart(basePos_ + offsets_[cannonIndex_], offsets_[cannonIndex_].Normalize());
			cannonIndex_ = (cannonIndex_ + 1) % 16;
			count_ = 0;
		}
	}

}

void CannonManager::Draw(const Camera& camera) {
	for (auto& cannon : cannons_) {
		cannon->Draw(camera);
	}
}

void CannonManager::DrawParticle(const Camera& camera) {
	for (auto& cannon : cannons_) {
		cannon->DrawParticle(camera);
	}


}

void CannonManager::AttackStart(const Vector3& basePos) {

	basePos_ = basePos;
	basePos_.y += 1.0f;
	isAttack_ = true;
	cannonIndex_ = 0;

}

void CannonManager::AttackFinish() {
	isAttack_ = false;
	count_ = 0;
}
