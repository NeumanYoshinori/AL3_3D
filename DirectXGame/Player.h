#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class MapChipField;

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
		Vector3 moveAmount_;
	};

	enum Corner {
		kRightBottom, // 右下
		kLeftBottom, // 左下
		kRightTop, // 右上
		kLeftTop, // 左上

		kNumCorner // 要素数
	};

	// 初期化
	void Initialize(Model* model, Camera* camera, const Vector3& position);

	// 更新
	void Update();

	// 移動入力
	void Input();

	// マップ衝突判定
	void MapCollision(CollisionMapInfo& info);

	std::array<Vector3, kNumCorner>PositionsNew(std::array<Vector3, kNumCorner>, const CollisionMapInfo& info);

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

	// 判定結果を反映して移動させる
	void Move(const CollisionMapInfo& info);

	// 天井に接触している場合の処理
	void CeilingHit(const CollisionMapInfo& info);

	// 接地している場合の処理
	void ChangeLanding(const CollisionMapInfo& info);

	void WallHit(const CollisionMapInfo& info);

private:
	// ワールド変換データ
	WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// カメラ
	Camera* camera_ = nullptr;

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	// 移動量
	Vector3 velocity_ = {};
	// フレームごとの加速度
	static inline const float kAcceleration = 0.01f;
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
	static inline const float kBlank = 0.2f;

	// 着地時の速度減衰率
	static inline const float kAttenuationLanding = 0.1f;

	// 微小な数値
	float smallNum = 0.1f;

	// 着地時の速度減衰率
	static inline const float kAttenuationWall = 0.4f;
};
