#include "Cannon.h"

#include "TextureManager.h"


void Cannon::Init(const std::shared_ptr<Model>& model) {

	obj_.reset(Object3d::Create(model));
	trail_.reset(GPUParticle::Create(TextureManager::Load("circle.png"), 5000));

	trail_->isLoop_ = true;

	trail_->emitter_.count = 100;
	trail_->emitter_.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	trail_->emitter_.direction = Vector3(0.0f, 1.0f, 1.0f);
	trail_->emitter_.angle = 360.0f;
	trail_->emitter_.frequency = 1.0f / 60.0f;
	trail_->emitter_.lifeTime = 40.0f / 60.0f;
	trail_->emitter_.scale = 0.5f;
	trail_->emitter_.size = Vector3(0.3f, 0.3f, 0.3f);
	trail_->emitter_.speed = 0.0f;


	isLife_ = false;
}

void Cannon::Update() {

	if (phaseRequest_) {

		phase_ = phaseRequest_.value();

		phaseInitTable_[phase_]();

		phaseRequest_ = std::nullopt;
	}

	phaseUpdateTable_[phase_]();

	trail_->Update();

	obj_->worldTransform_.UpdateMatrix();
	trail_->emitter_.translate = obj_->GetWorldPos();
}

void Cannon::Draw(const Camera& camera) {
	if (!isLife_) { return; }
	obj_->Draw(camera);
}

void Cannon::DrawParticle(const Camera& camera) {

	trail_->Draw(camera);

}

void Cannon::AttackStart(const Vector3& pos, const Vector3& direction) {

	obj_->worldTransform_.translation_ = pos;
	obj_->worldTransform_.scale_ = {};
	isLife_ = true;

	shootData_.startPoint_ = pos;
	shootData_.impactPoint_ = pos;
	shootData_.impactPoint_ += direction * shootData_.impactPointRange_;
	shootData_.impactPoint_.y += shootData_.impactPointHeight_;
	shootData_.controlPoint_ = pos;
	shootData_.controlPoint_ += direction * 8.0f;
	shootData_.controlPoint_.y += shootData_.controlPointHeight_;

	collider_.radius = 1.0f;

	phaseRequest_ = Phase::kCharge;

	obj_->worldTransform_.UpdateMatrix();
}

void Cannon::RootInit() {

	trail_->isLoop_ = false;

}

void Cannon::RootUpdate() {



}

void Cannon::ChargeInit() {

	chargeData_.param_ = 0.0f;

}

void Cannon::ChargeUpdate() {

	chargeData_.param_ += 0.01f;
	chargeData_.param_ = std::clamp(chargeData_.param_, 0.0f, 1.0f);

	obj_->worldTransform_.scale_ = Lerp(chargeData_.param_, chargeData_.minScale_, chargeData_.maxScale_);

	if (chargeData_.param_ >= 1.0f) {
		phaseRequest_ = Phase::kShoot;
	}

}

void Cannon::ShootInit() {

	shootData_.param_ = 0.0f;
	obj_->worldTransform_.scale_ = { 1.0f,1.0f,1.0f };
	trail_->isLoop_ = true;

}

void Cannon::ShootUpdate() {

	shootData_.param_ += 0.01f;
	shootData_.param_ = std::clamp(shootData_.param_, 0.0f, 1.0f);

	obj_->worldTransform_.translation_ = Bazier(shootData_.startPoint_, shootData_.controlPoint_, shootData_.impactPoint_,shootData_.param_);

	if (shootData_.param_ >= 1.0f) {
		phaseRequest_ = Phase::kRoot;
		isLife_ = false;
	}

}
