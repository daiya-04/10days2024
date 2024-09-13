#include "Stamp.h"

#include "Easing.h"
#include "ModelManager.h"

void Stamp::Init(const std::shared_ptr<Model>& model) {

	obj_.reset(Object3d::Create(model));
	warningZone_.reset(Object3d::Create(ModelManager::LoadOBJ("SquareWarningZone")));

	collider_.size = { 3.0f,0.5f,3.9f };

	isLife_ = false;
	preIsLife_ = false;

}

void Stamp::Update() {
	preIsLife_ = isLife_;
	preIsHit_ = isHit_;

	if (phaseRequest_) {

		phase_ = phaseRequest_.value();

		phaseInitTable_[phase_]();

		phaseRequest_ = std::nullopt;
	}

	phaseUpdateTable_[phase_]();

	obj_->worldTransform_.UpdateMatrixRotate(rotateMat_);
	warningZone_->worldTransform_.UpdateMatrixRotate(rotateMat_);
	UpdateCollider();
}

void Stamp::UpdateCollider() {

	collider_.orientation[0] = { rotateMat_.m[0][0], rotateMat_.m[0][1], rotateMat_.m[0][2] };
	collider_.orientation[1] = { rotateMat_.m[1][0], rotateMat_.m[1][1], rotateMat_.m[1][2] };
	collider_.orientation[2] = { rotateMat_.m[2][0], rotateMat_.m[2][1], rotateMat_.m[2][2] };

	collider_.center = obj_->GetWorldPos();

}

void Stamp::Draw(const Camera& camera) {
	if (!isLife_) { return; }
	obj_->Draw(camera);
	warningZone_->Draw(camera);
}

void Stamp::DrawParticle(const Camera& camera) {



}

void Stamp::Hit() {
	phaseRequest_ = Phase::kRoot;
	isLife_ = false;
	isHit_ = false;
}

void Stamp::Reset() {
	isLife_ = false;
	preIsLife_ = false;
	isHit_ = false;
	preIsHit_ = false;
	phaseRequest_ = Phase::kRoot;
}

void Stamp::AttackStart(const Vector3& startPos, const Vector3& direction) {

	direction_ = direction;
	obj_->worldTransform_.translation_ = startPos;
	attackData_.startPos_ = startPos;
	attackData_.impactPoint_ = startPos;
	attackData_.impactPoint_.y = -22.0f;
	phaseRequest_ = Phase::kCharge;
	isLife_ = true;

	warningZone_->worldTransform_.translation_ = attackData_.impactPoint_;
	warningZone_->worldTransform_.translation_.y += 0.1f;
	warningZone_->worldTransform_.scale_ = {};

	rotateMat_ = DirectionToDirection(Vector3(0.0f, 0.0f, 1.0f), direction_);

	obj_->worldTransform_.UpdateMatrixRotate(rotateMat_);
	warningZone_->worldTransform_.UpdateMatrixRotate(rotateMat_);
	UpdateCollider();
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

	chargeData_.param_ += 0.025f;
	chargeData_.param_ = std::clamp(chargeData_.param_, 0.0f, 1.5f);

	obj_->worldTransform_.scale_ = Lerp(chargeData_.param_, chargeData_.minScale_, chargeData_.maxScale_);
	warningZone_->worldTransform_.scale_ = Lerp(chargeData_.param_, Vector3(0.0f, 0.0f, 0.0f), Vector3(2.2f, 1.0f, 4.16f));

	if (chargeData_.param_ >= 1.5f) {
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
		isHit_ = true;
		if (++attackData_.count_ >= attackData_.coolTime_) {
			Hit();
		}
	}

}
