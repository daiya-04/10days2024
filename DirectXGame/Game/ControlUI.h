#pragma once
#include"ImGuiManager.h"
#include"Engine/Base/TextureManager.h"
#include"Sprite.h"

class ControlUI{
public:

	void Initialize();

	void Update();

	void Draw();

	void DrawGUI();

private:
	TextureManager* texManager_;

	Sprite* Atex_;
	Sprite* Xtex_;
	Sprite* RBtex_;
	Sprite* jumptex_;
	Sprite* attacktex_;
	Sprite* dashtex_;
	
	float scale_ = 10.0f;

};

