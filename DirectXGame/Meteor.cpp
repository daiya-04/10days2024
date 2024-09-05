#include "Meteor.h"
#include "TextureManager.h"

void Meteor::Init(const std::shared_ptr<Model>& model) {

	obj_.reset(Object3d::Create(model));

	velocity_ = { 0.0f,-0.1f,0.0f };

	isLife_ = false;
	preIsLife_ = false;

}

void Meteor::Update() {
	preIsLife_ = isLife_;

	if (phaseRequest_) {

		phase_ = phaseRequest_.value();

		phaseInitTable_[phase_]();

		phaseRequest_ = std::nullopt;
	}

	phaseUpdateTable_[phase_]();

	obj_->worldTransform_.UpdateMatrix();
}

void Meteor::Draw(const Camera& camera) {
	if (!isLife_) { return; }
	obj_->Draw(camera);
}

void Meteor::DrawParticle(const Camera& camera) {



}

void Meteor::AttackStart(const Vector3& startPos) {

	obj_->worldTransform_.translation_ = startPos;
	phaseRequest_ = Phase::kAttack;
	isLife_ = true;

	obj_->worldTransform_.UpdateMatrix();
}

void Meteor::RootInit() {

	

}

void Meteor::RootUpdate() {



}

void Meteor::AttackInit() {



}

void Meteor::AttackUpdate() {

	obj_->worldTransform_.translation_ += velocity_;


	if (obj_->worldTransform_.translation_.y <= 0.0f) {
		isLife_ = false;
		phaseRequest_ = Phase::kRoot;
	}

}
