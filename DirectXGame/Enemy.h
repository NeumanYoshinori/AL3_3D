#pragma once
#include <KamataEngine.h>
#include "EnemyBullet.h"
#include "Collider.h"

// 自機クラスの前方宣言
class Player;

// 敵
class Enemy : public Collider {
public:
	// 行動フェーズ
	enum class Phase {
		Approach, // 接近する
		Leave, // 離脱する
	};

	// デストラクタ
	~Enemy();

	// 初期化
	void Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position);

	// 更新
	void Update();

	// 接近フェーズ初期化
	void ApproachInitialize();

	// 接近
	void Approach();

	// 離脱
	void Leave();

	// 弾発射
	void Fire();

	// 描画
	void Draw(const KamataEngine::Camera& camera);

	void SetPlayer(Player* player) { player_ = player; }

	// ワールド座標の取得
	KamataEngine::Vector3 GetWorldPosition() override;

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision() override;

	// 弾リストを取得
	const std::list<EnemyBullet*>& GetBullets() const { return bullets_; }
	
	// 半径を取得
	float GetRadius() override { return radius; }

	bool IsDead() const { return isDead; }

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	static KamataEngine::Model* model_;
	KamataEngine::Model* bulletModel_ = nullptr;

	// フェーズ
	Phase phase_ = Phase::Approach;

	// 接近フェーズの速度
	static inline const KamataEngine::Vector3 kApproachSpeed = {0.0f, 0.0f, -0.2f};
	// 離脱フェーズの速度
	static inline const KamataEngine::Vector3 kLeaveSpeed = {-0.2f, 0.2f, -0.2f};

	// メンバ関数ポインタ
	static void (Enemy::*spFuncPhaseTable[])();

	// 弾
	std::list<EnemyBullet*> bullets_;

	// 発射間隔
	static const int kFireInterval = 200;

	// 発射タイマー
	int32_t fireTimer = 0;

	// 自キャラ
	Player* player_ = nullptr;

	// 半径
	float radius = 0.5f;

	bool isDead = false;

	int hp = 20;
};
