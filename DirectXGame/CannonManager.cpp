#include "CannonManager.h"

#include "TextureManager.h"
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
	hitEff_.reset(GPUParticle::Create(TextureManager::Load("circle.png"), 30000));

	float range = 14.0f;
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

void CannonManager::Update() {
	preIsAttack_ = isAttack_;

	for (auto& cannon : cannons_) {
		if (cannon->HitFlag()) {
			hitEff_->emitter_.emit = 1;
			hitEff_->emitter_.translate = cannon->GetWorldPos();
			hitEff_->emitter_.translate.y = -12.0f;
		}
		cannon->Update();
	}

	hitEff_->Update();

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
	hitEff_->Draw(camera);

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

void CannonManager::Reset() {
	for (auto& cannon : cannons_) {
		cannon->Reset();
	}
}
