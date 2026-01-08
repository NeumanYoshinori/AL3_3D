#pragma once
#include <KamataEngine.h>
#include "Collider.h"

class Player;

// 敵の弾
class EnemyBullet : public Collider{
public:
	// 初期化
	void Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position, const KamataEngine::Vector3& velocity);

	// 更新
	void Update();

	// 描画
	void Draw(const KamataEngine::Camera& camera);

	// デスフラグを取得
	bool IsDead() const { return isDead_; }

	void SetPlayer(Player* player) { player_ = player; }

	// ワールド座標を取得
	KamataEngine::Vector3 GetWorldPosition() override;

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision() override;

	// 半径を取得
	float GetRadius() override { return radius; }

	bool IsDead() { return isDead_; }

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	// 速度
	KamataEngine::Vector3 velocity_;

	// 寿命
	static const int32_t kLifeTime = 60 * 5;

	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// デスフラグ
	bool isDead_ = false;

	Player* player_ = nullptr;

	const float kBulletSpeed = 0.05f;

	float t = 0.2f;

	// 半径
	float radius = 1.5f;
};
