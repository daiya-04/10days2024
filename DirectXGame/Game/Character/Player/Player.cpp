#include "Player.h"
#include "ShapesDraw.h"

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

	bodyObj_->Initialize(modelManager_->LoadGLTF("PlayerFace"));

	rightHandObj_ = std::make_unique<Object3d>();

	rightHandObj_->Initialize(modelManager_->LoadOBJ("PlayerHand"));

	leftHandObj_ = std::make_unique<Object3d>();

	leftHandObj_->Initialize(modelManager_->LoadOBJ("PlayerHand"));

	shadowObj_= std::make_unique<Object3d>();

	shadowObj_->Initialize(modelManager_->LoadGLTF("shadow"));

	input_ = Input::GetInstance();

	PLTransform_.Init();

	PLTransform_.scale_ = { 0.3f,0.3f,0.3f };

	collider_.radius = 0.4f;

	attackCollider_.center.y = 100.0f;
	reflectionCollider_.center.y = 100.0f;


	PLTransform_.translation_ = Vector3(1.0f, 8.0f, -30.0f);

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

	ShadowTransform_.Init();
	ShadowTransform_.scale_ = { 0.5f,0.005f,0.5f };

	postureVec_ = { 0.0f,0.0f,1.0f };
	frontVec_ = { 0.0f,0.0f,1.0f };

	isDown_ = true;

}

void Player::Update(const Vector3& centerTarget, const Vector2& minAndMax){
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
		case Behavior::kHitCollision:
			BehaviorHitCollosionInitialize();
			break;
		}

	}
	// 振る舞いリクエストをリセット
	behaviorRequest_ = std::nullopt;

	centerPos_ = centerTarget;
	minAndMax_ = minAndMax;

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
	case Behavior::kHitCollision:
		BehaviorHitCollisionUpdate();
		break;

	}
	auto front = frontVec_.Normalize();
	auto posture = postureVec_.Normalize();

	if (front == -posture) {
		frontVec_ = postureVec_;
	}

	//行列更新
	PLTransform_.UpdateMatrix();
	bodyObj_->worldTransform_ = PLTransform_;
	bodyObj_->worldTransform_.translation_.y -= 1.0f;
	collider_.center = bodyObj_->worldTransform_.translation_;
	bodyObj_->worldTransform_.UpdateMatrixRotate(playerRotateMatX_ * playerRotateMatY_);

	if (isCharge_) {
		rightHandRotateMat_ = MakeRotateXMatrix(chargeRotate_);

		RHandTransform_.UpdateMatrixRotate(rightHandRotateMat_ * playerRotateMatX_.Inverse());

		rightHandObj_->worldTransform_ = RRotateHandTransform_;
		rightHandObj_->worldTransform_.UpdateMatrix();
	}
	else {
		if (behavior_ == Behavior::kHitCollision){
			rightHandObj_->worldTransform_ = RHandTransform_;
			rightHandObj_->worldTransform_.UpdateMatrix();
		}
		else {
			rightHandObj_->worldTransform_ = RHandTransform_;
			rightHandObj_->worldTransform_.UpdateMatrixRotate(playerRotateMatX_.Inverse());

		}
		
	}
	if (behavior_ == Behavior::kHitCollision) {
		leftHandObj_->worldTransform_ = LHandTransform_;
		leftHandObj_->worldTransform_.UpdateMatrix();
	}
	else {
		leftHandObj_->worldTransform_ = LHandTransform_;
		leftHandObj_->worldTransform_.UpdateMatrixRotate(playerRotateMatX_.Inverse());
	}
	

	Shadow();

	shadowObj_->worldTransform_ = ShadowTransform_;
	shadowObj_->worldTransform_.UpdateMatrix();
}

void Player::Draw(const Camera& camera) {
#ifdef _DEBUG

	ShapesDraw::DrawSphere(collider_, camera, Vector4(0.0f, 1.0f, 0.0f, 1.0f));

	ShapesDraw::DrawSphere(attackCollider_, camera, Vector4(0.0f, 0.0f, 1.0f, 1.0f));

	ShapesDraw::DrawSphere(reflectionCollider_, camera, Vector4(0.0f, 1.0f, 1.0f, 1.0f));


#endif // _DEBUG

	bodyObj_->Draw(camera);
	rightHandObj_->Draw(camera);
	leftHandObj_->Draw(camera);
	shadowObj_->Draw(camera);
}

void Player::Reset(){
	PLTransform_.translation_ = { -1.0f,8.0f,-30.0f };
	playerRotateMatY_ = MakeRotateYMatrix(0.0f);
	postureVec_ = { 0.0f,0.0f,1.0f };
	frontVec_ = { 0.0f,0.0f,1.0f };

	behaviorRequest_ = Behavior::kRoot;

}

