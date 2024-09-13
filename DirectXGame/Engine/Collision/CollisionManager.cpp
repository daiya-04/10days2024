#include "CollisionManager.h"

void CollisionManager::Initialize(Player* player, Boss* boss){
	player_ = player;
	boss_ = boss;
}

void CollisionManager::TitleInitialize(Player* player, Sandbag* sandbag){
	player_ = player;
	sandbag_ = sandbag;
}

void CollisionManager::AllCollision(){
	if (boss_) {
		if (IsCollision(player_->GetAttackCollider(), boss_->GetCollider())) {
			//履歴に残っているかチェックし連続で当たらないように
			if (player_->RecordCheck(0)) {
				return;
			}
			//接触履歴に登録
			player_->AddRecord(0);
			boss_->HitPlayerAttackCollision(player_->GetAttackPower());
			player_->HitEffectInit();
			player_->PlayingHitSE();
		}
	}
	else if (sandbag_){
		if (IsCollision(player_->GetAttackCollider(), sandbag_->GetCollider())) {
			//履歴に残っているかチェックし連続で当たらないように
			if (player_->RecordCheck(1)) {
				return;
			}
			//接触履歴に登録
			player_->AddRecord(1);
			sandbag_->OnCollision(player_->GetAttackPower());
			player_->HitEffectInit();
			player_->PlayingHitSE();
		}
	}
}
