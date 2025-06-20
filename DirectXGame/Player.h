#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

// 自キャラ
class Player {
public:
	// 左右
	enum class LRDirection {
		kRight,
		kLeft,
	};

	// 初期化
	void Initialize(Model* model, Camera* camera, const Vector3& position);

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

	Vector3 velocity_ = {};
	static inline const float kAcceleration = 0.05f;
	static inline const float kAttenuation = 0.05f;
	static inline const float kLimitRunSpeed = 0.5f;

	LRDirection lrDirection_ = LRDirection::kRight;
	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;
	// 旋回時間<秒>
	static inline const float kTimeTurn = 0.3f;

	// 接地状態フラグ
	bool onGround_ = true;

	// 重力加速度
	static inline const float kJumpAcceleration = 20.0f;
	// 最大落下速度
	static inline const float kGravityAcceleration = 0.98f;
	// ジャンプ初速
	static inline const float kLimitFallSpeed = 0.5f;
};
