#include "Player.h"

void Player::Initialize(){
	modelManager_ = ModelManager::GetInstance();

	bodyObj_ = std::make_unique<Object3d>();

	bodyObj_->Initialize(modelManager_->LoadOBJ("box"));

	rightHandObj_ = std::make_unique<Object3d>();

	rightHandObj_->Initialize(modelManager_->LoadOBJ("box"));

	leftHandObj_ = std::make_unique<Object3d>();

	leftHandObj_->Initialize(modelManager_->LoadOBJ("box"));

	input_ = Input::GetInstance();

	PLTransform_.Init();

	PLTransform_.scale_ = { 1.0f,1.0f,1.0f };

	RHandTransform_.Init();

	RHandTransform_.scale_ = { 0.5f,0.5f,0.5f };
	RHandTransform_.translation_.x = 2.0f;
	RHandTransform_.parent_ = &bodyObj_->worldTransform_;

	LHandTransform_.Init();

	LHandTransform_.scale_ = { 0.5f,0.5f,0.5f };
	LHandTransform_.translation_.x = -2.0f;
	LHandTransform_.parent_ = &bodyObj_->worldTransform_;


	postureVec_ = { 0.0f,0.0f,1.0f };
	frontVec_ = { 0.0f,0.0f,1.0f };

	anim_ = AnimationManager::Load("LockOn");
	anim_.SetAnimationSpeed(1.0f / 90.0f);


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
		case Behavior::kFallingAttack:
			BehaviorFallingAttackInitialize();
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
	case Behavior::kFallingAttack:
		BehaviorFallingAttackUpdate();
		break;

	}
	auto front = frontVec_.Normalize();
	auto posture = postureVec_.Normalize();

	if (front == -posture) {
		frontVec_ = postureVec_;
	}
	//playerRotateMatX_ = MakeRotateXMatrix(0.52f);

	//行列更新
	bodyObj_->worldTransform_ = PLTransform_;
	bodyObj_->worldTransform_.UpdateMatrixRotate(playerRotateMatX_ * playerRotateMatY_);

	rightHandObj_->worldTransform_ = RHandTransform_;
	rightHandObj_->worldTransform_.UpdateMatrixRotate(playerRotateMatX_.Inverse());

	leftHandObj_->worldTransform_ = LHandTransform_;
	leftHandObj_->worldTransform_.UpdateMatrixRotate(playerRotateMatX_.Inverse());
}

void Player::Draw(const Camera& camera) {

	bodyObj_->Draw(camera);
	rightHandObj_->Draw(camera);
	leftHandObj_->Draw(camera);
}

void Player::ParticleDraw(const Camera& camera){

}

void Player::Imgui(){
	auto inputVector = input_->GetMoveXZ();

	ImGui::Begin("プレイヤー");
	
	ImGui::DragFloat3("move", &move_.x, 0.1f); 
	ImGui::DragFloat3("downvector", &downVector_.x, 0.1f);
	ImGui::DragFloat3("input", &inputVector.x, 0.1f);

	ImGui::DragFloat3("postire", &postureVec_.x, 0.1f);
	ImGui::DragFloat3("front", &frontVec_.x, 0.1f);

	ImGui::DragFloat("gravityAttack", &gravityPowerAttack_, 0.01f);
	ImGui::DragFloat("jumpPowerAttack", &jumpPowerAttack_, 0.01f);

	ImGui::DragFloat("motionSpeed", &motionSpeed_, 0.01f, 0.0f, 10.0f);
	ImGui::DragInt("motionDistance", &(motionDistance_), 1, 0, 50);

	ImGui::DragFloat3("rightHand", &RHandTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("leftHand", &LHandTransform_.translation_.x, 0.1f);

	ImGui::End();
}

void Player::BehaviorRootInitialize(){
	isDive_ = false;
	workAttack_.comboIndex_ = 0;
	workAttack_.comboNext_ = false;
	RHandTransform_.translation_ = { 2.0f,0.0f,0.0f };
	LHandTransform_.translation_ = { -2.0f,0.0f,0.0f };

	playerRotateMatX_ = MakeRotateXMatrix(0.0f);

}

void Player::BehaviorRootUpdate(){
	//前フレームの向きベクトルを記録
	frontVec_ = postureVec_;

	//入力を受け取って移動
	if (input_->GetMoveXZ().z != 0) {
		move_.z = (input_->GetMoveXZ().z / 32767.0f) * moveSpeed_;
	}
	/*else if (input_->PushKey(DIK_UP)){
		move_.z += moveSpeed_;
	}
	else if (input_->PushKey(DIK_DOWN)) {
		move_.z -= moveSpeed_;
	}*/
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
	/*else if (input_->PushKey(DIK_RIGHT)) {
		move_.x += moveSpeed_;
	}
	else if (input_->PushKey(DIK_LEFT)) {
		move_.x -= moveSpeed_;
	}*/
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
	//スティック操作していれば方向ベクトルを更新する
	if (input_->GetJoystickLState()){
		postureVec_ = move_;

		Matrix4x4 directionTodirection;
		directionTodirection = DirectionToDirection(frontVec_, postureVec_);

		playerRotateMatY_ = playerRotateMatY_ * directionTodirection;

	}
	else {
		/*if (input_->PushKey(DIK_UP) or input_->PushKey(DIK_DOWN) or input_->PushKey(DIK_RIGHT) or input_->PushKey(DIK_LEFT)){
			postureVec_ = move_;

			Matrix4x4 directionTodirection;
			directionTodirection = DirectionToDirection(frontVec_, postureVec_);

			playerRotateMat_ = playerRotateMat_ * directionTodirection;
		}*/
	}
	
	PLTransform_.translation_ += move_;
	//Aボタンでジャンプ
	if (input_->TriggerButton(XINPUT_GAMEPAD_A) and !isDown_) {
		downVector_.y += jumpPower_;
		isDown_ = true;
	}
	//重力系の処理
	Gravity();
	//Xボタンで攻撃
	if (input_->TriggerButton(XINPUT_GAMEPAD_X) and !isDown_) {
		workAttack_.comboIndex_++;
		behaviorRequest_ = Behavior::kAttack;
	}

	//RBボタンで回避ダッシュ
	if (input_->TriggerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER) and !isDown_){
		behaviorRequest_ = Behavior::kAvoid;

	}

}

