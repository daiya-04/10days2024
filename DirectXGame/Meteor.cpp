#include "Meteor.h"
#include "TextureManager.h"

void Meteor::Init(const std::shared_ptr<Model>& model) {

	obj_.reset(Object3d::Create(model));

	fireTrail_.reset(GPUParticle::Create(TextureManager::Load("circle.png"), 10000));

	fireTrail_->isLoop_ = true;

	
	fireTrail_->emitter_.color = Vector4(0.89f, 0.27f, 0.03f, 1.0f);
	fireTrail_->emitter_.direction = Vector3(0.0f, 1.0f, 0.0f);
	fireTrail_->emitter_.angle = 360.0f;
	fireTrail_->emitter_.frequency = 1.0f / 60.0f;
	fireTrail_->emitter_.lifeTime = 20.0f / 60.0f;
	fireTrail_->emitter_.scale = 0.5f;
	fireTrail_->emitter_.size = Vector3(1.3f, 1.3f, 1.3f);
	fireTrail_->emitter_.speed = 0.0f;

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

	fireTrail_->Update();

	obj_->worldTransform_.UpdateMatrix();

	fireTrail_->emitter_.translate = obj_->GetWorldPos();

}

void Meteor::Draw(const Camera& camera) {
	if (!isLife_) { return; }
	obj_->Draw(camera);
}

void Meteor::DrawParticle(const Camera& camera) {

	fireTrail_->Draw(camera);

}

void Meteor::AttackStart(const Vector3& startPos) {

	obj_->worldTransform_.translation_ = startPos;
	phaseRequest_ = Phase::kAttack;
	isLife_ = true;

	obj_->worldTransform_.UpdateMatrix();
	fireTrail_->emitter_.translate = obj_->GetWorldPos();
}

void Meteor::RootInit() {

	fireTrail_->emitter_.count = 0;

}

void Meteor::RootUpdate() {

	fireTrail_->emitter_.frequencyTime = 0.0f;

}

void Meteor::AttackInit() {

	fireTrail_->emitter_.count = 1000;

}

void Meteor::AttackUpdate() {

	obj_->worldTransform_.translation_ += velocity_;


	if (obj_->worldTransform_.translation_.y <= 0.0f) {
		isLife_ = false;
		phaseRequest_ = Phase::kRoot;
	}

}
