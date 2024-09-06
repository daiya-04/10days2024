#pragma once
#include "Ground.h"
#include <array>

class Stage {
public:
	Stage() = default;
	~Stage() = default;

	void Initialize(const LevelData* data);

	void Draw(const Camera& camera);

private:

	std::array<std::unique_ptr<Ground>, 3> grounds_;

};
