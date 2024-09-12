#include "Meteor.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ShapesDraw.h"
#include "ImGuiManager.h"

void Meteor::Init(const std::shared_ptr<Model>& model) {

	obj_.reset(Object3d::Create(model));
	warningZone_.reset(Object3d::Create(ModelManager::LoadOBJ("WarningZone")));

	fireTrail_.reset(GPUParticle::Create(TextureManager::Load("circle.png"), 20000));

	fireTrail_->isLoop_ = true;

	obj_->worldTransform_.scale_ = Vector3(1.0f, 1.0f, 1.0f) * 2.0f;
	collider_.radius = 2.0f;
	
	fireTrail_->emitter_.color = Vector4(0.89f, 0.27f, 0.03f, 1.0f);
	fireTrail_->emitter_.direction = Vector3(0.0f, 1.0f, 0.0f);
	fireTrail_->emitter_.angle = 360.0f;
	fireTrail_->emitter_.frequency = 1.0f / 60.0f;
	fireTrail_->emitter_.lifeTime = 60.0f / 60.0f;
	fireTrail_->emitter_.scale = 0.5f;
	fireTrail_->emitter_.size = Vector3(1.0f, 1.0f, 1.0f) * 3.5f;
	fireTrail_->emitter_.speed = 0.0f;
	fireTrail_->emitter_.count = 1000;

	velocity_ = { 0.0f,-0.1f,0.0f };

	isLife_ = false;
	preIsLife_ = false;
	
	damage_ = 10;
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
	warningZone_->worldTransform_.UpdateMatrix();

	fireTrail_->emitter_.translate = obj_->GetWorldPos();

	UpdateCollider();
}

void Meteor::Draw(const Camera& camera) {
	
	if (!isLife_) { return; }
	obj_->Draw(camera);
	warningZone_->Draw(camera);
	
}

void Meteor::DrawParticle(const Camera& camera) {

	//fireTrail_->Draw(camera);

}

void Meteor::Hit() {

	isLife_ = false;
	phaseRequest_ = Phase::kRoot;

}

void Meteor::Reflection() {
	phaseRequest_ = Phase::kReflected;
}

void Meteor::AttackStart(const Vector3& startPos) {

	obj_->worldTransform_.translation_ = startPos;
	warningZone_->worldTransform_.translation_ = startPos;
	warningZone_->worldTransform_.translation_.y = -2.1f;
	warningZone_->worldTransform_.scale_ = {};
	phaseRequest_ = Phase::kAttack;
	isLife_ = true;

	obj_->worldTransform_.UpdateMatrix();
	warningZone_->worldTransform_.UpdateMatrix();
	fireTrail_->emitter_.translate = obj_->GetWorldPos();
	UpdateCollider();
}

void Meteor::RootInit() {

	fireTrail_->isLoop_ = false;

}

void Meteor::RootUpdate() {

	

}

void Meteor::AttackInit() {

	fireTrail_->emitter_.count = 1000;
	fireTrail_->emitter_.frequencyTime = 0.0f;
	fireTrail_->isLoop_ = true;

	
	zoneParam_ = 0.0f;

}

void Meteor::AttackUpdate() {

	zoneParam_ += 0.05f;
	zoneParam_ = std::clamp(zoneParam_, 0.0f, 1.0f);

	obj_->worldTransform_.translation_ += velocity_;

	warningZone_->worldTransform_.scale_ = Lerp(zoneParam_, Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f) * 2.5f);

	if (obj_->worldTransform_.translation_.y <= 0.0f) {
		Hit();
	}

}

void Meteor::ReflectedInit() {

	reflectDict_ = (targetPos_ - obj_->worldTransform_.translation_).Normalize();
	warningZone_->worldTransform_.scale_ = {};

}

void Meteor::ReflectedUpdate() {
	float speed = 0.3f;

	obj_->worldTransform_.translation_ += reflectDict_ * speed;

	

}
