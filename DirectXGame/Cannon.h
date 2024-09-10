#pragma once
#include "ModelManager.h"
#include "Camera.h"
#include "WorldTransform.h"
#include "Object3d.h"
#include "CollisionShapes.h"
#include "GPUParticle.h"
#include "Vec3.h"

#include <memory>
#include <map>
#include <functional>
#include <optional>

class Cannon {
public:

	enum class Phase {
		kRoot,
		kCharge,
		kShoot,
	};

	Phase phase_ = Phase::kRoot;
	std::optional<Phase> phaseRequest_ = Phase::kRoot;

	std::map<Phase, std::function<void()>> phaseInitTable_{
		{Phase::kRoot,[this]() { RootInit(); }},
		{Phase::kCharge,[this]() {ChargeInit(); }},
		{Phase::kShoot,[this]() {ShootInit(); }},
	};

	std::map<Phase, std::function<void()>> phaseUpdateTable_{
		{Phase::kRoot,[this]() {RootUpdate(); }},
		{Phase::kCharge,[this]() {ChargeUpdate(); }},
		{Phase::kShoot,[this]() {ShootUpdate(); }},
	};

	struct ChargeData {
		float param_ = 0.0f;
		Vector3 minScale_ = { 0.0f, 0.0f, 0.0f };
		Vector3 maxScale_ = { 2.0f, 2.0f, 2.0f };
	};

	struct ShootData {
		float param_ = 0.0f;
		Vector3 startPoint_{};
		Vector3 controlPoint_{};
		Vector3 impactPoint_{};
		float impactPointRange_ = 16.0f;
		float controlPointHeight_ = 12.0f;
		float impactPointHeight_ = -3.0f;
	};

	ChargeData chargeData_;
	ShootData shootData_;


private:

	void RootInit();
	void RootUpdate();

	void ChargeInit();
	void ChargeUpdate();

	void ShootInit();
	void ShootUpdate();

	
public:

	void Init(const std::shared_ptr<Model>& model);

	void Update();
	void UpdateCollider() {
		collider_.center = obj_->GetWorldPos();
	}

	void Draw(const Camera& camera);

	void DrawParticle(const Camera& camera);

	void AttackStart(const Vector3& pos, const Vector3& direction);

	bool IsLife() const { return isLife_; }
	bool HitFlag() const { return !isLife_ && preIsLife_; }

	Vector3 GetWorldPos() const { return obj_->GetWorldPos(); }
	Shapes::Sphere GEtCollider() { return collider_; }

private:

	std::unique_ptr<Object3d> obj_;
	std::unique_ptr<GPUParticle> trail_;

	Shapes::Sphere collider_;

	bool isLife_ = false;
	bool preIsLife_ = false;

};