void Player::BehaviorAttackInitialize(){
	workAttack_.comboNext_ = false;
	workAttack_.attackParameter_ = 0;
	easeT_ = 0;
	RHandTransform_.translation_ = { 2.0f,0.0f,0.0f };
	LHandTransform_.translation_ = { -2.0f,0.0f,0.0f };

	workAttack_.AttackTimer_ = 0;
	WaitTime_ = WaitTimeBase_;
	isEndAttack_ = false;
	isShakeDown_ = false;
}

void Player::BehaviorAttackUpdate(){
	frontVec_ = postureVec_;

	if (isEndAttack_) {
		if (workAttack_.comboNext_) {
			workAttack_.comboIndex_++;

			if (input_->GetMoveXZ().x != 0 || input_->GetMoveXZ().z != 0) {
				postureVec_ = input_->GetMoveXZ();
				/*Matrix4x4 newRotateMatrix = Matrix::GetInstance()->MakeRotateMatrix(viewProjection_->rotation_);
				postureVec_ = Matrix::GetInstance()->TransformNormal(postureVec_, newRotateMatrix);*/
				postureVec_.y = 0;
				postureVec_.Normalize();
				Matrix4x4 directionTodirection;
				directionTodirection= DirectionToDirection((frontVec_), (postureVec_));
				playerRotateMatY_ = playerRotateMatY_ * directionTodirection;

			}
			

			if (workAttack_.comboIndex_ == 1) {
				BehaviorAttackInitialize();
			}
			else if (workAttack_.comboIndex_ == 2) {
				BehaviorSecondAttackInitialize();
			}
			else if (workAttack_.comboIndex_ == 3) {
				BehaviorThirdAttackInitialize();
			}
			
		}
		else {

			 {
				if (++workAttack_.attackParameter_ >= ((float)(motionDistance_) / motionSpeed_)) {

					behaviorRequest_ = Behavior::kRoot;
					workAttack_.attackParameter_ = 0;
				}
			}

		}

	}
	
	switch (workAttack_.comboIndex_) {
	case 1:
		AttackMotion();
		break;
	case 2:
		secondAttackMotion();
		break;
	case 3:
		thirdAttackMotion();
		break;
	default:
		break;
	}
	//コンボ上限に達していない
	if (workAttack_.comboIndex_ < conboNum_) {
		if (input_->TriggerButton(XINPUT_GAMEPAD_X)) {
			//コンボ有効
			workAttack_.comboNext_ = true;
		}
	}
	Gravity();

	/*if (workAttack_.attackParameter_ >= 35) {
		if (workAttack_.comboNext_) {


		}
		else {

			workAttack_.attackParameter_ = 0;


			behaviorRequest_ = Behavior::kRoot;
		}
	}*/

	if (input_->TriggerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		behaviorRequest_ = Behavior::kAvoid;
	}
}

void Player::BehaviorAvoidInitialize(){
	avoidTime_ = 0;
	RHandTransform_.translation_ = { 2.0f,0.0f,0.0f };
	LHandTransform_.translation_ = { -2.0f,0.0f,0.0f };

}

void Player::BehaviorAvoidUpdate(){
	Matrix4x4 newRotateMatrix_ = playerRotateMatY_;
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
		if (input_->PushButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)){
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
	RHandTransform_.translation_ = { 2.0f,0.0f,0.0f };
	LHandTransform_.translation_ = { -2.0f,0.0f,0.0f };

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

		playerRotateMatY_ = playerRotateMatY_ * directionTodirection;

	}
	else {
		if (!isDown_){
			behaviorRequest_ = Behavior::kRoot;
		}
		
	}

	PLTransform_.translation_ += move_;

	if (input_->TriggerButton(XINPUT_GAMEPAD_A) and !isDown_) {
		downVector_.y += jumpPower_;
		isDown_ = true;
	}

	Gravity();

	if (input_->TriggerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER) and !isDown_) {
		behaviorRequest_ = Behavior::kAvoid;

	}
}

