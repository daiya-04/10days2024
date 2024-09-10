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

class Player {
public:
	//調整項目の代入
	void ApplyGlobalVariables();

	//調整項目の追加
	void SetGlobalVariables();
	//初期化処理
	void Initialize();
	//更新処理
	void Update(const Vector3& centerTarget, const Vector2& minAndMax);
	//描画処理
	void Draw(const Camera& camera);

	//描画処理
	void ParticleDraw(const Camera& camera);
	//デバック用GUIの表示
	void Imgui();

public:
	const WorldTransform& GetWorldTrnas() const { return PLTransform_; }

	void SetCameraRotate(const Vector3* rotate) { cameraRotate_ = rotate; }


private:
	/*振る舞い系*/
	enum class Behavior {
		kRoot,			//通常状態
		kAttack,		//攻撃
		kAvoid,			//加速しながら回避
		kDash,			//ダッシュ中
		kFallingAttack,	//落下攻撃
		kChargeAttack	//溜め攻撃
	};
	//現在の状態
	Behavior behavior_ = Behavior::kRoot;
	//次の状態の予約
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

private:
	//クラス内行動関数

	//通常行動初期化
	void BehaviorRootInitialize();
	// 通常行動更新
	void BehaviorRootUpdate();
	//攻撃行動初期化
	void BehaviorAttackInitialize();
	//攻撃行動更新
	void BehaviorAttackUpdate();
	//回避行動初期化
	void BehaviorAvoidInitialize();
	//回避行動更新
	void BehaviorAvoidUpdate();
	//ダッシュ行動初期化
	void BehaviorDashInitialize();
	//ダッシュ行動更新
	void BehaviorDashUpdate();
	//ダッシュ行動初期化
	void BehaviorFallingAttackInitialize();
	//ダッシュ行動更新
	void BehaviorFallingAttackUpdate();

private:
	/*攻撃処理の初期化、更新*/
	
	//攻撃行動初期化
	void BehaviorSecondAttackInitialize();
	//攻撃行動
	void BehaviorThirdAttackInitialize();

	//攻撃のモーション
	void AttackMotion();

	void secondAttackMotion();

	void thirdAttackMotion();

	//溜め攻撃初期化
	void BehaviorChargeAttackInitialize();
	//溜め攻撃更新
	void BehaviorChargeAttackUpdate();

private:
	//落下復帰処理
	void Respawn();
	//丸影の処理
	void Shadow();
	
	//重力関係の処理
	void Gravity();
	//床に当たったときの反応処理
	void OnFloorCollision();

public: // とりあえずGameSceneで角
	//Stageとの衝突判定Clamp 中心点(原点)
	bool StageClampCollision(const Vector3& worldTrans);

private:
	/*ベースとなるモデルやトランスフォームなど*/
	//モデルマネージャー
	ModelManager* modelManager_;
	//体のモデル
	std::unique_ptr<Object3d> bodyObj_;

	//右手のモデル
	std::unique_ptr<Object3d> rightHandObj_;

	//左手のモデル
	std::unique_ptr<Object3d> leftHandObj_;

	//左手のモデル
	std::unique_ptr<Object3d> shadowObj_;
	//キー入力
	Input* input_;

	std::unique_ptr<Particle> diveParticle_;
	//プレイヤーの基本となる回転軸
	const Vector3 baseAxis_ = { 0.0f,1.0f,0.0f };

	const char* groupName_ = "Player";

	//カメラの回転
	const Vector3* cameraRotate_ = nullptr;

	//ステージ内外でのあたり用変数
	Vector2 minAndMax_;

	Vector3 centerPos_;
private:
	/*重力関係*/

	//重力の強さ
	float gravityPower_ = -0.04f;
	//落下のベクトル
	Vector3 downVector_ = {};
	//ジャンプの強さ
	float jumpPower_ = 0.9f;

	//落下するかどうか
	bool isDown_ = false;
	//ジャンプ攻撃時の重力
	float gravityPowerAttack_ = -0.08f;
	//ジャンプ攻撃時のジャンプの強さ
	float jumpPowerAttack_ = 0.9f;

	float groundLength_ = 1.7f;

private:
	/*攻撃関連変数*/
	struct  WorkAttack {
		//
		uint32_t attackParameter_ = 0;
		//現在のコンボ段数
		int32_t comboIndex_ = 0;
		//次の段の攻撃を有効化するかどうか
		bool comboNext_ = false;
		//チャージ攻撃を有効化するかどうか
		bool chargeAttackNext_ = false;
		//チャージ攻撃に派生するための時間
		int32_t chargeFlugTime_ = 0;
	};

	WorkAttack workAttack_;
	//現時点でのコンボが終了したかどうか
	bool endCombo_ = false;

	bool isEndAttack_ = false;
	//攻撃後の待機時間
	int waitTimeBase_ = 10;
	int waitTimeBaseCharge_ = 20;
	int waitTime_ = 0;
	//大本なる攻撃の速度
	float baseAttackSpeed_ = 0.08f;
	//モーションとモーションの隙間時間
	int32_t motionDistance_ = 25;
	//イージングに使用する変数
	float easeT_ = 0.0f;
	//モーションの速度倍率
	float motionSpeed_ = 2.0f;
	//最大コンボ数
	const int conboNum_ = 3;

	//チャージ用の倍率変数
	int32_t chargeRotateSpeed_ = 1;
	//元となる回転速度
	float beseRotateSpeed_ = 0.1f;
	//チャージ用の回転変数
	float chargeRotate_ = 0.0f;
	//チャージ攻撃している時間
	int32_t chargeTime_ = 0;
	//チャージ攻撃中か
	bool isCharge_ = false;

private:
	/*行動関連の変数*/
	//Behaviorで操作する本体のトランスフォーム
	WorldTransform PLTransform_;

	//Behaviorで操作する右手のトランスフォーム
	WorldTransform RHandTransform_;

	//Behaviorで操作する左手のトランスフォーム
	WorldTransform LHandTransform_;

	//溜め攻撃をするときの右手のトランスフォーム
	WorldTransform RRotateHandTransform_;

	//影用のトランスフォーム
	WorldTransform ShadowTransform_;

	float shadowY_ = -1.7f;

	//移動ベクトル
	Vector3 move_;
	//代入する回転行列Y軸
	Matrix4x4 playerRotateMatY_;

	//代入する回転行列X軸
	Matrix4x4 playerRotateMatX_;

	//右腕の回転行列
	Matrix4x4 rightHandRotateMat_;
	//前フレームの向きベクトル
	Vector3 frontVec_;
	//現在の向きベクトル
	Vector3 postureVec_;
	//移動速度
	float moveSpeed_ = 0.6f;
	//ダッシュ時の移動速度
	float dashSpeed_ = 0.8f;
	//回避時の移動速度
	float avoidSpeed_ = 1.0f;
	//回避の継続時間
	uint32_t avoidTime_ = 0;

};


