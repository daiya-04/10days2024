#include "FollowCamera.h"
void FollowCamera::SetGlobalVariables(){
	GlobalVariables* global = GlobalVariables::GetInstance();
	//グループの追加
	global->CreateGroup(groupName_);
	//項目の追加
	global->AddItem(groupName_, "height", height_);
	global->AddItem(groupName_, "distance", distance_);
	global->AddItem(groupName_, "rotateX", cameraRotateX_);

}

void FollowCamera::ApplyGlobalVariables(){
	GlobalVariables* global = GlobalVariables::GetInstance();
	//項目の追加
	height_ = global->GetFloatValue(groupName_, "height");
	distance_ = global->GetFloatValue(groupName_, "distance");
	cameraRotateX_ = global->GetFloatValue(groupName_, "rotateX");

}


FollowCamera::FollowCamera(){
	rootOffset_ = { 0.0f, height_, distance_ };

	baseOffset_ = rootOffset_;

	postureVec_ = { 0.0f,0.0f,1.0f };
	frontVec_ = { 0.0f,0.0f,1.0f };

	SetGlobalVariables();
}

void FollowCamera::Update(){
	frontVec_ = postureVec_;

	ApplyGlobalVariables();

	if (isGame_) {

		Vector3 lockOnPos = { 0.0f,0.0f,0.0f };
		Vector3 sub = lockOnPos - target_->translation_;
		sub.y = 0;
		postureVec_ = sub;

		Matrix4x4 newMatrix;
		newMatrix = DirectionToDirection((Vector3(0.0f, 0.0f, 1.0f)), (postureVec_.Normalize()));
		rotation_.y = RotateAngleYFromMatrix(newMatrix);
		rotation_.x = cameraRotateX_;
	}
	
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

