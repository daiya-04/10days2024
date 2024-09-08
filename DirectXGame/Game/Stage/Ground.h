#pragma once
#include "GroundPiece.h"
#include "LevelLoader.h"

class Ground {
public:
	Ground() = default;
	~Ground() = default;

	void Initialize(const LevelData* data, const std::vector<std::shared_ptr<Model>>& models, WorldTransform transform);

	void Update();

	void Draw(const Camera& camera);

private:
	std::vector<std::unique_ptr<GroundPiece>> pieces_;
	WorldTransform transform_; // 地面の親world座標


};