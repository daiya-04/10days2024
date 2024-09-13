#pragma once
#include "IScene.h"
#include <memory>
#include <list>
#include <vector>
#include <optional>

#include "Sprite.h"
#include "Object3d.h"
#include "SkinningObject.h"
#include "Particle.h"
#include "GPUParticle.h"
#include "Camera.h"
#include "WorldTransform.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "LevelLoader.h"
#include "PostEffect.h"
#include "OutLine.h"
#include "HSVFilter.h"
#include "SkyBox.h"
#include "Character/Player/Player.h"
#include"controlUI.h"
#include "AudioManager.h"

#include "Boss.h"
#include "MeteorManager.h"
#include "CannonManager.h"
#include "StampManager.h"
#include "Stage/Stage.h"
#include "Camera/DebugCamera.h"
#include "Camera/FollowCamera.h"
#include "CollisionManager.h"

class GameScene : public IScene {
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

	~GameScene()override;

	GameScene();

private:
  
	Camera camera_;

	PointLight pointLight_;
	SpotLight spotLight_;

private:

	std::unique_ptr<Boss> boss_;

	MeteorManager* meteor_ = nullptr;
	CannonManager* cannon_ = nullptr;
	StampManager* stamp_ = nullptr;

	LevelData* levelData_;
	std::unique_ptr<Stage> stage_;
	std::unique_ptr<DebugCamera> debugCamera_;
	std::unique_ptr<FollowCamera> followCamera_;

	std::unique_ptr<SkyBox> skyBox_;

private:
	//モデルマネージャー
	ModelManager* modelManager_;

	//コリジョンマネージャー
	std::unique_ptr<CollisionManager> collisionManager_;

	//プレイヤー
	std::unique_ptr<Player> player_;

	bool oldFallAttack_ = false;

	TextureManager* texManager_;

	std::array<Sprite*, 6> playerLifeTex_;

	float lifeScale_ = 0.7f;

	Vector2 lifeTexBasePos_ = { 530,640 };

	float lifeTexLength_ = 43.0f;

private:

	Audio* bgm_ = nullptr;

};

