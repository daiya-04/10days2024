#pragma once
#include "ModelManager.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "Object3d.h"
#include "GPUParticle.h"
#include "CollisionShapes.h"

#include <memory>
#include <map>
#include <functional>
#include <optional>

class Meteor {
public:

	enum class Phase {
		kRoot,
		kAttack,
		kReflected,
	};

	Phase phase_ = Phase::kRoot;
	std::optional<Phase> phaseRequest_ = Phase::kRoot;

	std::map<Phase, std::function<void()>> phaseInitTable_{
		{Phase::kRoot,[this]() {RootInit(); }},
		{Phase::kAttack,[this]() {AttackInit(); }},
		{Phase::kReflected,[this]() {ReflectedInit(); }},
	};

	std::map<Phase, std::function<void()>> phaseUpdateTable_{
		{Phase::kRoot,[this]() {RootUpdate(); }},
		{Phase::kAttack,[this]() {AttackUpdate(); }},
		{Phase::kReflected,[this]() {ReflectedUpdate(); }},
	};

private:

	void RootInit();
	void RootUpdate();

	void AttackInit();
	void AttackUpdate();

	void ReflectedInit();
	void ReflectedUpdate();

public:

	void Init(const std::shared_ptr<Model>& model);

	void Update();
	void UpdateCollider() {
		collider_.center = obj_->GetWorldPos();
	}

	void Draw(const Camera& camera);

	void DrawParticle(const Camera& camera);

	void Reset();

	void Hit();

	void Reflection();

	void AttackStart(const Vector3& startPos);

	void SetTargetPos(const Vector3& targetPos) { targetPos_ = targetPos; }

	bool IsLife() const { return isLife_; }
	bool HitFlag() const { return !isLife_ && preIsLife_; }
	const uint32_t& GetDamage() const { return damage_; }

	Vector3 GetWorldPos() const { return obj_->GetWorldPos(); }
	Shapes::Sphere GetCollider() { return collider_; }

private:

	std::unique_ptr<Object3d> obj_;
	std::unique_ptr<Object3d> warningZone_;
	std::unique_ptr<GPUParticle> fireTrail_;

	Shapes::Sphere collider_;

	bool isLife_ = false;
	bool preIsLife_ = false;

	Vector3 velocity_{};

	Vector3 targetPos_{};
	Vector3 reflectDict_{};

	float zoneParam_ = 0.0f;

	uint32_t damage_ = 20;

};