void Player::BehaviorFallingAttackInitialize(){
	downVector_ = { 0.0f,jumpPowerAttack_,0.0f };
}

void Player::BehaviorFallingAttackUpdate(){
	downVector_.y += gravityPowerAttack_;

	PLTransform_.translation_.y += downVector_.y;

	if (PLTransform_.translation_.y <= 0.0f) {
		OnFloorCollision();
		behaviorRequest_ = Behavior::kRoot;
	}
}

void Player::BehaviorSecondAttackInitialize(){
	workAttack_.comboNext_ = false;
	workAttack_.attackParameter_ = 0;
	easeT_ = 0;
	RHandTransform_.translation_ = { 2.0f,0.0f,0.0f };
	LHandTransform_.translation_ = { -2.0f,0.0f,0.0f };

	workAttack_.AttackTimer_ = 0;
	WaitTime_ = WaitTimeBase_;
	isEndAttack_ = false;
	isShakeDown_ = false;
}

void Player::BehaviorThirdAttackInitialize(){
	workAttack_.comboNext_ = false;
	workAttack_.attackParameter_ = 0;
	workAttack_.AttackTimer_ = 0;
	easeT_ = 0;
	RHandTransform_.translation_ = { 2.0f,0.0f,0.0f };
	LHandTransform_.translation_ = { -2.0f,0.0f,0.0f };

	WaitTime_ = WaitTimeBase_;
	isEndAttack_ = false;
	isShakeDown_ = false;
}

void Player::AttackMotion(){
	easeT_ += baseAttackSpeed_ * motionSpeed_;
	if (easeT_ >= 1.0f) {
		easeT_ = 1.0f;
		WaitTime_ -= 1;
	}
	else {
		workAttack_.AttackTimer_++;
	}

	if (WaitTime_ <= 0) {
		isEndAttack_ = true;
	}

	RHandTransform_.translation_.x = Easing::Ease(Easing::EaseName::EaseInBack, 2.0f, 1.0f, easeT_);
	RHandTransform_.translation_.z = Easing::Ease(Easing::EaseName::EaseInBack, 0.0f, 6.0f, easeT_);

	if (easeT_ >= 1.0f) {
		easeT_ = 1.0f;
	}
}

void Player::secondAttackMotion(){
	easeT_ += baseAttackSpeed_ * motionSpeed_;
	if (easeT_ >= 1.0f) {
		easeT_ = 1.0f;
		WaitTime_ -= 1;
	}
	else {
		workAttack_.AttackTimer_++;
	}

	if (WaitTime_ <= 0) {
		isEndAttack_ = true;
	}

	LHandTransform_.translation_.x = Easing::Ease(Easing::EaseName::EaseInBack, -2.0f, -1.0f, easeT_);
	LHandTransform_.translation_.z = Easing::Ease(Easing::EaseName::EaseInBack, 0.0f, 6.0f, easeT_);

	if (easeT_ >= 1.0f) {
		easeT_ = 1.0f;
	}
}

void Player::thirdAttackMotion(){
	easeT_ += baseAttackSpeed_ * motionSpeed_;
	if (easeT_ >= 1.0f) {
		easeT_ = 1.0f;
		WaitTime_ -= 1;
	}
	else {
		workAttack_.AttackTimer_++;
	}

	if (WaitTime_ <= 0) {
		isEndAttack_ = true;
	}

	auto radian = Easing::Ease(Easing::EaseName::EaseInBack, 0.0f, 0.52f, easeT_);

	playerRotateMatX_ = MakeRotateXMatrix(radian);

	RHandTransform_.translation_.x = Easing::Ease(Easing::EaseName::EaseInBack, 2.0f, 1.0f, easeT_);
	RHandTransform_.translation_.y = Easing::Ease(Easing::EaseName::EaseInBack, 0.0f, 3.0f, easeT_);
	RHandTransform_.translation_.z = Easing::Ease(Easing::EaseName::EaseInBack, 0.0f, 5.0f, easeT_);

	LHandTransform_.translation_.x = Easing::Ease(Easing::EaseName::EaseInBack, -2.0f, -1.0f, easeT_);
	LHandTransform_.translation_.y = Easing::Ease(Easing::EaseName::EaseInBack, 0.0f, 3.0f, easeT_);
	LHandTransform_.translation_.z = Easing::Ease(Easing::EaseName::EaseInBack, 0.0f, 5.0f, easeT_);

	if (easeT_ >= 1.0f) {
		easeT_ = 1.0f;
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

	if ((fabsf(downVector_.y) < 0.3f and input_->TriggerButton(XINPUT_GAMEPAD_X)) and isDown_) {
		behaviorRequest_ = Behavior::kFallingAttack;
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
