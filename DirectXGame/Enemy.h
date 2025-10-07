#pragma once
#include <KamataEngine.h>
#include <numbers>
#include "Aabb.h"

using namespace KamataEngine;
using namespace std;
using namespace numbers;

class Player;

class WorldUpdate;

class Enemy {
public:
	// 初期化
	void Initialize(Model* model, Camera* camera, const Vector3& position);

	// 更新
	void Update();

	// 描画
	void Draw();

	// ワールド座標を取得
	Vector3 GetWorldPosition();

	// AABBを取得
	AABB GetAABB();

	// 衝突応答
	void OnCollision(const Player* player);

private:
	// ワールド変換データ
	WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;

	// カメラ
	Camera* camera_ = nullptr;

	WorldUpdate* worldTransformUpdate_ = nullptr;

	// 歩行の速さ
	static inline const float kWalkSpeed = 0.02f;
	// 速度
	Vector3 velocity_ = {};

	// 最初の角度[度]
	static inline const float kWalkMotionAngleStart = 0.0f;
	// 最後の角度
	static inline const float kWalkMotionAngleEnd = 30.0f;
	// アニメーションの周期となる時間[秒]
	static inline const float kWalkMotionTime = 1.0f;

	// 経過時間
	float walkTimer_ = 0.0f;

	// キャラクターの当たり判定のサイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
};
