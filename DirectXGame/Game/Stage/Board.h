#pragma once
#include "Object3d.h"

class Board {
public:
	Board() = default;
	~Board() = default;
	
	void Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
	void SetModel(std::shared_ptr<Model> model);
	void SetTextureHandle(const int32_t& handle) { handle_ = handle; }

	void Update();
	void Draw(const Camera& camera);

	void ImGuiProc();
	WorldTransform& GetTransform() const { return model_->worldTransform_; }
	void StartAnimation(bool flag) {
		IsAnimation_ = flag;
		easeParam_.Reset();
	}

private:
	void Animation();

private:
	std::unique_ptr<Object3d> model_;
	uint32_t handle_;

	struct EaseParam {
		float T = 0.0f;
		float maxCount_ = 120.0f;
		float start_ = 0.0f;
		float end_ = 0.0f;
		void Initialize(float start, float end, float max) {
			start_ = start;
			end_ = end;
			maxCount_ = max;
		}
		void Reset() {
			T = 0.0f;
		}
	};
	EaseParam easeParam_;
	bool IsAnimation_ = false;
};
