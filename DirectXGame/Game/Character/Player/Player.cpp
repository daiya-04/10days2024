#include "Player.h"

void Player::SetGlobalVariables(){
	GlobalVariables* global = GlobalVariables::GetInstance();
	//グループの追加
	global->CreateGroup(groupName_);
	//項目の追加
	global->AddItem(groupName_, "moveSpeed", moveSpeed_);
	global->AddItem(groupName_, "dashSpeed", dashSpeed_);
	global->AddItem(groupName_, "avoidSpeed", avoidSpeed_);

	global->AddItem(groupName_, "gravityPower", gravityPower_);
	global->AddItem(groupName_, "jumpPower", jumpPower_);
	global->AddItem(groupName_, "gravityPowerAttack", gravityPowerAttack_);
	global->AddItem(groupName_, "jumpPowerAttack", jumpPowerAttack_);
	global->AddItem(groupName_, "waitTimeBase", waitTimeBase_);
	global->AddItem(groupName_, "waitTimeBaseCharge", waitTimeBaseCharge_);
	global->AddItem(groupName_, "baseAttackSpeed", baseAttackSpeed_);
	global->AddItem(groupName_, "motionDistance", motionDistance_);
	global->AddItem(groupName_, "motionSpeed", motionSpeed_);
	global->AddItem(groupName_, "beseRotateSpeed", beseRotateSpeed_);

}

void Player::ApplyGlobalVariables(){
	GlobalVariables* global = GlobalVariables::GetInstance();
	//項目の追加
	moveSpeed_ = global->GetFloatValue(groupName_, "moveSpeed");
	dashSpeed_ = global->GetFloatValue(groupName_, "dashSpeed");
	avoidSpeed_ = global->GetFloatValue(groupName_, "avoidSpeed");

	gravityPower_ = global->GetFloatValue(groupName_, "gravityPower");
	jumpPower_ = global->GetFloatValue(groupName_, "jumpPower");
	gravityPowerAttack_ = global->GetFloatValue(groupName_, "gravityPowerAttack");
	jumpPowerAttack_ = global->GetFloatValue(groupName_, "jumpPowerAttack");
	waitTimeBase_ = global->GetIntValue(groupName_, "waitTimeBase");
	waitTimeBaseCharge_ = global->GetIntValue(groupName_, "waitTimeBaseCharge");
	baseAttackSpeed_ = global->GetFloatValue(groupName_, "baseAttackSpeed");
	motionDistance_ = global->GetIntValue(groupName_, "motionDistance");
	motionSpeed_ = global->GetFloatValue(groupName_, "motionSpeed");
	beseRotateSpeed_ = global->GetFloatValue(groupName_, "beseRotateSpeed");

}


void Player::Initialize(){
	SetGlobalVariables();

	modelManager_ = ModelManager::GetInstance();

	bodyObj_ = std::make_unique<Object3d>();

	bodyObj_->Initialize(modelManager_->LoadOBJ("box"));

	rightHandObj_ = std::make_unique<Object3d>();

	rightHandObj_->Initialize(modelManager_->LoadOBJ("box"));

	leftHandObj_ = std::make_unique<Object3d>();

	leftHandObj_->Initialize(modelManager_->LoadOBJ("box"));

	input_ = Input::GetInstance();

	PLTransform_.Init();

	PLTransform_.scale_ = { 0.5f,0.5f,0.5f };
	PLTransform_.translation_.z = -10.0f;

	RHandTransform_.Init();

	RHandTransform_.scale_ = { 0.5f,0.5f,0.5f };
	RHandTransform_.translation_.x = 2.0f;
	RHandTransform_.parent_ = &bodyObj_->worldTransform_;

	RRotateHandTransform_.Init();

	RRotateHandTransform_.scale_ = { 1.0f,1.0f,1.0f };
	RRotateHandTransform_.translation_.y = 3.0f;
	RRotateHandTransform_.parent_ = &RHandTransform_;

	LHandTransform_.Init();

	LHandTransform_.scale_ = { 0.5f,0.5f,0.5f };
	LHandTransform_.translation_.x = -2.0f;
	LHandTransform_.parent_ = &bodyObj_->worldTransform_;

	postureVec_ = { 0.0f,0.0f,1.0f };
	frontVec_ = { 0.0f,0.0f,1.0f };



}

