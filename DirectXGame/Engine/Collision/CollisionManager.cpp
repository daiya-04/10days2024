#include "CollisionManager.h"

void CollisionManager::Initialize(Player* player, Boss* boss){
	player_ = player;
	boss_ = boss;
}

void CollisionManager::AllCollision(){
	if (IsCollision(player_->GetAttackCollider(), boss_->GetCollider())) {
		//履歴に残っているかチェックし連続で当たらないように
		if (player_->RecordCheck(0)) {
			return;
		}
		//接触履歴に登録
		player_->AddRecord(0);
		boss_->HitPlayerAttackCollision();
	}
}
