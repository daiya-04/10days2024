#pragma once
#include "Object3d.h"

class Board {
public:
	Board() = default;
	~Board() = default;
	
	void Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
	void SetModel(std::shared_ptr<Model> model);

	void Update();
	void Draw(const Camera& camera);

	void ImGuiProc();
	WorldTransform& GetTransform() const { return model_->worldTransform_; }

private:
	std::unique_ptr<Object3d> model_;


};
