#pragma once
#include <KamataEngine.h>
#include <numbers>
#include "Math.h"

using namespace KamataEngine;
using namespace std;
using namespace numbers;

class Player;
class WorldUpdate;
class Math;
class GameScene;

class Enemy {
public:
	enum class Behavior {
		kUnknown,
		kWalk,
		kDeath,
	};

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

	// デスフラグのgetter
	bool IsDead() const { return isDead_; }

	// コリジョンが無効か
	bool IsCollisionDisabled() const { return isCollisionDisabled_; }

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

private:
	// ワールド変換データ
	WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;

	// カメラ
	Camera* camera_ = nullptr;

	// ワールド変換行列
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

	// デスフラグ
	bool isDead_ = false;

	// 振る舞い
	Behavior behavior_ = Behavior::kWalk;
	// 振る舞いリクエスト
	Behavior behaviorRequest_ = Behavior::kUnknown;

	// カウンター
	float counter_ = 0.0f;

	// 行列
	Math* matrix_ = nullptr;

	// 旋回開始角度
	static inline const float deathAngleStart = 0.0f;
	// 旋回終了角度
	static inline const float deathAngleEnd = -60.0f;

	// 死亡時間
	static inline const float kDeathTime = 0.6f;

	// コリジョン無効フラグ
	bool isCollisionDisabled_ = false;
	
	// ゲームシーン
	GameScene* gameScene_ = nullptr;
};
