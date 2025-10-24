#pragma once
#include "KamataEngine.h"
#include "Player.h"

class GameScene {
public:
	~GameScene();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
	// 3Dモデルデータの生成
	KamataEngine::Model* model_ = nullptr;

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// 自キャラ
	Player* player_ = nullptr;

	// カメラ
	KamataEngine::Camera camera_;

	// シングルトンインスタンスを取得する
	KamataEngine::Input* input_ = KamataEngine::Input::GetInstance();

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
};
