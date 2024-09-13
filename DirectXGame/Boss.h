#pragma once
#include "WorldTransform.h"
#include "Object3d.h"
#include "ModelManager.h"
#include "Camera.h"
#include "Animation.h"
#include "CollisionShapes.h"
#include "Sprite.h"
#include"AudioManager.h"
#include <map>
#include <functional>
#include <memory>


class Boss {
public:

	enum class AttackMode {
		kHigh,
		kMiddle,
		kUnder,
	};

	enum class Behavior {
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
	void DrawUI();
	
	void HitPlayerAttackCollision(const int32_t power);

	const WorldTransform& GetWorldTransform() { return obj_->worldTransform_; }

	const Shapes::Sphere& GetCollider() { return collider_[attackMode_]; }

	bool GetIsDead() const { return isDead_; }

	void SetAttackMode(uint32_t layer_) {
		if (layer_ == 0) {
			attackMode_ = AttackMode::kHigh;
		}
		else if (layer_ == 1) {
			attackMode_ = AttackMode::kMiddle;
		}
		else if (layer_ == 2) {
			attackMode_ = AttackMode::kUnder;
		}
		if (attackMode_ != preAttackMode_) {
			behaviorRequest_ = Behavior::kRoot;
		}
	}

	Shapes::Sphere GetCollider(AttackMode attackMode) { return collider_[attackMode]; }

	void AttackHit();

	void DebugGUI();

	void ShakeInit();
	void Shakeing();

private:

	std::unique_ptr<Object3d> obj_;
	std::unique_ptr<Sprite> hpBer_;

	std::map<AttackMode, Shapes::Sphere> collider_;

	AttackMode attackMode_ = AttackMode::kHigh;
	AttackMode preAttackMode_ = AttackMode::kHigh;

	int32_t maxHp_ = 300;
	int32_t hp_ = maxHp_;

	bool isDead_ = false;

	const WorldTransform* target_;

	Vector3 direction_{};
	Matrix4x4 rotateMat_ = MakeIdentity44();

	int32_t count_ = 0;
	int32_t coolTime_ = 60;

	Vector3 shakePos_{};
	Vector3 basePos_{};
	int32_t shakeCount_ = 0;
	int32_t shakeTime_ = 10;

	Audio* reflectAttackHitSE_ = nullptr;

	bool isShake_ = false;

};

