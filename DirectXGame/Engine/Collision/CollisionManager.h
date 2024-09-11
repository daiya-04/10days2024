#pragma once
#include"Hit.h"
#include"Player.h"
#include"Boss.h"

class CollisionManager{
public: // メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Player* player, Boss* boss);

	/// <summary>
	/// 衝突
	/// </summary>
	void AllCollision();

private: // メンバ関数

private: // メンバ変数

	// ゲームオブジェクト
	Player* player_ = nullptr;
	Boss* boss_ = nullptr;
};

