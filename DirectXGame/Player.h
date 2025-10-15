#pragma once
#include "KamataEngine.h"
#include <numbers>
#include "Math.h"

using namespace KamataEngine;
using namespace std;
using namespace numbers;

class MapChipField;
class Enemy;
class Math;

// 自キャラ
class Player {
public:
	// 左右
	enum class LRDirection {
		kRight,
		kLeft,
	};

	struct CollisionMapInfo {
		bool hitCeiling = false;
		bool hitGround = false;
		bool hitWall = false;
		Vector3 move;
	};

	enum Corner {
		kRightBottom, // 右下
		kLeftBottom, // 左下
		kRightTop, // 右上
		kLeftTop, // 左上

		kNumCorner // 要素数
	};

	enum class Behavior {
		kUnknown, // なし
		kRoot, // 通常状態
		kAttack // 攻撃中
	};

	enum class AttackPhase {
		kCharge, // 溜め
		kDash, // 突進
		kRecovery, // 余韻
	};

	// 初期化
	void Initialize(Model* model, Model* modelAttack, Camera* camera, const Vector3& position);

	// 更新
	void Update();

	// 通常行動初期化
	void BehaviorRootInitialize();
	// 攻撃行動初期化
	void BehaviorAttackInitialize();

	// 通常行動更新
	void BehaviorRootUpdate();
	// 攻撃行動更新
	void BehaviorAttackUpdate();

	// 移動入力
	void Input();

	// マップ衝突判定
	void MapCollision(CollisionMapInfo& info);

	array<Vector3, kNumCorner>PositionsNew(array<Vector3, kNumCorner>, const CollisionMapInfo& info);

	// 上方向
	void IsHitTop(CollisionMapInfo& info);
	// 下方向
	void IsHitBottom(CollisionMapInfo& info);
	//// 右方向
	void IsHitRight(CollisionMapInfo& info);
	//// 左方向
	void IsHitLeft(CollisionMapInfo& info);

	Vector3 CornerPosition(const Vector3& center, Corner corner);

	// 描画
	void Draw();

	// ワールドトランスフォームのgetter
	const WorldTransform& GetWorldTransform() const { return worldTransform_; }

	// 速度のgetter
	const Vector3& GetVelocity() const { return velocity_; }

	// マップチップフィールドのsetter
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// 天井に接触している場合の処理
	void CeilingHit(const CollisionMapInfo& info);

	// 接地している場合の処理
	void ChangeLanding(const CollisionMapInfo& info);

	// 壁に接触している場合の処理
	void WallHit(const CollisionMapInfo& info);

	// 旋回制御
	void Turn();

	// ワールド座標を取得
	Vector3 GetWorldPosition()const;

	// AABBを取得
	AABB GetAABB();

	// 衝突応答
	void OnCollision(const Enemy* enemy);

	// デスフラグのgetter
	bool IsDead() const { return isDead_; }

	// 攻撃フラグ
	bool IsAttack() const { return behavior_ == Behavior::kAttack && attackPhase_ == AttackPhase::kDash; };

	bool IsCollisionDisabled() const { return isCollisionDisabled_; }

private:
	// ワールド変換データ
	WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;

	// 攻撃モデル
	Model* modelAttack_ = nullptr;
	WorldTransform worldTransformAttack_;

	// カメラ
	Camera* camera_ = nullptr;

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	// 行列
	Math* matrix_ = nullptr;

	// 移動量
	Vector3 velocity_ = {};
	// フレームごとの加速度
	static inline const float kAcceleration = 0.1f;
	// 非入力時の摩擦係数
	static inline const float kAttenuation = 0.05f;
	// 最高速度
	static inline const float kLimitRunSpeed = 0.3f;

	// 顔の向き
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

	// キャラクターの当たり判定のサイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	static inline const float kBlank = 0.04f;

	// 着地時の速度減衰率
	static inline const float kAttenuationLanding = 0.0f;

	// 微小な数値
	static inline const float smallNum = 0.06f;

	// 壁に接触時の速度減衰率
	static inline const float kAttenuationWall = 0.2f;

	// デスフラグ
	bool isDead_ = false;

	// 振る舞い
	Behavior behavior_ = Behavior::kRoot;
	// 振る舞いリクエスト
	Behavior behaviorRequest_ = Behavior::kUnknown;

	// 攻撃ギミックの経過時間カウンター
	uint32_t attackParameter_ = 0;

	// 溜め動作時間
	static inline const uint32_t chargeTime = 8;
	// 攻撃時間
	static inline const uint32_t attackTime = 5;
	// 余韻時間
	static inline const uint32_t recoveryTime = 12;

	// 現在の攻撃フェーズ
	AttackPhase attackPhase_;

	// 衝突無効化
	bool isCollisionDisabled_ = false;
};