void Player::Update(){
	ApplyGlobalVariables();

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
		case Behavior::kChargeAttack:
			BehaviorChargeAttackInitialize();
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
	case Behavior::kChargeAttack:
		BehaviorChargeAttackUpdate();
		break;

	}
	auto front = frontVec_.Normalize();
	auto posture = postureVec_.Normalize();

	if (front == -posture) {
		frontVec_ = postureVec_;
	}

	//行列更新
	bodyObj_->worldTransform_ = PLTransform_;
	bodyObj_->worldTransform_.UpdateMatrixRotate(playerRotateMatX_ * playerRotateMatY_);

	if (isCharge_) {
		rightHandRotateMat_ = MakeRotateXMatrix(chargeRotate_);

		RHandTransform_.UpdateMatrixRotate(rightHandRotateMat_ * playerRotateMatX_.Inverse());

		rightHandObj_->worldTransform_ = RRotateHandTransform_;
		rightHandObj_->worldTransform_.UpdateMatrix();
	}
	else {
		rightHandObj_->worldTransform_ = RHandTransform_;
		rightHandObj_->worldTransform_.UpdateMatrixRotate(playerRotateMatX_.Inverse());

	}

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

	ImGui::DragFloat("RotateSpeed", &beseRotateSpeed_, 0.01f, 0.0f, 3.14f);

	ImGui::End();
}

void Player::BehaviorRootInitialize(){
	workAttack_.comboIndex_ = 0;
	workAttack_.comboNext_ = false;
	RHandTransform_.translation_ = { 2.0f,0.0f,0.0f };
	LHandTransform_.translation_ = { -2.0f,0.0f,0.0f };

	isCharge_ = false;
	workAttack_.chargeAttackNext_ = false;
	workAttack_.chargeFlugTime_ = 0;

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
	Matrix4x4 newRotateMatrix = MakeRotateYMatrix(cameraRotate_->y);
	move_ = TransformNormal(move_, newRotateMatrix);
	move_.y = 0.0f;
	move_.Normalize();
	move_ *= moveSpeed_;
	move_.y = 0.0f; 
	//スティック操作していれば方向ベクトルを更新する
	if (input_->GetJoystickLState() and (move_ != Vector3(0.0f,0.0f,0.0f))) {
		postureVec_ = move_;
		postureVec_.y = 0;
		postureVec_.Normalize();
		postureVec_.y = 0;

		Matrix4x4 directionTodirection;
		directionTodirection = DirectionToDirection(frontVec_, postureVec_);

		playerRotateMatY_ = playerRotateMatY_ * directionTodirection;

	}
	/*else {
		if (input_->PushKey(DIK_UP) or input_->PushKey(DIK_DOWN) or input_->PushKey(DIK_RIGHT) or input_->PushKey(DIK_LEFT)){
			postureVec_ = move_;
			postureVec_.y = 0;
			postureVec_.Normalize();
			postureVec_.y = 0;

			Matrix4x4 directionTodirection;
			directionTodirection = DirectionToDirection(frontVec_, postureVec_);

			playerRotateMatY_ = playerRotateMatY_ * directionTodirection;
		}
	}*/
	
	PLTransform_.translation_ += move_;
	//Aボタンでジャンプ
	if (input_->TriggerButton(Input::Button::A) and !isDown_) {
		downVector_.y += jumpPower_;
		isDown_ = true;
	}
	//重力系の処理
	Gravity();
	//Xボタンで攻撃
	if (input_->TriggerButton(Input::Button::X) and !isDown_) {
		workAttack_.comboIndex_++;
		behaviorRequest_ = Behavior::kAttack;
	}

	//RBボタンで回避ダッシュ
	if (input_->TriggerButton(Input::Button::RIGHT_SHOULDER) and !isDown_){
		behaviorRequest_ = Behavior::kAvoid;

	}

}

