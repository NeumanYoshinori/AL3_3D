#pragma once
#include "KamataEngine.h"
#include "PlayerBullet.h"

class Math;

// 自キャラ
class Player {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model, uint32_t texturehandle);

	// 更新
	void Update();

	// 描画
	void Draw(KamataEngine::Camera& camera);

	// 旋回
	void Rotate();

	// 攻撃
	void Attack();

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	// モデル
	KamataEngine::Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// シングルトンインスタンスを取得する
	KamataEngine::Input* input_ = KamataEngine::Input::GetInstance();

	// 数学関数
	Math* math_ = nullptr;

	// 弾
	PlayerBullet* bullet_ = nullptr;
};
