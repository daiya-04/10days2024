#pragma once
#include"Object3d.h"
#include"Model/ModelManager.h"
#include"Hit.h"
#include"imgui.h"
#include"Particle/Particle.h"
#include"AnimationManager.h"
#include"Input.h"
#include"Engine/Easing/Easing.h"
#include"Engine/DebugText/GlobalVariables.h"
#include"CollisionShapes.h"
#include"HitRecord/HitRecord.h"


class Sandbag{
public:
	//初期化
	void Initialize();

	//更新
	void Update();

	//描画
	void Draw(const Camera& camera);

	//敵の攻撃に当たったときの反応
	void OnCollision(const int32_t power);

	void SetPosition(const Vector3& position) { 
		bodyObj_->worldTransform_.translation_ = position;
		isDead_ = false;
		hp_ = 25;
	}

	const Sphere& GetCollider() const { return collider_; }

	const bool GetIsDead()const { return isDead_; }

private:
	/*ベースとなるモデルやトランスフォームなど*/
	//モデルマネージャー
	ModelManager* modelManager_;
	//体のモデル
	std::unique_ptr<Object3d> bodyObj_;

	//コライダー
	Sphere collider_;

	float xRadian_;

	//代入する回転行列Y軸
	Matrix4x4 playerRotateMatY_;

	//Behaviorで操作する本体のトランスフォーム
	WorldTransform PLTransform_;

	int32_t hp_ = 25;

	bool isDead_ = false;

};