void Player::BehaviorAttackInitialize(){
	workAttack_.comboNext_ = false;
	workAttack_.attackParameter_ = 0;
	easeT_ = 0;
	RHandTransform_.translation_ = { 2.0f,0.0f,0.0f };
	LHandTransform_.translation_ = { -2.0f,0.0f,0.0f };

	waitTime_ = waitTimeBase_;
	isEndAttack_ = false;

	isCharge_ = false;
	workAttack_.chargeAttackNext_ = false;
	workAttack_.chargeFlugTime_ = 0;
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
				postureVec_.y = 0;
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
			if (workAttack_.chargeAttackNext_){
				workAttack_.chargeFlugTime_ = 0;
				behaviorRequest_ = Behavior::kChargeAttack;
			}
			 
			if (++workAttack_.attackParameter_ >= ((float)(motionDistance_) / motionSpeed_)) {

				behaviorRequest_ = Behavior::kRoot;
				workAttack_.attackParameter_ = 0;
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
		if (input_->TriggerButton(Input::Button::X)) {
			//コンボ有効
			workAttack_.comboNext_ = true;
		}
		else if (input_->PushButton(Input::Button::X)) {
			workAttack_.chargeFlugTime_++;
			if (workAttack_.chargeFlugTime_ >= 20) {
				//コンボ有効
				workAttack_.chargeAttackNext_ = true;
			}
			
		}
	}
	Gravity();

	

	if (input_->TriggerButton(Input::Button::RIGHT_SHOULDER)) {
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
	workAttack_.comboIndex_ = 0;
	workAttack_.comboNext_ = false;
	RHandTransform_.translation_ = { 2.0f,0.0f,0.0f };
	LHandTransform_.translation_ = { -2.0f,0.0f,0.0f };

	isCharge_ = false;
	workAttack_.chargeAttackNext_ = false;
	workAttack_.chargeFlugTime_ = 0;

	playerRotateMatX_ = MakeRotateXMatrix(0.0f);

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

	if (input_->GetJoystickLState() and (move_ != Vector3(0.0f, 0.0f, 0.0f))) {
		postureVec_ = move_;
		postureVec_.y = 0;
		postureVec_.Normalize();
		postureVec_.y = 0;

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

	if (input_->TriggerButton(Input::Button::A) and !isDown_) {
		downVector_.y += jumpPower_;
		isDown_ = true;
	}

	Gravity();
	//Xボタンで攻撃
	if (input_->TriggerButton(Input::Button::X) and !isDown_) {
		workAttack_.comboIndex_++;
		behaviorRequest_ = Behavior::kAttack;
	}

	if (input_->TriggerButton(Input::Button::RIGHT_SHOULDER) and !isDown_) {
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

	waitTime_ = waitTimeBase_;
	isEndAttack_ = false;

	isCharge_ = false;
	workAttack_.chargeAttackNext_ = false;
	workAttack_.chargeFlugTime_ = 0;
}

void Player::BehaviorThirdAttackInitialize(){
	workAttack_.comboNext_ = false;
	workAttack_.attackParameter_ = 0;
	easeT_ = 0;
	RHandTransform_.translation_ = { 2.0f,0.0f,0.0f };
	LHandTransform_.translation_ = { -2.0f,0.0f,0.0f };

	waitTime_ = waitTimeBase_;
	isEndAttack_ = false;

	isCharge_ = false;
	workAttack_.chargeAttackNext_ = false;
	workAttack_.chargeFlugTime_ = 0;
}

void Player::AttackMotion(){
	easeT_ += baseAttackSpeed_ * motionSpeed_;
	if (easeT_ >= 1.0f) {
		easeT_ = 1.0f;
		waitTime_ -= 1;
	}

	if (waitTime_ <= 0) {
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
		waitTime_ -= 1;
	}

	if (waitTime_ <= 0) {
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
		waitTime_ -= 1;
	}

	if (waitTime_ <= 0) {
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

void Player::BehaviorChargeAttackInitialize(){
	workAttack_.comboNext_ = false;
	workAttack_.attackParameter_ = 0;
	easeT_ = 0;
	RHandTransform_.translation_ = { 2.0f,0.0f,0.0f };
	LHandTransform_.translation_ = { -2.0f,0.0f,0.0f };
	isCharge_ = true;
	chargeRotateSpeed_ = 0;
	chargeTime_ = 0;

	waitTime_ = waitTimeBaseCharge_;
	isEndAttack_ = false;
}

void Player::BehaviorChargeAttackUpdate(){
	if (input_->PushButton(Input::Button::X)){
		chargeRotateSpeed_ = (chargeTime_ / 30) + 1;
		chargeRotate_ += beseRotateSpeed_ * chargeRotateSpeed_;


		chargeTime_++;
		if (chargeTime_ >= 180) {
			chargeTime_ = 180;
		}
	}
	else {
		isCharge_ = false;
		easeT_ += baseAttackSpeed_ * motionSpeed_;
		if (easeT_ >= 1.0f) {
			easeT_ = 1.0f;
			waitTime_ -= 1;
		}

		if (waitTime_ <= 0) {
			behaviorRequest_ = Behavior::kRoot;
		}

		RHandTransform_.translation_.x = Easing::Ease(Easing::EaseName::EaseInBack, 2.0f, 1.0f, easeT_);
		RHandTransform_.translation_.z = Easing::Ease(Easing::EaseName::EaseInBack, -2.0f, 6.0f, easeT_);

		if (easeT_ >= 1.0f) {
			easeT_ = 1.0f;
		}

		
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

	if ((fabsf(downVector_.y) < 0.3f and input_->TriggerButton(Input::Button::X)) and isDown_) {
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

void Player::StageClampCollision(const Vector3& centerTarget) {
	const float kMax = 20.0f;
	const float kMin = 10.0f;

	// 中央から現在地のベクトルを取得
	Vector3 vec = PLTransform_.translation_ - centerTarget;
	//高さに関係なくするためにｙを打ち消す
	vec.y = 0;
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
		PLTransform_.translation_ = { vec.x,PLTransform_.translation_.y,vec.z };
		PLTransform_.UpdateMatrixRotate(playerRotateMatY_);
	}
}
