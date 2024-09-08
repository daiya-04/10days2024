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

	anim_ = AnimationManager::Load("LockOn");
	anim_.SetAnimationSpeed(1.0f / 90.0f);

	lockOnObj_ = std::make_unique<Object3d>();

	lockOnObj_->Initialize(modelManager_->LoadGLTF("LockOn"));

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
		case Behavior::kAvoid:
			BehaviorAvoidInitialize();
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
	case Behavior::kAvoid:
		BehaviorAvoidUpdate();
		break;

	}
	auto front = frontVec_.Normalize();
	auto posture = postureVec_.Normalize();

	if (front == -posture) {
		frontVec_ = postureVec_;
	}


	bodyObj_->worldTransform_ = PLTransform_;
	bodyObj_->worldTransform_.UpdateMatrixRotate(playerRotateMat_);
	if (isDive_) {
		anim_.Play(lockOnObj_->GetModel()->rootNode_, true);
		lockOnObj_->worldTransform_.translation_ = PLTransform_.translation_;
		lockOnObj_->worldTransform_.translation_.y = 2.0f;
		lockOnObj_->worldTransform_.UpdateMatrix();

		lockOnObj_->worldTransform_.matWorld_ = anim_.GetLocalMatrix() * lockOnObj_->worldTransform_.matWorld_;
	}
}

void Player::Draw(const Camera& camera) {

	bodyObj_->Draw(camera);
	if (isDive_) {
		lockOnObj_->Draw(camera);
	}
}

void Player::ParticleDraw(const Camera& camera){

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
	isDive_ = false;
}

void Player::BehaviorRootUpdate(){
	//前フレームの向きベクトルを記録
	frontVec_ = postureVec_;

	//入力を受け取って移動
	if (input_->GetMoveXZ().z != 0) {
		move_.z = (input_->GetMoveXZ().z / 32767.0f) * moveSpeed_;
	}
	else {

		if (move_.z > 0.0f) {
			move_.z -= 0.01f;
		}
		else if (move_.z < 0.0f) {
			move_.z += 0.01f;
		}
		if (fabsf(move_.z) <= 0.01f) {
			move_.z = 0.0f;
		}
	}

	//入力を受け取って移動
	if (input_->GetMoveXZ().x != 0) {
		move_.x = (input_->GetMoveXZ().x / 32767.0f) * moveSpeed_;
	}
	else {
		if (move_.x > 0.0f) {
			move_.x -= 0.01f;
		}
		else if (move_.x < 0.0f) {
			move_.x += 0.01f;
		}

		if (fabsf(move_.x) <= 0.01f) {
			move_.x = 0.0f;
		}
	}


	//カメラの向きによって移動ベクトルに補正を掛ける
	/*Matrix4x4 newRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(viewProjection_->rotation_);
	move_ = Matrix::GetInstance()->TransformNormal(move_, newRotateMatrix);
	move_.y = 0.0f;
	move_ = Vector3::Mutiply(Vector3::Normalize(move_), moveSpeed_ * 3.0f);
	move_.y = 0.0f;*/
	move_.Normalize();
	move_ *= moveSpeed_;
	move_.y = 0.0f; 

	if (input_->GetJoystickLState()){
		postureVec_ = move_;

		Matrix4x4 directionTodirection;
		directionTodirection = DirectionToDirection(frontVec_, postureVec_);

		playerRotateMat_ = playerRotateMat_ * directionTodirection;

	}
	
	PLTransform_.translation_ += move_;

	if (input_->TriggerButton(Input::Button::A) and !isDown_) {
		downVector_.y += jumpPower_;
		isDown_ = true;
	}

	Gravity();

	if (input_->TriggerButton(Input::Button::RIGHT_SHOULDER) and !isDown_){
		behaviorRequest_ = Behavior::kAvoid;

	}

}

void Player::BehaviorAttackInitialize(){

}

void Player::BehaviorAttackUpdate(){

}

void Player::BehaviorAvoidInitialize(){
	avoidTime_ = 0;
}

