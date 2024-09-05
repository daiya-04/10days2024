#pragma once
#include "Meteor.h"
#include "Camera.h"
#include "ModelManager.h"




class MeteorManager {
public:

	static MeteorManager* GetInstance();

	void Init(const std::shared_ptr<Model>& model);

	void Update();

	void Draw(const Camera& camera);

	void DrawParticle(const Camera& camera);

	void AttackStart(const Vector3& basePos);
	void AttackFinish();

private:

	std::array<std::unique_ptr<Meteor>, 8> meteors_;

	bool isAttack_ = false;
	bool preIsAttack_ = false;

	Vector3 basePos_{};
	float startHight_ = 8.0f;

	std::array<Vector3, 8> offset_;

	int32_t meteorIndex_ = 0;

	int32_t count_ = 0;
	int32_t attackTime_ = 30;




};

