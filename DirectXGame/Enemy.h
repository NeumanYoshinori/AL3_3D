#pragma once
#include <KamataEngine.h>

class Math;

// 敵
class Enemy {
public:
	// 行動フェーズ
	enum class Phase {
		Approach, // 接近する
		Leave, // 離脱する
	};

	// 初期化
	void Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position);

	// 更新
	void Update();

	// 接近
	void Approach();

	// 離脱
	void Leave();

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
};