void Player::BehaviorAvoidUpdate(){
	Matrix4x4 newRotateMatrix_ = playerRotateMat_;
	move_ = { 0, 0, dashSpeed_ };

	move_ = TransformNormal(move_, newRotateMatrix_);

	//ダッシュの時間<frame>
	const uint32_t behaviorDashTime = 15;

	Vector3 NextPos = PLTransform_.translation_ + move_;

	/*if (NextPos.x >= 97.0f or NextPos.x <= -97.0f) {
		move_.x = 0;
	}
	if (NextPos.z >= 97.0f or NextPos.z <= -97.0f) {
		move_.z = 0;
	}*/
	PLTransform_.translation_ += move_;

	//既定の時間経過で通常状態に戻る
	if (++avoidTime_ >= behaviorDashTime) {
		if (input_->PushButton(Input::Button::RIGHT_SHOULDER)){
			behaviorRequest_ = Behavior::kDash;
		}
		else {

			/*dashCoolTime = kDashCoolTime;
			isDash_ = false;*/
			behaviorRequest_ = Behavior::kRoot;
		}
	}
}

void Player::BehaviorDashInitialize(){

}

void Player::BehaviorDashUpdate(){
	//前フレームの向きベクトルを記録
	frontVec_ = postureVec_;

	//入力を受け取って移動
	if (input_->GetMoveXZ().z != 0) {
		move_.z = (input_->GetMoveXZ().z / 32767.0f) * dashSpeed_;
	}
	else {

		if (move_.z > 0.0f) {
			move_.z -= 0.01f;
		}
		else if (move_.z < 0.0f) {
			move_.z += 0.01f;
		}
		if (fabsf(move_.z) <= 0.01f) {
			move_.z = 0.0f;
		}
	}

	//入力を受け取って移動
	if (input_->GetMoveXZ().x != 0) {
		move_.x = (input_->GetMoveXZ().x / 32767.0f) * dashSpeed_;
	}
	else {
		if (move_.x > 0.0f) {
			move_.x -= 0.01f;
		}
		else if (move_.x < 0.0f) {
			move_.x += 0.01f;
		}

		if (fabsf(move_.x) <= 0.01f) {
			move_.x = 0.0f;
		}
	}


	//カメラの向きによって移動ベクトルに補正を掛ける
	/*Matrix4x4 newRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(viewProjection_->rotation_);
	move_ = Matrix::GetInstance()->TransformNormal(move_, newRotateMatrix);
	move_.y = 0.0f;
	move_ = Vector3::Mutiply(Vector3::Normalize(move_), moveSpeed_ * 3.0f);
	move_.y = 0.0f;*/
	move_.Normalize();
	move_ *= (dashSpeed_);
	move_.y = 0.0f;

	if (input_->GetJoystickLState()) {
		postureVec_ = move_;

		Matrix4x4 directionTodirection;
		directionTodirection = DirectionToDirection(frontVec_, postureVec_);

		playerRotateMat_ = playerRotateMat_ * directionTodirection;

	}
	else {
		if (!isDown_){
			behaviorRequest_ = Behavior::kRoot;
		}
		
	}

	PLTransform_.translation_ += move_;

	if (input_->TriggerButton(Input::Button::A) and !isDown_) {
		downVector_.y += jumpPower_;
		isDown_ = true;
	}

	Gravity();

	if (input_->TriggerButton(Input::Button::RIGHT_SHOULDER) and !isDown_) {
		behaviorRequest_ = Behavior::kAvoid;

	}
}


void Player::Respawn(){
	PLTransform_.translation_.y = 0.0f;
	downVector_ = { 0.0f,0.0f,0.0f };
	postureVec_ = { 0.0f,0.0f,1.0f };
	frontVec_ = { 0.0f,0.0f,1.0f };
}

void Player::Gravity(){
	

	if (isDown_){
		downVector_.y += gravityPower_;
	}
	PLTransform_.translation_.y += downVector_.y;

	if (PLTransform_.translation_.y <= 0.0f){
		OnFloorCollision();
	}
}

void Player::OnFloorCollision(){
	PLTransform_.translation_.y = 0.0f;
	downVector_ = { 0.0f,0.0f,0.0f };
	isDown_ = false;
}

void Player::StageClampCollision(const Vector3& centerTarget) {
	const float kMax = 20.0f;
	const float kMin = 10.0f;

	// 中央から現在地のベクトルを取得
	Vector3 vec = PLTransform_.translation_ - centerTarget;
	// 長さを取得し、クランプ
	float length = vec.Length();

	bool flag = false;
	if (length <= kMin) {
		vec = vec.Normalize() * kMin;
		flag = true;
	}
	else if (length >= kMax) {
		vec = vec.Normalize() * kMax;
		flag = true;
	}

	if (flag) {
		PLTransform_.translation_ = vec;
		PLTransform_.UpdateMatrix();
	}
}
