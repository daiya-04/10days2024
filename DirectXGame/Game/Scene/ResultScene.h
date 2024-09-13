#pragma once
#include "IScene.h"
#include <memory>
#include <list>

#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"
#include"Character/Player/Player.h"
#include "Camera.h"
#include "WorldTransform.h"
#include"Camera/DebugCamera.h"
#include"Camera/FollowCamera.h"
#include"CollisionManager.h"
#include "SkyBox.h"

#include"Character/Sandbag/Sandbag.h"

class ResultScene : public IScene {
public:
	void Init()override;

	void Update()override;

	void DrawBackGround()override;

	void DrawModel()override;

	void DrawParticleModel()override;

	void DrawParticle()override;

	void DrawUI()override;

	void DrawPostEffect()override;

	void DrawRenderTexture()override;

	void DebugGUI()override;

	~ResultScene()override;

private:
	/*敗北と勝利で呼ぶ関数を変化させる*/
	//敗北時の初期化
	void LoseInitialize();
	//敗北時の更新
	void LoseUpdate();
	//敗北時のモデル描画
	void LoseDrawModel();
	//敗北時のUI描画
	void LoseDrawUI();

	//勝利時の初期化
	void WinInitialize();
	//勝利時の更新
	void WinUpdate();
	//勝利時のモデル描画
	void WinDrawModel();
	//勝利時のUI描画
	void WinDrawUI();

public:

	static bool isLose_;
private:
	Camera camera_;

	PointLight pointLight_;
	SpotLight spotLight_;
private:


private:
	//モデルマネージャー
	ModelManager* modelManager_;

	TextureManager* texManager_;


	//鏡餅
	std::unique_ptr<Sandbag> sandbag_;
	//鏡餅置くよう用
	std::unique_ptr<Object3d> shelfobj_;

	std::unique_ptr<SkyBox> skyBox_;

	std::unique_ptr<Sprite> fade_;
	float alpha_ = 0.6f;
	std::unique_ptr<Sprite> loseTex_;
};

