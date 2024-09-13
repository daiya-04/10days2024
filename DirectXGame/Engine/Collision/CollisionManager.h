#pragma once
#include"Hit.h"
#include"Character/Player/Player.h"
#include"Boss.h"
#include"Character/Sandbag/Sandbag.h"

class CollisionManager{
public: // メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Player* player, Boss* boss);

	/// <summary>
	/// 初期化
	/// </summary>
	void TitleInitialize(Player* player, Sandbag* sandbag);

	/// <summary>
	/// 衝突
	/// </summary>
	void AllCollision();

private: // メンバ関数

private: // メンバ変数

	// ゲームオブジェクト
	Player* player_ = nullptr;
	Boss* boss_ = nullptr;
	Sandbag* sandbag_ = nullptr;
};

