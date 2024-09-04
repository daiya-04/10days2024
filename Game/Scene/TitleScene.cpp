#include "TitleScene.h"

#include "TextureManager.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "SceneManager.h"
#include "Input.h"

TitleScene::~TitleScene() {}

void TitleScene::Init(){

	

}

void TitleScene::Update(){

#ifdef _DEBUG

	if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_2)) {
		SceneManager::GetInstance()->ChangeScene("Game");
	}
	if (Input::GetInstance()->PushKey(DIK_LCONTROL) && Input::GetInstance()->TriggerKey(DIK_3)) {
		SceneManager::GetInstance()->ChangeScene("Debug");
	}

#endif // _DEBUG

	

}

void TitleScene::DrawBackGround(){

	

}

void TitleScene::DrawModel(){



}

void TitleScene::DrawParticleModel(){



}

void TitleScene::DrawParticle(){



}

void TitleScene::DrawUI(){



}

void TitleScene::DrawPostEffect() {



}

void TitleScene::DrawRenderTexture() {



}

void TitleScene::DebugGUI(){
#ifdef _DEBUG




#endif // _DEBUG
}


