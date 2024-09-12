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
	Camera camera_;

	PointLight pointLight_;
	SpotLight spotLight_;
private:


private:
	//モデルマネージャー
	ModelManager* modelManager_;

	//鏡餅
	std::unique_ptr<Sandbag> sandbag_;
	//鏡餅置くよう用
	std::unique_ptr<Object3d> shelfobj_;

};

