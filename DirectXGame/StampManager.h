#pragma once
#include "Stamp.h"
#include "Camera.h"
#include "ModelManager.h"
#include "GPUParticle.h"

#include <array>


class StampManager {
public:

	static StampManager* GetInstance();

	void Init(const std::shared_ptr<Model>& model);

	void Update();

	void Draw(const Camera& camera);

	void DrawParticle(const Camera& camera);

	void Hit(uint32_t index) { stamps_[index]->Hit(); }

	void AttackStart(const Vector3& basePos);
	void AttackFinish();

	Shapes::OBB GetCollider(uint32_t index) { return stamps_[index]->GetCollider(); }
	bool IsLife(uint32_t index) { return stamps_[index]->IsLife(); }
	bool GetIsHit(uint32_t index) const { return stamps_[index]->HitFlag(); } // 当たった瞬間
	uint32_t GetDamage(uint32_t index) const { return stamps_[index]->GetDamage(); }


private:

	std::array<std::unique_ptr<Stamp>, 16> stamps_;

	bool isAttack_ = false;
	bool preIsAttack_ = false;

	Vector3 basePos_{};
	float startHeight_ = 10.0f;

	std::array<Vector3, 16> offsets_;
	int32_t stampIndex_ = 0;

	int32_t count_ = 0;
	int32_t attackTime_ = 60;

	int32_t indexCount_ = 0;

};

