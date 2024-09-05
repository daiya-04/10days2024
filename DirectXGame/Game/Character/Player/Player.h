#pragma once
#include"Object3d.h"
#include"Model/ModelManager.h"
#include"Hit.h"
#include"imgui.h"
#include"Input.h"

class Player{
public:
	//初期化処理
	void Initialize();
	//更新処理
	void Update();
	//描画処理
	void Draw(const Camera& camera);
	//デバック用GUIの表示
	void Imgui();

private:
	/*振る舞い系*/
	enum class Behavior {
		kRoot,			//通常状態
		kAttack,		//攻撃
		kDash,			//ダッシュ中
		kDive			//潜行状態
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
	//ダッシュ行動初期化
	void BehaviorDashInitialize();
	//ダッシュ行動更新
	void BehaviorDashUpdate();
	//潜行行動初期化
	void BehaviorDiveInitialize();
	//潜行行動更新
	void BehaviorDiveUpdate();

private:
	//落下復帰処理
	void Respawn();
	//重力関係の処理
	void Gravity();
	//床に当たったときの反応処理
	void OnFloorCollision();

private:
	/*ベースとなるモデルやトランスフォームなど*/
	//モデルマネージャー
	ModelManager* modelManager_;
	//体のモデル
	std::unique_ptr<Object3d> bodyObj_;
	//キー入力
	Input* input_;
	
private:
	/*重力関係*/

	//重力の強さ
	float gravityPower_ = -0.01f;
	//落下のベクトル
	Vector3 downVector_ = {};
	//ジャンプの強さ
	float jumpPower_ = 0.25f;

	//落下するかどうか
	bool isDown_ = false;

private:
	/*行動関連の変数*/
	//Behaviorで操作するトランスフォーム
	WorldTransform PLTransform_;

	//移動ベクトル
	Vector3 move_;
	//代入する回転行列
	Matrix4x4 playerRotateMat_;

	//前フレームの向きベクトル
	Vector3 frontVec_;
	//現在の向きベクトル
	Vector3 postureVec_;
	//移動速度
	float moveSpeed_ = 0.3f;

};

