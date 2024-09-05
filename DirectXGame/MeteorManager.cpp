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

	float dict = 5.0f;

	offset_[0] = Vector3(0.0f, 0.0f, -1.0f).Normalize() * dict;
	offset_[1] = Vector3(1.0f, 0.0f, -1.0f).Normalize() * dict;
	offset_[2] = Vector3(1.0f, 0.0f, 0.0f).Normalize() * dict;
	offset_[3] = Vector3(1.0f, 0.0f, 1.0f).Normalize() * dict;
	offset_[4] = Vector3(0.0f, 0.0f, 1.0f).Normalize() * dict;
	offset_[5] = Vector3(-1.0f, 0.0f, 1.0f).Normalize() * dict;
	offset_[6] = Vector3(-1.0f, 0.0f, 0.0f).Normalize() * dict;
	offset_[7] = Vector3(-1.0f, 0.0f, -1.0f).Normalize() * dict;
	

	isAttack_ = false;
	preIsAttack_ = false;

	meteorIndex_ = 0;

}

void MeteorManager::Update() {
	preIsAttack_ = isAttack_;

	for (auto& meteor : meteors_) {
		meteor->Update();
	}

	if (!isAttack_) { return; }

	if (!meteors_[meteorIndex_]->IsLife()) {
		if (++count_ >= attackTime_) {
			meteors_[meteorIndex_]->AttackStart(basePos_ + offset_[meteorIndex_]);
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



}

void MeteorManager::AttackStart(const Vector3& basePos) {

	isAttack_ = true;
	basePos_ = basePos;
	basePos_.y = startHight_;
	meteorIndex_ = 0;

	meteors_[meteorIndex_]->AttackStart(basePos_ + offset_[meteorIndex_]);
	meteorIndex_ = (meteorIndex_ + 1) % 8;

}

void MeteorManager::AttackFinish() {
	isAttack_ = false;
	count_ = 0;
}
