#pragma once
#include "WorldTransform.h"
#include "Camera.h"

#include <memory>
#include <vector>
#include "ModelManager.h"
#include "Object3d.h"
#include "LevelLoader.h"
#include "CollisionShapes.h"

class GroundPiece {
public:
	GroundPiece() = default;
	~GroundPiece() = default;

	void Initialize(const LevelData::ObjectData& data, const std::vector<std::shared_ptr<Model>>& models);

	void Update();

	void Draw(const Camera& camera);

private:
	void UpdateTrans(); // 座標更新用関数

private:

	std::vector<std::shared_ptr<Object3d>> models_;
	bool isAlive_ = true; // 生存フラグ

};
