#include "Stage.h"
#include "ImGuiManager.h"

void Stage::Initialize(const LevelData* data) {

	std::shared_ptr<Model> stagePiece = ModelManager::LoadOBJ("StagePiece");
	std::shared_ptr<Model> electricBoardModel = ModelManager::LoadOBJ("ElectricBoard");
	
	WorldTransform transform;
	transform.Init();
	transform.translation_.y = 0.0f;
	transform.UpdateMatrix();
	for (auto& ground : grounds_) {
		ground = std::make_unique<Ground>();
		ground->Initialize(data, { stagePiece }, transform, electricBoardModel);
		transform.translation_.y -= 15.0f;
		transform.UpdateMatrix();
	}
}

void Stage::Update() {
#ifdef _DEBUG
	ImGui::Begin("Stage");
	static float distance = 10.0f;
	ImGui::DragFloat("Distance", &distance, 0.1f);
	ImGui::End();
	float lDist = distance;
	for (auto& ground : grounds_) {
		ground->transform_.translation_.y = lDist;
		lDist -= distance;
	}

#endif // _DEBUG


	for (auto& ground : grounds_) {
		ground->Update();
	}
}

void Stage::Draw(const Camera& camera) {
	for (auto& ground : grounds_) {
		ground->Draw(camera);
	}
}

void Stage::IsCollision(const Vector3& position) {
	float angle = GetRadianVectorToVector(Vector3(0.0f, 0.0f, 1.0f), position.Normalize());
	if (position.x < 0.0f) {
		angle = -angle;
	}
	float oneRad = 16.0f / 360.0f;

	for (auto& ground : grounds_) {
		ground->IsCollision(angle);
	}
}
