#include "Ground.h"

void Ground::Initialize(const LevelData* data, const std::vector<std::shared_ptr<Model>>& models) {

	for (auto& objectdata : data->objectDatas_) {
		uint32_t index = 1u;
		if (objectdata.objectName.find("Ground") != std::string::npos) {
			auto& piece = pieces_.emplace_back();
			piece->Initialize(objectdata, models);
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
