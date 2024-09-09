#pragma once
#include "GroundPiece.h"
#include "LevelLoader.h"

class Ground {
public:
	Ground() = default;
	~Ground() = default;

	void SetGlobalVariables();
	void ApplyGlobalVariables();

	void Initialize(const LevelData* data, const std::vector<std::shared_ptr<Model>>& models, WorldTransform transform, const std::shared_ptr<Model>& boardModel);

	void Update();

	void Draw(const Camera& camera);

	void IsCollision(const float& angle); // ボスの攻撃座標


private:
	std::vector<std::unique_ptr<GroundPiece>> pieces_;
	WorldTransform transform_; // 地面の親world座標

	float endurancePower_ = 100.0f; // 耐久力
	float groundScale_ = 1.0f; // マスターscale
	std::string groupName_ = "Ground";

	std::unique_ptr<Object3d> electricBoard_;

};