#pragma once
#include"Camera.h"
#include"WorldTransform.h"
#include"GlobalVariables.h"

class FollowCamera{
public:
	FollowCamera();
	~FollowCamera() = default;

	

	void Update();
	const Vector3& GetCameraTranslate() const {
		return translate_;
	}
	const Vector3& GetCameraRotate() const {
		return rotation_;
	}

	

	void SetTarget(const WorldTransform* target);
	//ゲームシーンかどうか
	void SetScene(const bool scene) { isGame_ = scene; }

private:
	//調整項目の代入
	void ApplyGlobalVariables();

	//調整項目の追加
	void SetGlobalVariables();

	Vector3 offsetCalculation(const Vector3& offset) const {
		Vector3 offset_ = offset;
		Matrix4x4 newRotateMatrix = MakeRotateYMatrix(rotation_.y);

		offset_ = TransformNormal(offset_, newRotateMatrix);

		return offset_;
	}

private:
	const char* groupName_ = "FollowCamera";

private:
	const WorldTransform* target_ = nullptr;

	//姿勢ベクトル
	const Vector3 Vec_ = { 0.0f,0.0f,1.0f };
	Vector3 postureVec_{};
	Vector3 frontVec_{};

	float distance_ = -10.0f;
	float height_ = 1.5f;

	float cameraRotateX_ = 0.15f;

	//基準のオフセット
	Vector3 baseOffset_;

	Vector3 rootOffset_;

	Vector3 translate_;
	Vector3 rotation_;
	bool isGame_ =true;
};

