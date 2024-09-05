#include "Player.h"

void Player::Initialize(){
	modelManager_ = ModelManager::GetInstance();

	bodyObj_ = std::make_unique<Object3d>();

	bodyObj_->Initialize(modelManager_->LoadOBJ("box"));

	input_ = Input::GetInstance();

	PLTransform_.Init();

	PLTransform_.scale_ = { 1.0f,1.0f,1.0f };

	postureVec_ = { 0.0f,0.0f,1.0f };
	frontVec_ = { 0.0f,0.0f,1.0f };

}

void Player::Update(){
	if (behaviorRequest_) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
			BehaviorRootInitialize();
			break;
		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		case Behavior::kDash:
			BehaviorDashInitialize();
			break;
		case Behavior::kDive:
			BehaviorDiveInitialize();
			break;
		}

	}
	// 振る舞いリクエストをリセット
	behaviorRequest_ = std::nullopt;

	switch (behavior_) {
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	case Behavior::kDash:
		BehaviorDashUpdate();
		break;
	case Behavior::kDive:
		BehaviorDiveUpdate();
		break;

	}

	bodyObj_->worldTransform_ = PLTransform_;
	bodyObj_->worldTransform_.UpdateMatrixRotate(playerRotateMat_);

}

void Player::Draw(const Camera& camera){
	
	bodyObj_->Draw(camera);
}

void Player::Imgui(){
	auto inputVector = input_->GetMoveXZ();

	ImGui::Begin("プレイヤー");
	
	ImGui::DragFloat3("move", &move_.x, 0.1f);
	ImGui::DragFloat3("input", &inputVector.x, 0.1f);

	ImGui::DragFloat3("postire", &postureVec_.x, 0.1f);
	ImGui::DragFloat3("front", &frontVec_.x, 0.1f);
	ImGui::End();
}

void Player::BehaviorRootInitialize(){
	move_ = { 0.0f,0.0f,0.0f };
}

void Player::BehaviorRootUpdate(){
	//前フレームの向きベクトルを記録
	frontVec_ = postureVec_;

	//入力を受け取って移動
	if (input_->GetMoveXZ().z != 0) {
		move_.z = (input_->GetMoveXZ().z / 32767.0f) * moveSpeed_;
	}
	else {
		move_.z = 0.0f;
	}

	//入力を受け取って移動
	if (input_->GetMoveXZ().x != 0) {
		move_.x = (input_->GetMoveXZ().x / 32767.0f) * moveSpeed_;
	}
	else {
		move_.x = 0.0f;
	}


	//カメラの向きによって移動ベクトルに補正を掛ける
	/*Matrix4x4 newRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(viewProjection_->rotation_);
	move_ = Matrix::GetInstance()->TransformNormal(move_, newRotateMatrix);
	move_.y = 0.0f;
	move_ = Vector3::Mutiply(Vector3::Normalize(move_), moveSpeed_ * 3.0f);
	move_.y = 0.0f;*/
	move_.Normalize();
	move_ *= (moveSpeed_ * 3.0f);
	move_.y = 0.0f; 

	if (move_.x != 0.0f or move_.z != 0.0f){
		postureVec_ = move_;

		Matrix4x4 directionTodirection;
		directionTodirection = DirectionToDirection(frontVec_, postureVec_);

		playerRotateMat_ = playerRotateMat_ * directionTodirection;

	}
	
	PLTransform_.translation_ += move_;

}

void Player::BehaviorAttackInitialize(){

}

void Player::BehaviorAttackUpdate(){

}

void Player::BehaviorDashInitialize(){

}

void Player::BehaviorDashUpdate(){

}

void Player::BehaviorDiveInitialize(){

}

void Player::BehaviorDiveUpdate(){

}

void Player::Respawn(){

}

void Player::Gravity(){

}
