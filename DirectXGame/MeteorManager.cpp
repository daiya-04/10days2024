#include "MeteorManager.h"
#include "TextureManager.h"
#include "RandomEngine.h"

#include <numbers>


MeteorManager* MeteorManager::GetInstance() {
	static MeteorManager instance;

	return &instance;
}

void MeteorManager::Init(const std::shared_ptr<Model>& model) {

	for (auto& meteor : meteors_) {
		meteor = std::make_unique<Meteor>();
		meteor->Init(model);
	}

	hitEff_.reset(GPUParticle::Create(TextureManager::Load("circle.png"), 50000));

	float range = 30.0f;

	for (int32_t index = 0; index < 16; index++) {
		float angle = float(index) * (360.0f / 16.0f);
		angle = (angle / 180.0f) * std::numbers::pi_v<float>;
		offsets_[index] = Transform(Vector3(0.0f, 0.0f, -1.0f), MakeRotateAxisAngle(Vector3(0.0f, 1.0f, 0.0f), angle)).Normalize() * range;
	}


	hitEff_->isLoop_ = false;

	hitEff_->emitter_.count = 10000;
	hitEff_->emitter_.direction = Vector3(0.0f, 1.0f, 0.0f);
	hitEff_->emitter_.angle = 180.0f;
	hitEff_->emitter_.color = Vector4(0.89f, 0.27f, 0.03f, 1.0f);
	hitEff_->emitter_.emit = 0;
	hitEff_->emitter_.lifeTime = 1.0f;
	hitEff_->emitter_.speed = 10.0f;
	hitEff_->emitter_.scale = 0.5f;
	hitEff_->emitter_.size = { 0.01f,0.01f,0.01f };
	

	isAttack_ = false;
	preIsAttack_ = false;

	meteorIndex_ = 0;

}

void MeteorManager::Update() {
	preIsAttack_ = isAttack_;

	for (auto& meteor : meteors_) {
		meteor->Update();
		if (meteor->HitFlag()) {
			hitEff_->emitter_.emit = 1;
			hitEff_->emitter_.translate = meteor->GetWorldPos();
		}
	}

	hitEff_->Update();




	if (!isAttack_) { return; }

	if (++count_ >= attackTime_) {
		while (true) {
			int32_t nextIndex = RandomEngine::GetIntRandom(0, 15);
			if (nextIndex != meteorIndex_ && !meteors_[nextIndex]->IsLife()) {
				meteorIndex_ = nextIndex;
				indexCount_ = 0;
				break;
			}
			indexCount_++;
			if (indexCount_ >= 16) { break; }
		}
		meteors_[meteorIndex_]->AttackStart(basePos_ + offsets_[meteorIndex_]);
		count_ = 0;
	}
	


}

void MeteorManager::Draw(const Camera& camera) {

	for (auto& meteor : meteors_) {
		meteor->Draw(camera);
	}

}

void MeteorManager::DrawParticle(const Camera& camera) {

	for (auto& meteor : meteors_) {
		meteor->DrawParticle(camera);
	}
	//hitEff_->Draw(camera);

}

void MeteorManager::AttackStart(const Vector3& basePos) {

	isAttack_ = true;
	basePos_ = basePos;
	basePos_.y += startHight_;

	meteors_[meteorIndex_]->AttackStart(basePos_ + offsets_[meteorIndex_]);
	meteorIndex_ = (meteorIndex_ + 1) % 16;

}

void MeteorManager::AttackFinish() {
	isAttack_ = false;
	count_ = 0;
}

void MeteorManager::SetTargetPos(const Vector3& targetPos) {
	for (auto& meteor : meteors_) {
		meteor->SetTargetPos(targetPos);
	}
}
