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

	float range = 1.0f;

	offsets_[0] = Vector3(0.0f, 0.0f, -1.0f).Normalize() * range;
	offsets_[1] = Vector3(1.0f, 0.0f, -1.0f).Normalize() * range;
	offsets_[2] = Vector3(1.0f, 0.0f, 0.0f).Normalize() * range;
	offsets_[3] = Vector3(1.0f, 0.0f, 1.0f).Normalize() * range;
	offsets_[4] = Vector3(0.0f, 0.0f, 1.0f).Normalize() * range;
	offsets_[5] = Vector3(-1.0f, 0.0f, 1.0f).Normalize() * range;
	offsets_[6] = Vector3(-1.0f, 0.0f, 0.0f).Normalize() * range;
	offsets_[7] = Vector3(-1.0f, 0.0f, -1.0f).Normalize() * range;

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
			cannonIndex_ = (cannonIndex_ + 1) % 8;
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
	isAttack_ = true;
	cannonIndex_ = 0;

}

void CannonManager::AttackFinish() {
	isAttack_ = false;
	count_ = 0;
}
