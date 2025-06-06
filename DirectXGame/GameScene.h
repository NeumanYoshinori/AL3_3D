#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include <vector>
#include "Skydome.h"
#include "MapChipField.h"

class GameScene {
public:
	~GameScene();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	void GenerateBlocks();

private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
	// 3Dモデルデータの生成
	Model* model_ = nullptr;
	// ワールド変換データ
	WorldTransform worldTransform_;
	// 自キャラ
	Player* player_ = nullptr;
	// カメラ
	Camera camera_;
	// 3Dモデルデータ
	Model* modelBlock_ = nullptr;
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;
	// デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;
	// 天球
	Skydome* skydome_ = nullptr;
	// 3Dモデル
	Model* modelSkydome_ = nullptr;
	// マップチップフィールド
	MapChipField* mapChipField_;
};
