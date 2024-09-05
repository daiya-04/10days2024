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

#include "Boss.h"
#include "MeteorManager.h"


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

};

