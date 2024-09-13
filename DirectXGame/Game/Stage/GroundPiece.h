#pragma once
#include "WorldTransform.h"
#include "Camera.h"

#include <memory>
#include <vector>
#include "ModelManager.h"
#include "Object3d.h"
#include "LevelLoader.h"
#include "AudioManager.h"

class GroundPiece {
public:
	GroundPiece() = default;
	GroundPiece(const std::vector<std::shared_ptr<Model>>& models);
	~GroundPiece() = default;

	void Initialize(const LevelData::ObjectData& data, const std::string& tag, const WorldTransform* parent = nullptr);
	void Initialize();

	void Update();

	void Draw(const Camera& camera);

	void OnCollision(const int32_t& damage); // ダメージを追った処理

	const std::string& GetTag() const { return tag_; }

	Vector3 GetRotation() const { return transform_.rotation_; }
	bool GetIsAlive() const { return isAlive_; }
	void SetTextureHandle(const int32_t& handle) { handle_ = handle; }


private:
	void UpdateTrans(); // 座標更新用関数

private:
	std::string tag_;
	std::vector<std::shared_ptr<Object3d>> models_;
	WorldTransform transform_;
	bool isAlive_ = true; // 生存フラグ
	int32_t hp_ = 50; // 耐久力

	Audio* breakSE_ = nullptr;
	uint32_t handle_ = 0u;
public:
	bool IsDamaged_ = false; // hpが半分以下ならtrueになる

};
