#include "FollowCamera.h"

FollowCamera::FollowCamera(){
	rootOffset_ = { 0.0f, height_, distance_ };

	baseOffset_ = rootOffset_;

	postureVec_ = { 0.0f,0.0f,1.0f };
	frontVec_ = { 0.0f,0.0f,1.0f };
}

void FollowCamera::Update(){
	frontVec_ = postureVec_;

	ApplyGlobalVariables();

	Vector3 lockOnPos = { 0.0f,0.0f,0.0f };
	Vector3 sub = lockOnPos - target_->translation_;
	sub.y = 0;
	postureVec_ = sub;

	Matrix4x4 newMatrix;
	newMatrix = DirectionToDirection((Vector3(0.0f,0.0f,1.0f)), (postureVec_.Normalize()));
	rotation_.y = RotateAngleYFromMatrix(newMatrix);
	rotation_.x = 0.15f;
	
	rootOffset_ = { 0.0f, height_, distance_ };
	baseOffset_ = rootOffset_;

	if (target_) {
		//追従対象からカメラまでのオフセット
		Vector3 offset = offsetCalculation(baseOffset_);

		//座標をコピーしてオフセット分ずらす
		translate_ = target_->translation_ + offset;

	}

}

void FollowCamera::SetTarget(const WorldTransform* target){
	target_ = target;
	Vector3 offset = offsetCalculation(baseOffset_);
	translate_ = target_->translation_ + offset;
}

void FollowCamera::ApplyGlobalVariables(){

}

void FollowCamera::SetGlobalVariables(){

}
