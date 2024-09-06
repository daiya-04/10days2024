#include "Ground.h"
#include "Hit.h"

void Ground::Initialize(const LevelData* data, const std::vector<std::shared_ptr<Model>>& models, WorldTransform transform) {
	transform_ = transform;
	for (auto& objectdata : data->objectDatas_) {
		//uint32_t index = 1u;
		if (objectdata.objectName.find("Ground") != std::string::npos) {
			auto& piece = pieces_.emplace_back(std::make_unique<GroundPiece>());
			piece->Initialize(objectdata, models, &transform_);
		}
	}
	
}

void Ground::Update() {
	for (auto& piece : pieces_) {
		piece->Update();
	}
}

void Ground::Draw(const Camera& camera) {
	for (auto& piece : pieces_) {
		piece->Draw(camera);
	}
}
