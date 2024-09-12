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


class Stamp {
public:

	enum class Phase {
		kRoot,
		kCharge,
		kAttack,
	};

	Phase phase_ = Phase::kRoot;
	std::optional<Phase> phaseRequest_ = Phase::kRoot;

	std::map<Phase, std::function<void()>> phaseInitTable_{
		{Phase::kRoot,[this]() {RootInit(); }},
		{Phase::kCharge,[this]() {ChargeInit(); }},
		{Phase::kAttack,[this]() {AttackInit(); }},
	};

	std::map<Phase, std::function<void()>> phaseUpdateTable_{
		{Phase::kRoot,[this]() {RootUpdate(); }},
		{Phase::kCharge,[this]() {ChargeUpdate(); }},
		{Phase::kAttack,[this]() { AttackUpdate(); }},
	};

	struct ChargeData {
		float param_ = 0.0f;
		Vector3 minScale_ = {};
		Vector3 maxScale_ = { 1.0f,1.0f,1.0f };
	};

	struct AttackData {
		float param_ = 0.0f;
		Vector3 startPos_{};
		Vector3 impactPoint_{};
		int32_t count_ = 0;
		int32_t coolTime_ = 20;
	};

	ChargeData chargeData_;
	AttackData attackData_;


private:

	void RootInit();
	void RootUpdate();

	void ChargeInit();
	void ChargeUpdate();

	void AttackInit();
	void AttackUpdate();

public:

	void Init(const std::shared_ptr<Model>& model);

	void Update();
	void UpdateCollider();

	void Draw(const Camera& camera);

	void DrawParticle(const Camera& camera);

	void Hit();

	void AttackStart(const Vector3& startPos,const Vector3& direction);

	bool IsLife() const { return isLife_; }
	Shapes::OBB GetCollider() { return collider_; }

private:

	std::unique_ptr<Object3d> obj_;
	std::unique_ptr<Object3d> warningZone_;
	Vector3 direction_{};

	Matrix4x4 rotateMat_ = MakeIdentity44();

	Shapes::OBB collider_;

	bool isLife_ = false;
	bool preIsLife_ = false;


};
