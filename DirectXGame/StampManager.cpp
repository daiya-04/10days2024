#include "StampManager.h"

#include "RandomEngine.h"
#include "TextureManager.h"

StampManager* StampManager::GetInstance() {
	static StampManager instance;

	return &instance;
}

void StampManager::Init(const std::shared_ptr<Model>& model) {

	for (auto& stamp : stamps_) {
		stamp = std::make_unique<Stamp>();
		stamp->Init(model);
	}
	hitEff_.reset(GPUParticle::Create(TextureManager::Load("circle.png"), 30000));

	float range = 30.0f;
	Vector3 startAxis = Transform(Vector3(0.0f, 0.0f, 1.0f), MakeRotateAxisAngle(Vector3(0.0f, 1.0f, 0.0f), (11.25 / 180.0f) * std::numbers::pi_v<float>));

	for (int32_t index = 0; index < 16; index++) {
		float angle = float(index) * (360.0f / 16.0f);
		angle = (angle / 180.0f) * std::numbers::pi_v<float>;
		offsets_[index] = Transform(startAxis, MakeRotateAxisAngle(Vector3(0.0f, 1.0f, 0.0f), angle)).Normalize() * range;
	}

	hitEff_->isLoop_ = false;

	hitEff_->emitter_.count = 5000;
	hitEff_->emitter_.direction = Vector3(0.0f, 1.0f, 0.0f);
	hitEff_->emitter_.angle = 360.0f;
	hitEff_->emitter_.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	hitEff_->emitter_.emit = 0;
	hitEff_->emitter_.lifeTime = 0.4f;
	hitEff_->emitter_.speed = 50.0f;
	hitEff_->emitter_.scale = 0.5f;
	hitEff_->emitter_.size = { 0.01f,0.01f,0.01f };
	hitEff_->emitter_.isHalf = 1;

	isAttack_ = false;
	preIsAttack_ = false;

}

void StampManager::Update() {
	preIsAttack_ = isAttack_;

	for (auto& stamp : stamps_) {
		if (stamp->HitFlag()) {
			hitEff_->emitter_.emit = 1;
			hitEff_->emitter_.translate = stamp->GetWorldPos();
			hitEff_->emitter_.translate.y = -22.0f;
		}
		stamp->Update();
	}

	hitEff_->Update();

	if (!isAttack_) { return; }

	if (++count_ >= attackTime_) {
		while (true) {
			int32_t nextIndex = RandomEngine::GetIntRandom(0, 15);
			if (nextIndex != stampIndex_ && !stamps_[nextIndex]->IsLife()) {
				stampIndex_ = nextIndex;
				indexCount_ = 0;
				break;
			}
			indexCount_++;
			if (indexCount_ >= 16) { break; }
		}
		stamps_[stampIndex_]->AttackStart(basePos_ + offsets_[stampIndex_], offsets_[stampIndex_].Normalize());
		count_ = 0;
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
	hitEff_->Draw(camera);

}

void StampManager::AttackStart(const Vector3& basePos) {

	isAttack_ = true;
	basePos_ = basePos;
	basePos_.y += startHeight_;

}

void StampManager::AttackFinish() {
	isAttack_ = false;
	stampIndex_ = 0;
}

void StampManager::Reset() {
	for (auto& stamp : stamps_) {
		stamp->Reset();
	}
}
