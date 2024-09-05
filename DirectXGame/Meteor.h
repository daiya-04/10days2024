#pragma once
#include "WorldTransform.h"
#include "Camera.h"
#include "Object3d.h"
#include "GPUParticle.h"
#include "CollisionShapes.h"

#include <memory>
#include <map>
#include <functional>

class Meteor {
public:

	enum Phase {
		kRoot,
		kAttack,
	};

	Phase phase_ = Phase::kRoot;
	std::optional<Phase> phaseRequest_ = Phase::kRoot;

	std::map<Phase, std::function<void()>> phaseInitTable_{
		{Phase::kRoot,[this]() {RootInit(); }},
		{Phase::kAttack,[this]() {AttackInit(); }},
	};

	std::map<Phase, std::function<void()>> phaseUpdateTable_{
		{Phase::kRoot,[this]() {RootUpdate(); }},
		{Phase::kAttack,[this]() {AttackUpdate(); }},
	};

private:

	void RootInit();
	void RootUpdate();

	void AttackInit();
	void AttackUpdate();

public:

	void Init(const std::shared_ptr<Model>& model);

	void Update();

	void Draw(const Camera& camera);

	void DrawParticle(const Camera& camera);

	void AttackStart(const Vector3& startPos);

	bool IsLife() const { return isLife_; }

private:

	std::unique_ptr<Object3d> obj_;

	Shapes::Sphere collider_;

	bool isLife_ = false;
	bool preIsLife_ = false;

	Vector3 velocity_{};


};

