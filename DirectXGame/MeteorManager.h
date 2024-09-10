#pragma once
#include "Meteor.h"
#include "Camera.h"
#include "ModelManager.h"
#include "GPUParticle.h"

#include <array>

class MeteorManager {
public:

	static MeteorManager* GetInstance();

	void Init(const std::shared_ptr<Model>& model);

	void Update();

	void Draw(const Camera& camera);

	void DrawParticle(const Camera& camera);

	void AttackStart(const Vector3& basePos);
	void AttackFinish();

	Shapes::Sphere GetCollider(uint32_t index) { return meteors_[index]->GetCollider(); }

private:

	std::array<std::unique_ptr<Meteor>, 16> meteors_;
	std::unique_ptr<GPUParticle> hitEff_;

	bool isAttack_ = false;
	bool preIsAttack_ = false;

	Vector3 basePos_{};
	float startHight_ = 10.0f;

	std::array<Vector3, 16> offsets_;

	int32_t meteorIndex_ = 0;

	int32_t count_ = 0;
	int32_t attackTime_ = 30;




};

