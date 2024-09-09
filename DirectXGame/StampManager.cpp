#include "StampManager.h"


StampManager* StampManager::GetInstance() {
	static StampManager instance;

	return &instance;
}

void StampManager::Init(const std::shared_ptr<Model>& model) {

	for (auto& stamp : stamps_) {
		stamp = std::make_unique<Stamp>();
		stamp->Init(model);
	}

	float range = 8.0f;

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

void StampManager::Update() {
	preIsAttack_ = isAttack_;

	for (auto& stamp : stamps_) {
		stamp->Update();
	}

	if (!isAttack_) { return; }

	if (!stamps_[stampIndex_]->IsLife()) {
		if (++count_ >= attackTime_) {
			stamps_[stampIndex_]->AttackStart(basePos_ + offsets_[stampIndex_],offsets_[stampIndex_].Normalize());
			stampIndex_ = (stampIndex_ + 1) % 8;
			count_ = 0;
		}
	}

}

void StampManager::Draw(const Camera& camera) {

	for (auto& stamp : stamps_) {
		stamp->Draw(camera);
	}

}

void StampManager::DrawParticle(const Camera& camera) {

	for (auto& stamp : stamps_) {
		stamp->DrawParticle(camera);
	}

}

void StampManager::AttackStart(const Vector3& basePos) {

	isAttack_ = true;
	basePos_ = basePos;
	basePos_.y = startHeight_;

}

void StampManager::AttackFinish() {
	isAttack_ = false;
	stampIndex_ = 0;
}
