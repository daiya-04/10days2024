#pragma once
#include "Ground.h"
#include <array>
#include "Stage/Board.h"

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

	bool IsCollision(const Vector3& position, const int32_t& damage = 0);

	/// <summary>
	/// playerの足元に床が存在するか否か
	/// </summary>
	/// <param name="position">playerの座標</param>
	/// <returns>床があればtrue / 床がなければfalse</returns>
	bool IsPlayerCollision(const Vector3& position);

	bool ResetCheck(const Vector3& position);

	bool GetIsEnd()const { return isEnd_; }
	Vector3 GetGroundPosition() const;
	Vector3 GetGroundPosition(int32_t index) const;
	Vector3 GetNextGroundPosition() ;
	uint32_t GetLayerNumber() const { return nowLayerNumber_; } // 現在の階層番号の取得 012 = 上中下
	std::array<bool, 16u> GetPieceAlive() const { return pieceAlives_; } // 現在のLayerのpieceの生存状況をフラグで取得
	bool GetChangeLayer() const { return nowLayerNumber_ != preLayerNumber_; } // layerが切替わったタイミングのフラグを取得

public: // title用
	// 調整項目の代入
	void ApplyGlobalVariables();
	// 調整項目の追加
	void SetGlobalVariables();
	// セーブ用
	void SaveGlobalVariables() const;
	// 初期化という名の看板配置
	void TitleInitialize();

	void TitleUpdate();

private:
	void ImGuiProc();

private:
	std::string LayerCheck(const float& playerPositionY);

private:
	std::array<std::unique_ptr<Ground>, 3> grounds_;
	uint32_t nowLayerNumber_ = 0u;
	uint32_t preLayerNumber_ = 0u;
	std::array<bool, 16u> pieceAlives_;
	float distance_ = 10.0f;

	bool isEnd_ = false;

	std::vector<std::unique_ptr<Board>> boards_;
	struct BoardParam {
		int32_t number; // boardの数

	};
	BoardParam boardParam_;
	std::string groupName_ = "boards";
	std::shared_ptr<Model> boardModel_;

};
