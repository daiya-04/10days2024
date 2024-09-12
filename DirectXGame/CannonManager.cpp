#include "CannonManager.h"

#include "RandomEngine.h"

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
	Vector3 startAxis = Transform(Vector3(0.0f, 0.0f, 1.0f), MakeRotateAxisAngle(Vector3(0.0f, 1.0f, 0.0f), (11.25 / 180.0f) * std::numbers::pi_v<float>));

	for (int32_t index = 0; index < 16; index++) {
		float angle = float(index) * (360.0f / 16.0f);
		angle = (angle / 180.0f) * std::numbers::pi_v<float>;
		offsets_[index] = Transform(startAxis, MakeRotateAxisAngle(Vector3(0.0f, 1.0f, 0.0f), angle)).Normalize() * range;
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
	
	if (++count_ >= attackTime_) {
		while (true) {
			int32_t nextIndex = RandomEngine::GetIntRandom(0, 15);
			if (nextIndex != cannonIndex_ && !cannons_[nextIndex]->IsLife()) {
				cannonIndex_ = nextIndex;
				indexCount_ = 0;
				break;
			}
			indexCount_++;
			if (indexCount_ >= 16) { break; }
		}
		cannons_[cannonIndex_]->AttackStart(basePos_ + offsets_[cannonIndex_], offsets_[cannonIndex_].Normalize());
		count_ = 0;
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

void CannonManager::SetTargetPos(const Vector3& targetPos) {
	for (auto& cannon : cannons_) {
		cannon->SetTargetPos(targetPos);
	}
}
