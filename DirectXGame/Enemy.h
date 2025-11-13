#pragma once
#include <KamataEngine.h>

class Math;

class Enemy;

class BaseEnemyState {
public:
	virtual void Update(Enemy* pEnemy) = 0;
};

class EnemyStateApproach : public BaseEnemyState {
public:
	void Update(Enemy* pEnemy);

private:
	// 接近フェーズの速度
	static inline const KamataEngine::Vector3 kApproachSpeed = {0.0f, 0.0f, -0.2f};
};

class EnemyStateLeave : public BaseEnemyState {
public:
	void Update(Enemy* pEnemy);

private:
	// 離脱フェーズの速度
	static inline const KamataEngine::Vector3 kLeaveSpeed = {-0.2f, 0.2f, -0.2f};
};

// 敵
class Enemy {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position);

	// 更新
	void Update();

	void ChangeState(BaseEnemyState* newState);

	void MoveEnemy(KamataEngine::Vector3 kSpeed);

	KamataEngine::Vector3 GetTranslation() const { return worldTransform_.translation_; }

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

	// ステート
	BaseEnemyState* state;
};
