#pragma once
#include "Ground.h"
#include <array>

class Stage {
public:
	Stage() = default;
	~Stage() = default;

	void Initialize(const LevelData* data);

	void Update();

	void Draw(const Camera& camera);

	void IsCollision(const Vector3& position);

private:

	std::array<std::unique_ptr<Ground>, 3> grounds_;

};
