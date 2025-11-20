#pragma once
#include <KamataEngine.h>
#include "EnemyBullet.h"

class Math;

// 敵
class Enemy {
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

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	static KamataEngine::Model* model_;

	// テクスチャハンドル
	uint32_t textureHandle_;

	// 数学関数
	Math* math_ = nullptr;

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
	static const int kFireInterval = 60;

	// 発射タイマー
	int32_t fireTimer = 0;
};
