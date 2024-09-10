#pragma once
#include "WorldTransform.h"
#include "Camera.h"

#include <memory>
#include <vector>
#include "ModelManager.h"
#include "Object3d.h"
#include "LevelLoader.h"

class GroundPiece {
public:
	GroundPiece() = default;
	GroundPiece(const std::vector<std::shared_ptr<Model>>& models);
	~GroundPiece() = default;

	void Initialize(const LevelData::ObjectData& data, const std::string& tag, const WorldTransform* parent = nullptr);
	void Initialize();

	void Update();

	void Draw(const Camera& camera);

	void OnCollision(const float& damage); // ダメージを追った処理

	const std::string& GetTag() const { return tag_; }

	Vector3 GetRotation() const { return transform_.rotation_; }
	bool GetIsAlive() const { return isAlive_; }

private:
	void UpdateTrans(); // 座標更新用関数

private:
	std::string tag_;
	std::vector<std::shared_ptr<Object3d>> models_;
	WorldTransform transform_;
	bool isAlive_ = true; // 生存フラグ
	float hp_ = 100.0f; // 耐久力

};
