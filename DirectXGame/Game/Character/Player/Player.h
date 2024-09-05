#pragma once
#include"Object3d.h"
#include"Model/ModelManager.h"
#include"imgui.h"

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

	ModelManager* modelManager_;

	std::unique_ptr<Object3d> bodyObj_;

	WorldTransform transform_;



};

