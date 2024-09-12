#pragma once
#include "Cannon.h"
#include "Camera.h"
#include "ModelManager.h"
#include "GPUParticle.h"

#include <array>


class CannonManager {
public:

	static CannonManager* GetInstance();

	void Init(const std::shared_ptr<Model>& model);

	void Update();

	void Draw(const Camera& camera);

	void DrawParticle(const Camera& camera);

	void AttackStart(const Vector3& basePos);
	void AttackFinish();

	void Hit(uint32_t index) { cannons_[index]->Hit(); }
	void Reflection(uint32_t index) { cannons_[index]->Reflection(); }

	void Reset();

	void SetTargetPos(const Vector3& targetPos);

	bool IsAttack() const { return isAttack_; }
	bool IsBossHit(uint32_t index) const { return cannons_[index]->IsBossHit(); }
	bool IsLife(uint32_t index) const { return cannons_[index]->IsLife(); }

	Shapes::Sphere GetCollider(uint32_t index) { return cannons_[index]->GetCollider(); }

private:

	std::array<std::unique_ptr<Cannon>, 16> cannons_;
	std::unique_ptr<GPUParticle> hitEff_;

	bool isAttack_ = false;
	bool preIsAttack_ = false;

	std::array<Vector3, 16> offsets_;
	Vector3 basePos_ = {};

	int32_t cannonIndex_ = 0;

	int32_t count_ = 0;
	int32_t attackTime_ = 60;

	int32_t indexCount_ = 0;

};

