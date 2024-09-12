#pragma once
#include "IScene.h"
#include <memory>
#include <list>
#include "Stage/Stage.h"

#include "Sprite.h"
#include "Object3d.h"
#include "Particle.h"
#include"Character/Player/Player.h"
#include "Camera.h"
#include "WorldTransform.h"
#include"Camera/DebugCamera.h"
#include"Camera/FollowCamera.h"
#include"CollisionManager.h"
#include"ControlUI.h"
#include"Character/Sandbag/Sandbag.h"


class TitleScene : public IScene {
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

	~TitleScene()override;

private:
	Camera camera_;

	PointLight pointLight_;
	SpotLight spotLight_;
private:

	std::unique_ptr<DebugCamera> debugCamera_;
	std::unique_ptr<FollowCamera> followCamera_;


private:
	//モデルマネージャー
	ModelManager* modelManager_;

	LevelData* levelData_;

	std::unique_ptr<Stage> stage_;

	//コリジョンマネージャー
	std::unique_ptr<CollisionManager> collisionManager_;
	//プレイヤー
	std::unique_ptr<Player> player_;

	//鏡餅
	std::unique_ptr<Sandbag> sandbag_;
	//鏡餅置くよう用
	std::unique_ptr<Object3d> shelfobj_;

	//鏡餅置くよう用
	std::unique_ptr<Object3d> titleobj_;

	bool oldFallAttack_ = false;

	int32_t killCount_ = 0;

	std::unique_ptr<ControlUI> controlUI_;
};

