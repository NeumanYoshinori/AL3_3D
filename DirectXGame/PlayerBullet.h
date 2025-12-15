#pragma once
#include <KamataEngine.h>

class Math;

// 自キャラの弾
class PlayerBullet {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position, const KamataEngine::Vector3& velocity);

	// 更新
	void Update();

	// 描画
	void Draw(const KamataEngine::Camera& camera);

	bool IsDead() const { return isDead_; }

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision();

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	static KamataEngine::Model* model_;

	// テクスチャハンドル
	uint32_t textureHandle_;

	// 速度
	KamataEngine::Vector3 velocity_;

	// 寿命
	static const int32_t kLifeTime = 60 * 5;

	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// デスフラグ
	bool isDead_ = false;
};
