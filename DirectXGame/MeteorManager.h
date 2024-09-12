#pragma once
#include "Meteor.h"
#include "Camera.h"
#include "ModelManager.h"
#include "GPUParticle.h"

#include <array>
#include <list>

class MeteorManager {
public:

	static MeteorManager* GetInstance();

	void Init(const std::shared_ptr<Model>& model);

	void Update();

	void Draw(const Camera& camera);

	void DrawParticle(const Camera& camera);

	void Hit(uint32_t index);
	void Reflection(uint32_t index) { return meteors_[index]->Reflection(); }

	void Reset();

	void AttackStart(const Vector3& basePos);
	void AttackFinish();

	void SetTargetPos(const Vector3& targetPos);
	
	void SetPieceAlives(const std::array<bool,16> pieceAlives) { pieceAlives_ = pieceAlives; }

	bool IsAttack()const { return isAttack_; }

	Shapes::Sphere GetCollider(uint32_t index) { return meteors_[index]->GetCollider(); }
	bool IsLife(uint32_t index) { return meteors_[index]->IsLife(); }
	bool GetIsHit(uint32_t index) const { return meteors_[index]->HitFlag(); } // 当たった瞬間
	const uint32_t& GetDamage(uint32_t index) const { return meteors_[index]->GetDamage(); }

private:

	std::array<std::unique_ptr<Meteor>, 16> meteors_;
	std::unique_ptr<GPUParticle> hitEff_;

	bool isAttack_ = false;
	bool preIsAttack_ = false;

	Vector3 basePos_{};
	float startHight_ = 15.0f;

	std::array<Vector3, 16> offsets_;

	int32_t count_ = 0;
	int32_t attackTime_ = 40;

	int32_t indexCount_ = 0;

	std::array<bool, 16> pieceAlives_;

	std::list<int32_t> indexList_;
	std::list<int32_t>::iterator indexIt_;

};

