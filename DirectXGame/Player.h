#pragma once
#include "KamataEngine.h"

class Math;

// 自キャラ
class Player {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model, uint32_t texturehandle, KamataEngine::Camera* camera);

	// 更新
	void Update();

	// 描画
	void Draw();

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	// モデル
	KamataEngine::Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;
	// シングルトンインスタンスを取得する
	KamataEngine::Input* input_ = KamataEngine::Input::GetInstance();

	// 数学関数
	Math* math_ = nullptr;
};
