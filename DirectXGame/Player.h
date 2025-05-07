#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

// 自キャラ
class Player {
public:
	// 初期化
	void Initialize(Model* model, uint32_t texturehandle, Camera* camera);

	// 更新
	void Update();

	// 描画
	void Draw();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// カメラ
	Camera* camera_ = nullptr;
};
