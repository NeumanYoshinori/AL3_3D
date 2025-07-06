#define NOMINMAX

#include "Player.h"
#include <cassert>
#include "Math.h"
#include "worldTransform.h"
#include <numbers>
#include <algorithm>
#include "MapChipField.h"

worldTransform* worldTransformUpdate_ = new worldTransform;
Math* matrix2 = new Math;

void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {
	// NULLポインタチェック
	assert(model);
	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	// 引数の内容をメンバ変数に記録
	camera_ = camera;
}

void Player::Update() {
	Input();
	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.moveAmount_ = velocity_;

	// マップ衝突チェック
	MapCollision(collisionMapInfo);

	// 判定結果を反映して移動させる
	Move(collisionMapInfo);

	// 天井に接触している場合の処理
	CeilingHit(collisionMapInfo);

	bool landing = false;

	// 地面との当たり判定
	if (velocity_.y < 0) {
		if (worldTransform_.translation_.y <= 1.0f) {
			landing = true;
		}
	}

	// 接地判定
	if (onGround_) {
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			// 空中状態に移行
			onGround_ = false;
		}
	} else {
		// 着地
		if (landing) {
			// めり込み排斥
			worldTransform_.translation_.y = 1.0f;
			// 摩擦で横方向速度が減衰する
			velocity_.x *= (1.0f - kAttenuation);
			// 下方向速度をリセット
			velocity_.y = 0.0f;
			// 接地状態に移行
			onGround_ = true;
		}
	}

	// 旋回制御
	if (turnTimer_ > 0.0f) {
		// タイマーを進める
		turnTimer_ = std::max(turnTimer_ - (1.0f / 60.0f), 0.0f);
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		// 自キャラの角度を設定する
		worldTransform_.rotation_.y = matrix2->EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
	}

	worldTransformUpdate_->WorldTransformUpdate(worldTransform_);
}

void Player::Input() {
	// 移動入力
	// 接地状態
	if (onGround_) {
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
			// 左右加速
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				if (velocity_.x < 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}

				acceleration.x += kAcceleration;

				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				// 右移動中の左入力
				if (velocity_.x > 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}

				acceleration.x -= kAcceleration;

				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			}

			// 加速／減速
			velocity_.x += acceleration.x;
			velocity_.y += acceleration.y;
			velocity_.z += acceleration.z;
			// 最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

		} else {
			velocity_.x *= (1.0f - kAttenuation);
		}
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			// ジャンプ初速
			velocity_.x += 0;
			velocity_.y += kJumpAcceleration / 60.0f;
			velocity_.z += 0;
		}
		// 空中
	} else {
		// 落下速度
		velocity_.x += 0;
		velocity_.y += -kGravityAcceleration / 60.0f;
		velocity_.z += 0;
		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
}

void Player::MapCollision(CollisionMapInfo& info) {
	IsHitTop(info);
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, // kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, // kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, // kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}, // kLeftTop
	};

	return {center.x + offsetTable[static_cast<uint32_t>(corner)].x, center.y + offsetTable[static_cast<uint32_t>(corner)].y, center.z + offsetTable[static_cast<uint32_t>(corner)].z};
}

void Player::IsHitTop(CollisionMapInfo& info) {
	// 上昇あり？
	if (info.moveAmount_.y <= 0) {
		return;
	}

	// 移動後の四つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(
		    {worldTransform_.translation_.x + info.moveAmount_.x, worldTransform_.translation_.y + info.moveAmount_.y, worldTransform_.translation_.z + info.moveAmount_.z}, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 真上の当たり判定を行う
	bool hit = false;
	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.moveAmount_.y = (rect.bottom - worldTransform_.translation_.y) - kHeight / 2.0f + kBlank;
		info.moveAmount_.y = std::max(0.0f, info.moveAmount_.y);
		// 天井に当たったことを記録する
		info.hitCeiling_ = true;
	}
}

void Player::Move(const CollisionMapInfo& info) {
	worldTransform_.translation_.x += info.moveAmount_.x;
	worldTransform_.translation_.y += info.moveAmount_.y;
	worldTransform_.translation_.z += info.moveAmount_.z;
}

void Player::CeilingHit(const CollisionMapInfo& info) {
	// 天井に当たった？
	if (info.hitCeiling_) {
		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = 0;
	}
}

void Player::Draw() {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, *camera_);
}
