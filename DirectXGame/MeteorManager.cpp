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
	Vector3 startAxis = Transform(Vector3(0.0f, 0.0f, 1.0f), MakeRotateAxisAngle(Vector3(0.0f, 1.0f, 0.0f), (11.25 / 180.0f) * std::numbers::pi_v<float>));

	for (int32_t index = 0; index < 16; index++) {
		float angle = float(index) * (360.0f / 16.0f);
		angle = (angle / 180.0f) * std::numbers::pi_v<float>;
		offsets_[index] = Transform(startAxis, MakeRotateAxisAngle(Vector3(0.0f, 1.0f, 0.0f), angle)).Normalize() * range;
	}


	hitEff_->isLoop_ = false;

	hitEff_->emitter_.count = 10000;
	hitEff_->emitter_.direction = Vector3(0.0f, 1.0f, 0.0f);
	hitEff_->emitter_.angle = 360.0f;
	hitEff_->emitter_.color = Vector4(0.89f, 0.27f, 0.03f, 1.0f);
	hitEff_->emitter_.emit = 0;
	hitEff_->emitter_.lifeTime = 0.4f;
	hitEff_->emitter_.speed = 50.0f;
	hitEff_->emitter_.scale = 0.5f;
	hitEff_->emitter_.size = { 0.01f,0.01f,0.01f };
	hitEff_->emitter_.isHalf = 1;
	

	isAttack_ = false;
	preIsAttack_ = false;

}

void MeteorManager::Update() {
	preIsAttack_ = isAttack_;

	for (auto& meteor : meteors_) {
		if (meteor->HitFlag()) {
			hitEff_->emitter_.emit = 1;
			hitEff_->emitter_.translate = meteor->GetWorldPos();
			hitEff_->emitter_.translate.y = -2.0f;
		}
		meteor->Update();
	}

	hitEff_->Update();

	if (!isAttack_) { return; }

	for (int32_t index : indexList_) {
		if (!pieceAlives_[index]) {
			indexList_.remove(index);
			break;
		}
	}

	if (++count_ >= attackTime_) {
		while (true) {
			if (indexList_.empty()) { break; }

			int32_t itNum = RandomEngine::GetIntRandom(0, (int)indexList_.size() - 1);
			indexIt_ = indexList_.begin();
			std::advance(indexIt_, itNum);
			int32_t nextIndex = *indexIt_;

			if (!meteors_[nextIndex]->IsLife() && pieceAlives_[nextIndex]) {
				meteors_[nextIndex]->AttackStart(basePos_ + offsets_[nextIndex]);
				indexCount_ = 0;
				//attackTime_ = RandomEngine::GetIntRandom(30, 40);
				break;
			}

			indexCount_++;
			if (indexCount_ >= indexList_.size()) { break; }
		}
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
	hitEff_->Draw(camera);

}

void MeteorManager::Hit(uint32_t index) {
	meteors_[index]->Hit();
}

void MeteorManager::Reset() {
	for (auto& meteor : meteors_) {
		meteor->Reset();
	}
}

void MeteorManager::AttackStart(const Vector3& basePos) {

	isAttack_ = true;
	basePos_ = basePos;
	basePos_.y += startHight_;

	indexList_.assign({ 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 });

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
