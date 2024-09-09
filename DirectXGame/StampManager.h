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

	void AttackStart(const Vector3& basePos);
	void AttackFinish();

private:

	std::array<std::unique_ptr<Stamp>, 8> stamps_;

	bool isAttack_ = false;
	bool preIsAttack_ = false;

	Vector3 basePos_{};
	float startHeight_ = 10.0f;

	std::array<Vector3, 8> offsets_;
	int32_t stampIndex_ = 0;

	int32_t count_ = 0;
	int32_t attackTime_ = 30;

};