void Player::HitEnemyAttackCollision(){
	behaviorRequest_ = Behavior::kHitCollision;
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

	ImGui::DragFloat("shadowY", &shadowY_, 0.01f);

	ImGui::DragFloat("RotateSpeed", &beseRotateSpeed_, 0.01f, 0.0f, 3.14f);

	ImGui::End();
}

void Player::SetFloorPosition(const float& positionY) {
	floorPositionY_ = positionY;
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

	ColliderReset(attackCollider_);
	ColliderReset(reflectionCollider_);
	
	attackPower_ = 0;
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

	auto PLTransfromNext = PLTransform_.translation_ + move_;

	if (!StageClampCollision(PLTransfromNext)){
		PLTransform_.translation_ += move_;
	}	
	
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

	//SPACEボタンで被弾モーション
	if (input_->TriggerKey(DIK_SPACE)) {
		HitEnemyAttackCollision();
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

	basePlayerRotateMatY_ = playerRotateMatY_;
	attackPower_ = 5;
	yRadian_ = 0;
	hitRecord_.Clear();
}

void Player::BehaviorAttackUpdate(){
	frontVec_ = postureVec_;

	if (isEndAttack_) {
		if (workAttack_.comboNext_) {
			workAttack_.comboIndex_++;

			if (input_->GetMoveXZ().x != 0 || input_->GetMoveXZ().z != 0) {
				postureVec_ = input_->GetMoveXZ();
				Matrix4x4 newRotateMatrix = MakeRotateYMatrix(cameraRotate_->y);
				postureVec_ = TransformNormal(postureVec_, newRotateMatrix);
				postureVec_.y = 0;
				postureVec_.Normalize();
				postureVec_.y = 0;
				Matrix4x4 directionTodirection;
				directionTodirection= DirectionToDirection((frontVec_), (postureVec_));
				playerRotateMatY_ = playerRotateMatY_ * directionTodirection;

			}
			

			if (workAttack_.comboIndex_ == 1) {
				ColliderReset(attackCollider_);
				BehaviorAttackInitialize();
			}
			else if (workAttack_.comboIndex_ == 2) {
				ColliderReset(attackCollider_);
				BehaviorSecondAttackInitialize();
			}
			else if (workAttack_.comboIndex_ == 3) {
				ColliderReset(attackCollider_);
				BehaviorThirdAttackInitialize();
			}
			
		}
		else {
			if (workAttack_.chargeAttackNext_){
				workAttack_.chargeFlugTime_ = 0;
				behaviorRequest_ = Behavior::kChargeAttack;
			}
			 
			if (++workAttack_.attackParameter_ >= ((float)(motionDistance_) / motionSpeed_)) {
				ColliderReset(attackCollider_);
				ColliderReset(reflectionCollider_);
				behaviorRequest_ = Behavior::kRoot;
				workAttack_.attackParameter_ = 0;
				return;
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

	ColliderReset(collider_);

}

void Player::BehaviorAvoidUpdate(){
	Matrix4x4 newRotateMatrix_ = playerRotateMatY_;
	move_ = { 0, 0, dashSpeed_ };

	move_ = TransformNormal(move_, newRotateMatrix_);

	
	//ダッシュの時間<frame>
	const uint32_t behaviorDashTime = 15;

	auto PLTransfromNext = PLTransform_.translation_ + move_;

	if (!StageClampCollision(PLTransfromNext)) {
		PLTransform_.translation_ += move_;
	}

	//既定の時間経過で通常状態に戻る
	if (++avoidTime_ >= behaviorDashTime) {
		collider_.radius = 0.4f;
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
	Matrix4x4 newRotateMatrix = MakeRotateYMatrix(cameraRotate_->y);
	move_ = TransformNormal(move_, newRotateMatrix);
	move_.y = 0.0f;
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

	auto PLTransfromNext = PLTransform_.translation_ + move_;

	if (!StageClampCollision(PLTransfromNext)) {
		PLTransform_.translation_ += move_;
	}

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
	basePlayerRotateMatY_ = playerRotateMatY_;

	xRadian_ = 0;
	yRadian_ = 0;
	fallingEaseT_ = 0.0f;
	waitTime_ = waitTimeBase_;
	attackPower_ = 10;
	hitRecord_.Clear();
}

void Player::BehaviorFallingAttackUpdate(){
	downVector_.y += gravityPowerAttack_;

	PLTransform_.translation_.y += downVector_.y;


	auto radian = 0.52f;
	
	if (downVector_.y <= 0.0f) {
		fallingEaseT_ += 0.1f;
		attackCollider_.radius = 1.2f;
		attackCollider_.center = (bodyObj_->worldTransform_.GetWorldPosition() + RHandTransform_.GetWorldPosition()) / 2.0f;

	}

	if (fallingEaseT_ > 1.0f) {

		fallingEaseT_ = 1.0f;
	}
	
	xRadian_ = Easing::Ease(Easing::EaseName::EaseInBack, -0.52f, 1.04f, fallingEaseT_);
	yRadian_ = Easing::Ease(Easing::EaseName::EaseInBack, 0.52f, -0.52f, fallingEaseT_);

	RHandTransform_.translation_.z = Easing::Ease(Easing::EaseName::EaseInBack, 0, 3.0f, fallingEaseT_);
	LHandTransform_.translation_.z = Easing::Ease(Easing::EaseName::EaseInBack, 2.0f, -1.0f, fallingEaseT_);

	RHandTransform_.UpdateMatrix();

	playerRotateMatX_ = MakeRotateXMatrix(xRadian_);
	playerRotateMatY_ = basePlayerRotateMatY_ * MakeRotateYMatrix(yRadian_);

	if (PLTransform_.translation_.y <= floorPositionY_) {
		OnFloorCollision();
		waitTime_--;
	}

	if (waitTime_ < 0) {
		behaviorRequest_ = Behavior::kRoot;
		playerRotateMatY_ = basePlayerRotateMatY_;
	}
}

void Player::BehaviorHitCollosionInitialize(){
	avoidTime_ = 0;
	RHandTransform_.translation_ = { 2.0f,0.0f,0.0f };
	LHandTransform_.translation_ = { -2.0f,0.0f,0.0f };
	downVector_.y += jumpPower_ / 2.0f;
	hitRotateX_ = 0;
	isDown_ = true;
}

void Player::BehaviorHitCollisionUpdate(){
	Matrix4x4 newRotateMatrix_ = playerRotateMatY_;
	move_ = { 0, 0, -(dashSpeed_ / 2.0f) };

	move_ = TransformNormal(move_, newRotateMatrix_);

	//重力系の処理
	Gravity();
	//ダッシュの時間<frame>

	hitRotateX_ += 0.5f;

	playerRotateMatX_ = MakeRotateXMatrix(hitRotateX_);

	const uint32_t behaviorDashTime = 15;

	auto PLTransfromNext = PLTransform_.translation_ + move_;

	if (!StageClampCollision(PLTransfromNext)) {
		PLTransform_.translation_ += move_;
	}

	//既定の時間経過で通常状態に戻る
	if (++avoidTime_ >= behaviorDashTime) {
		
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
	hitRecord_.Clear();
	attackPower_ = 5;

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
	hitRecord_.Clear();
	attackPower_ = 10;

}

void Player::AttackMotion(){
	easeT_ += baseAttackSpeed_ * motionSpeed_;
	if (easeT_ >= 1.0f) {
		attackCollider_.radius = 0.7f;
		attackCollider_.center = RHandTransform_.GetWorldPosition();
		easeT_ = 1.0f;
		waitTime_ -= 1;
	}

	if (waitTime_ <= 0) {
		isEndAttack_ = true;
	}

	RHandTransform_.translation_.x = Easing::Ease(Easing::EaseName::EaseInBack, 2.0f, 1.0f, easeT_);
	RHandTransform_.translation_.z = Easing::Ease(Easing::EaseName::EaseInBack, 0.0f, 6.0f, easeT_);

	RHandTransform_.UpdateMatrix();

	

	LHandTransform_.translation_.z = Easing::Ease(Easing::EaseName::EaseInBack, 3.0f, -2.0f, easeT_);

	if (easeT_ >= 1.0f) {
		easeT_ = 1.0f;
	}
}

void Player::secondAttackMotion(){
	easeT_ += baseAttackSpeed_ * motionSpeed_;
	if (easeT_ >= 1.0f) {
		attackCollider_.radius = 0.7f;
		attackCollider_.center = LHandTransform_.GetWorldPosition();

		easeT_ = 1.0f;
		waitTime_ -= 1;
	}

	if (waitTime_ <= 0) {
		isEndAttack_ = true;
	}

	LHandTransform_.translation_.x = Easing::Ease(Easing::EaseName::EaseInBack, -2.0f, -1.0f, easeT_);
	LHandTransform_.translation_.z = Easing::Ease(Easing::EaseName::EaseInBack, 0.0f, 6.0f, easeT_);

	LHandTransform_.UpdateMatrix();

	RHandTransform_.translation_.z = Easing::Ease(Easing::EaseName::EaseInBack, 3.0f, -2.0f, easeT_);


	if (easeT_ >= 1.0f) {
		easeT_ = 1.0f;
	}
}

void Player::thirdAttackMotion(){
	easeT_ += baseAttackSpeed_ * motionSpeed_;
	if (easeT_ >= 1.0f) {
		attackCollider_.radius = 0.9f;
		attackCollider_.center = (LHandTransform_.GetWorldPosition() + RHandTransform_.GetWorldPosition()) / 2.0f;

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

	RHandTransform_.UpdateMatrix();
	LHandTransform_.UpdateMatrix();

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
	ColliderReset(attackCollider_);

	waitTime_ = waitTimeBaseCharge_;
	isEndAttack_ = false;
	hitRecord_.Clear();
}

void Player::BehaviorChargeAttackUpdate(){
	if (input_->PushButton(Input::Button::X)){
		chargeRotateSpeed_ = (chargeTime_ / 30) + 1;
		chargeRotate_ += beseRotateSpeed_ * chargeRotateSpeed_;
		reflectionCollider_.center = bodyObj_->worldTransform_.GetWorldPosition();
		reflectionCollider_.radius = 1.0f;
		

		chargeTime_++;
		if (chargeTime_ >= 180) {
			chargeTime_ = 180;
		}
		attackPower_ = 5 + (int32_t)(20 * (float)((float)(chargeTime_) / 180.0f));
	}
	else {
		ColliderReset(reflectionCollider_);
		isCharge_ = false;
		easeT_ += baseAttackSpeed_ * motionSpeed_;
		if (easeT_ >= 1.0f) {
			attackCollider_.radius = 0.7f + (1.4f * (float)((float)(chargeTime_) / 180.0f));
			attackCollider_.center = RHandTransform_.GetWorldPosition();

			easeT_ = 1.0f;
			waitTime_ -= 1;
		}

		if (waitTime_ <= 0) {
			behaviorRequest_ = Behavior::kRoot;
		}

		RHandTransform_.translation_.x = Easing::Ease(Easing::EaseName::EaseInBack, 2.0f, 1.0f, easeT_);
		RHandTransform_.translation_.z = Easing::Ease(Easing::EaseName::EaseInBack, -2.0f, 6.0f, easeT_);

		RHandTransform_.UpdateMatrix();

		if (easeT_ >= 1.0f) {
			easeT_ = 1.0f;
		}

		
	}
	Gravity();
}


void Player::Respawn(){
	PLTransform_.translation_.y = 0.0f;
	downVector_ = { 0.0f,0.0f,0.0f };
	postureVec_ = { 0.0f,0.0f,1.0f };
	frontVec_ = { 0.0f,0.0f,1.0f };
}

void Player::Shadow(){
	//層によって値を変更しなければいけないので一旦保留

	ShadowTransform_.translation_ = PLTransform_.translation_;

	ShadowTransform_.translation_.y = shadowY_ + floorPositionY_;

	ShadowTransform_.scale_.x = 0.5f * (groundLength_ / (groundLength_ + (PLTransform_.translation_.y - floorPositionY_)));
	ShadowTransform_.scale_.z = 0.5f * (groundLength_ / (groundLength_ + (PLTransform_.translation_.y - floorPositionY_)));

	if (ShadowTransform_.scale_.x < 0.1f) {
		ShadowTransform_.scale_.x = 0.0f;
		ShadowTransform_.scale_.z = 0.0f;
	}
}

void Player::Gravity(){
	

	if (isDown_){
		downVector_.y += gravityPower_;
	}

	if ((fabsf(downVector_.y) < 0.3f and input_->TriggerButton(Input::Button::X)) and isDown_) {
		behaviorRequest_ = Behavior::kFallingAttack;
	}


	PLTransform_.translation_.y += downVector_.y;

	if (PLTransform_.translation_.y < floorPositionY_){
		OnFloorCollision();
	}
}

void Player::OnFloorCollision(){
	PLTransform_.translation_.y = floorPositionY_;
	downVector_ = { 0.0f,0.0f,0.0f };
	isDown_ = false;
}

void Player::ColliderReset(Sphere& collider){
	collider.center = { 0.0f,100.0f,0.0f };
	collider.radius = 0.0f;
}

bool Player::StageClampCollision(const Vector3& worldTrans){
	const float kMin = minAndMax_.x;
	const float kMax = minAndMax_.y;

	// 中央から現在地のベクトルを取得
	Vector3 vec = worldTrans - centerPos_;
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
		return true;
	}

	return false;
}



