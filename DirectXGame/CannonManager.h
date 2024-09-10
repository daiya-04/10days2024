#pragma once
#include "Cannon.h"
#include "Camera.h"
#include "ModelManager.h"

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

private:

	std::array<std::unique_ptr<Cannon>, 8> cannons_;

	bool isAttack_ = false;
	bool preIsAttack_ = false;

	std::array<Vector3, 8> offsets_;
	Vector3 basePos_ = {};

	int32_t cannonIndex_ = 0;

	int32_t count_ = 0;
	int32_t attackTime_ = 30;

};

