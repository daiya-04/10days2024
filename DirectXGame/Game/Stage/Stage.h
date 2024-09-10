#pragma once
#include "Ground.h"
#include <array>

class Stage {
public:
	Stage() = default;
	~Stage() = default;

	std::array<std::string, 3> layer{
		"Up",
		"Center",
		"Down",
	};


	void Initialize(const LevelData* data);

	void Update();

	void Draw(const Camera& camera);

	void IsCollision(const Vector3& position);

	/// <summary>
	/// playerの足元に床が存在するか否か
	/// </summary>
	/// <param name="position">playerの座標</param>
	/// <returns>床があればtrue / 床がなければfalse</returns>
	bool IsPlayerCollision(const Vector3& position);

	bool ResetCheck(const Vector3& position);

	Vector3 GetGroundPosition() const;
	Vector3 GetNextGroundPosition() const;

private:
	std::string LayerCheck(const float& playerPositionY);

private:
	std::array<std::unique_ptr<Ground>, 3> grounds_;
	uint32_t nowLayerNumber_ = 0u;
	float distance_ = 15.0f;

};
