#include "Stamp.h"

#include "Easing.h"

void Stamp::Init(const std::shared_ptr<Model>& model) {

	obj_.reset(Object3d::Create(model));


	isLife_ = false;
	preIsLife_ = false;

}

void Stamp::Update() {
	preIsLife_ = isLife_;

	if (phaseRequest_) {

		phase_ = phaseRequest_.value();

		phaseInitTable_[phase_]();

		phaseRequest_ = std::nullopt;
	}

	phaseUpdateTable_[phase_]();

	obj_->worldTransform_.UpdateMatrixRotate(rotateMat_);
}

void Stamp::Draw(const Camera& camera) {

	obj_->Draw(camera);

}

void Stamp::DrawParticle(const Camera& camera) {



}

void Stamp::AttackStart(const Vector3& startPos, const Vector3& direction) {

	direction_ = direction;
	obj_->worldTransform_.translation_ = startPos;
	attackData_.startPos_ = startPos;
	attackData_.impactPoint_ = startPos;
	attackData_.impactPoint_.y = -12.0f;
	phaseRequest_ = Phase::kCharge;
	isLife_ = true;

	rotateMat_ = DirectionToDirection(Vector3(0.0f, 0.0f, 1.0f), direction_);

	obj_->worldTransform_.UpdateMatrixRotate(rotateMat_);
}

void Stamp::RootInit() {

	obj_->worldTransform_.scale_ = {};

}

void Stamp::RootUpdate() {



}

void Stamp::ChargeInit() {

	chargeData_.param_ = 0.0f;

}

void Stamp::ChargeUpdate() {

	chargeData_.param_ += 0.005f;
	chargeData_.param_ = std::clamp(chargeData_.param_, 0.0f, 1.0f);

	obj_->worldTransform_.scale_ = Lerp(chargeData_.param_, chargeData_.minScale_, chargeData_.maxScale_);

	if (chargeData_.param_ >= 1.0f) {
		phaseRequest_ = Phase::kAttack;
	}

}

void Stamp::AttackInit() {

	attackData_.param_ = 0.0f;
	attackData_.count_ = 0;

}

void Stamp::AttackUpdate() {

	attackData_.param_ += 0.1f;

	obj_->worldTransform_.translation_ = Lerp(attackData_.param_, attackData_.startPos_, attackData_.impactPoint_);

	if (attackData_.param_ >= 1.0f) {
		if (++attackData_.count_ >= attackData_.coolTime_) {
			phaseRequest_ = Phase::kRoot;
			isLife_ = false;
		}
	}

}
