#include "MeteorManager.h"
#include "TextureManager.h"


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

	float range = 5.0f;

	offsets_[0] = Vector3(0.0f, 0.0f, -1.0f).Normalize() * range;
	offsets_[1] = Vector3(1.0f, 0.0f, -1.0f).Normalize() * range;
	offsets_[2] = Vector3(1.0f, 0.0f, 0.0f).Normalize() * range;
	offsets_[3] = Vector3(1.0f, 0.0f, 1.0f).Normalize() * range;
	offsets_[4] = Vector3(0.0f, 0.0f, 1.0f).Normalize() * range;
	offsets_[5] = Vector3(-1.0f, 0.0f, 1.0f).Normalize() * range;
	offsets_[6] = Vector3(-1.0f, 0.0f, 0.0f).Normalize() * range;
	offsets_[7] = Vector3(-1.0f, 0.0f, -1.0f).Normalize() * range;


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

	if (!meteors_[meteorIndex_]->IsLife()) {
		if (++count_ >= attackTime_) {
			meteors_[meteorIndex_]->AttackStart(basePos_ + offsets_[meteorIndex_]);
			meteorIndex_ = (meteorIndex_ + 1) % 8;
			count_ = 0;
		}
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
	basePos_.y = startHight_;
	meteorIndex_ = 0;

	meteors_[meteorIndex_]->AttackStart(basePos_ + offsets_[meteorIndex_]);
	meteorIndex_ = (meteorIndex_ + 1) % 8;

}

void MeteorManager::AttackFinish() {
	isAttack_ = false;
	count_ = 0;
}
