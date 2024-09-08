#include "Ground.h"
#include "Hit.h"
#include "Engine/DebugText/ImGuiManager.h"

void Ground::Initialize(const LevelData* data, const std::vector<std::shared_ptr<Model>>& models, WorldTransform transform) {
	transform_ = transform;
	transform_.scale_ = Vector3(1.0f, 1.0f, 1.0f);
	transform_.UpdateMatrix();
	uint32_t index = 0u;
	for (auto& objectdata : data->objectDatas_) {
		if (objectdata.objectName.find("Ground") != std::string::npos) {
			auto& piece = pieces_.emplace_back(std::make_unique<GroundPiece>());
			piece->Initialize(objectdata, models, &transform_);
		}
	}
}

void Ground::Update() {
	ImGui::Begin("Ground");
	static float mastarScale = 30.0f;
	ImGui::DragFloat("MasterScale", &mastarScale, 0.1f, 0.1f);
	transform_.scale_ = Vector3(mastarScale, mastarScale, mastarScale);
	transform_.UpdateMatrix();

	ImGui::End();

	for (auto& piece : pieces_) {
		piece->Update();
	}
}

void Ground::Draw(const Camera& camera) {
	for (auto& piece : pieces_) {
		piece->Draw(camera);
	}
}
