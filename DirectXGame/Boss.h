#pragma once
#include "WorldTransform.h"
#include "Object3d.h"
#include "ModelManager.h"
#include "Camera.h"
#include "Animation.h"
#include "CollisionShapes.h"

#include <map>
#include <functional>
#include <memory>


class Boss {
public:

	enum AttackMode {
		kHigh,
		kMiddle,
		kUnder,
	};

	enum Behavior {
		kRoot,
		kAttack,
		kDead,
		kDown,
	};

	Behavior behavior_ = Behavior::kRoot;
	std::optional<Behavior> behaviorRequest_ = Behavior::kRoot;

	std::map<Behavior, std::function<void()>> behaviorInitTable_{
		{Behavior::kRoot,[this]() {RootInit(); }},
		{Behavior::kAttack,[this]() {AttackInit(); }},
		{Behavior::kDead,[this]() {DeadInit(); }},
		{Behavior::kDown,[this]() {DownInit(); }},
	};

	std::map<Behavior, std::function<void()>> behaviorUpdateTable_{
		{Behavior::kRoot,[this]() {RootUpdate(); }},
		{Behavior::kAttack,[this]() {AttackUpdate(); }},
		{Behavior::kDead,[this]() {DeadUpdate(); }},
		{Behavior::kDown,[this]() {DownUpdate(); }},
	};

private:

	void RootInit();
	void RootUpdate();

	void AttackInit();
	void AttackUpdate();

	void DeadInit();
	void DeadUpdate();

	void DownInit();
	void DownUpdate();

public:

	void Init(const std::shared_ptr<Model>& model);

	void Update();

	void Draw(const Camera& camera);

	const WorldTransform& GetWorldTransform() { return obj_->worldTransform_; }

	void DebugGUI();

private:

	std::unique_ptr<Object3d> obj_;

	Shapes::AABB collider_;

	AttackMode attackMode_ = AttackMode::kHigh;

	int32_t maxHp_ = 100;
	int32_t hp_ = maxHp_;

	bool isDead_ = false;

	const WorldTransform* target_;

	Vector3 direction_{};
	Matrix4x4 rotateMat_ = MakeIdentity44();

};

