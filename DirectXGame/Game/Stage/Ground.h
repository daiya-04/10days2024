#pragma once
#include "GroundPiece.h"
#include "LevelLoader.h"

class Ground {
public:
	Ground() = default;
	~Ground() = default;

	void SetGlobalVariables();
	void ApplyGlobalVariables();

	void Initialize(const LevelData* data, const std::vector<std::shared_ptr<Model>>& models, WorldTransform transform, const std::shared_ptr<Model>& boardModel, const std::string& layer);
	void Initialize();

	void Update();

	void Draw(const Camera& camera);

	bool IsCollision(const float& angle, const float& damage = 0.0f);

	const std::string& GetLayer() const { return layer_; }
	WorldTransform GetTransform() const { return transform_; }

	WorldTransform transform_; // 地面の親world座標
private:
	std::vector<std::unique_ptr<GroundPiece>> pieces_;

	float endurancePower_ = 100.0f; // 耐久力
	float groundScale_ = 1.0f; // マスターscale
	std::string groupName_ = "Ground";
	std::string layer_;

	std::unique_ptr<Object3d> electricBoard_;

};