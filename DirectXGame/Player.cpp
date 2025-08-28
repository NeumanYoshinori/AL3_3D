#define NOMINMAX

#include "Player.h"
#include <cassert>
#include "Math.h"
#include "worldTransform.h"
#include <numbers>
#include <algorithm>
#include "MapChipField.h"

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
	CollisionMapInfo collisionMapInfo{};
	// 移動量に速度の値をコピー
	collisionMapInfo.move = velocity_;
	collisionMapInfo.hitGround = false;
	collisionMapInfo.hitWall = false;

	// マップ衝突チェック
	MapCollision(collisionMapInfo);

	worldTransform_.translation_ += collisionMapInfo.move;

	// 天井に接触している場合の処理

	if (collisionMapInfo.hitCeiling) {
		velocity_.y = 0.0f;
	}
	
	WallHit(collisionMapInfo);

	ChangeLanding(collisionMapInfo);

	DashLeft();
	DashRight();

	// 旋回制御
	if (turnTimer_ > 0.0f) {
		// タイマーを進める
		turnTimer_ = std::max(turnTimer_ - (1.0f / 60.0f), 0.0f);
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		// 自キャラの角度を設定する
		worldTransform_.rotation_.y = matrix_->EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
	}

	worldTransformUpdate_->WorldTransformUpdate(worldTransform_);
}

void Player::Input() {
	// 移動入力
	// 接地状態
	if (onGround_) {
		if (!hitDashLeft || !hitDashRight) {
			if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
				// 左右加速
				Vector3 acceleration{};
				if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
					if (velocity_.x < 0.0f) {
						// 速度と逆方向に入力中は急ブレーキ
						velocity_.x *= (1.0f - kAttenuation);
					}

					acceleration.x += kAcceleration / 60.0f;

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

					acceleration.x -= kAcceleration / 60.0f;

					if (lrDirection_ != LRDirection::kLeft) {
						lrDirection_ = LRDirection::kLeft;
						turnFirstRotationY_ = worldTransform_.rotation_.y;
						turnTimer_ = kTimeTurn;
					}
				}

				// 加速／減速
				velocity_ += acceleration;
				// 最大速度制限
				velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

			} else {
				velocity_.x *= (1.0f - kAttenuation);
			}
		}

		// ほぼ0の場合に0にする
		if (std::abs(velocity_.x) <= 0.0001f) {
			velocity_.x = 0.0f;
		}
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			// ジャンプ初速
			velocity_.y = kJumpAcceleration / 60.0f;
		}
		// 空中
	} else {
		// 落下速度
		velocity_ += Vector3(0, -kGravityAcceleration / 60.0f, 0);
		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
}

void Player::MapCollision(CollisionMapInfo& info) {
	IsHitTop(info);
	IsHitBottom(info);
	IsHitRight(info);
	IsHitLeft(info);
}

std::array<Vector3, 4> Player::PositionsNew(std::array<Vector3, kNumCorner> positionsNew, const CollisionMapInfo& info) {
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	return positionsNew;
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, // kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, // kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, // kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}, // kLeftTop
	};

	return {center + offsetTable[static_cast<uint32_t>(corner)]};
}

void Player::IsHitTop(CollisionMapInfo& info) {
	// 上昇あり？
	if (info.move.y <= 0) {
		return;
	}

	// 移動後の四つの角の座標
	std::array<Vector3, kNumCorner> positionsNew{};

	positionsNew = PositionsNew(positionsNew, info);

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真上の当たり判定を行う
	bool hit = false;
	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, kHeight / 2.0f, 0));
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, kHeight / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// めり込み先ブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
			// 天井に当たったことを記録する
			info.hitCeiling = true;
		}
	}

	hitDashLeft = false;

	if (mapChipType == MapChipType::kDashBoardL) {
		hitDashLeft = true;
	}

	hitDashRight = false;

	if (mapChipType == MapChipType::kDashBoardR) {
		hitDashRight = true;
	}
}

void Player::IsHitBottom(CollisionMapInfo& info) {
	if (info.move.y >= 0) {
		return;
	}

	// 移動後の四つの角の座標
	std::array<Vector3, kNumCorner> positionsNew{};

	positionsNew = PositionsNew(positionsNew, info);

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真下の当たり判定を行う
	bool hit = false;
	// 左下点の当たり判定を行う
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, -kHeight / 2.0f, 0));
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, -kHeight / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// めり込み先ブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + (kHeight / 2.0f + kBlank));
			// 天井に当たったことを記録する
			info.hitGround = true;
		}
	}

	hitDashLeft = false;

	if (mapChipType == MapChipType::kDashBoardL) {
		hitDashLeft = true;
	}

	hitDashRight = false;

	if (mapChipType == MapChipType::kDashBoardR) {
		hitDashRight = true;
	}
}

void Player::IsHitRight(CollisionMapInfo& info) {
	// 右移動あり？
	if (info.move.x <= 0) {
		return;
	}

	// 移動後の四つの角の座標
	std::array<Vector3, kNumCorner> positionsNew{};

	positionsNew = PositionsNew(positionsNew, info);

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真下の当たり判定を行う
	bool hit = false;
	// 右上点の当たり判定を行う
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(kWidth / 2.0f, 0, 0));
		// 現在座標が壁の外化判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(kWidth / 2.0f, 0, 0));
		if (indexSetNow.xIndex != indexSet.xIndex) {
			// めり込み先ブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
			// 壁に当たったことを判定結果に記録する
			info.hitWall = true;
		}
	}

	hitDashLeft = false;

	if (mapChipType == MapChipType::kDashBoardL) {
		hitDashLeft = true;
	}

	hitDashRight = false;

	if (mapChipType == MapChipType::kDashBoardR) {
		hitDashRight = true;
	}
}

void Player::IsHitLeft(CollisionMapInfo& info) {
	// 左移動あり？
	if (info.move.x >= 0) {
		return;
	}

	// 移動後の四つの角の座標
	std::array<Vector3, kNumCorner> positionsNew{};

	positionsNew = PositionsNew(positionsNew, info);

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真下の当たり判定を行う
	bool hit = false;
	// 右上点の当たり判定を行う
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(-kWidth / 2.0f, 0, 0));
		// 現在座標が壁の外化判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(-kWidth / 2.0f, 0, 0));
		if (indexSetNow.xIndex != indexSet.xIndex) {
			// めり込み先ブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.x = std::min(0.0f, rect.right - worldTransform_.translation_.x + (kWidth / 2.0f + kBlank));
			// 壁に当たったことを判定結果に記録する
			info.hitWall = true;
		}
	}

	hitDashLeft = false;

	if (mapChipType == MapChipType::kDashBoardL) {
		hitDashLeft = true;
	}

	hitDashRight = false;

	if (mapChipType == MapChipType::kDashBoardR) {
		hitDashRight = true;
	}
}

void Player::ChangeLanding(const CollisionMapInfo& info) {
	// 接地判定
	if (onGround_) {
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			// 空中状態に移行
			onGround_ = false;
		} else {
			// 移動後の四つの角の座標
			std::array<Vector3, kNumCorner> positionsNew{};

			positionsNew = PositionsNew(positionsNew, info);

			MapChipType mapChipType;
			// 真下の当たり判定を行う
			bool hit = false;
			// 左下点の判定 
			MapChipField::IndexSet indexSet;
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom] + Vector3(0, -smallNum, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}
			// 右下点の判定
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom] + Vector3(0, -smallNum, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			// 落下開始
			if (!hit) {
				// 空中状態に切り替える
				onGround_ = false;
			}
		}
	} else {
		if (info.hitGround) {
			// 着地状態に切り替える（落下を止める）
			onGround_ = true;
			// 着地時にX速度を減速
			velocity_.x *= (1.0f - kAttenuationLanding);
			// Y速度をゼロにする
			velocity_.y = 0.0f;
		}
	}
}

void Player::WallHit(const CollisionMapInfo& info) {
	// 壁接触による減衰
	if (info.hitWall) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
}

void Player::DashLeft() {
	if (hitDashLeft) {
		Vector3 acceleration{};
		acceleration.x += kDashAcceleration / 60.0f;

		velocity_ -= acceleration;
		velocity_.x = std::clamp(velocity_.x, -kLimitDashSpeed, kLimitDashSpeed);
	}
}

void Player::DashRight() {
	if (hitDashRight) {
		Vector3 acceleration{};
		acceleration.x += kDashAcceleration / 60.0f;

		velocity_ += acceleration;
		velocity_.x = std::clamp(velocity_.x, -kLimitDashSpeed, kLimitDashSpeed);
	}
}

Vector3 Player::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos{};
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos = worldTransform_.translation_;

	return worldPos;
}

AABB Player::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb{};

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Player::OnCollision() {
	isDead_ = true;
}

void Player::Draw() {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, *camera_);
}
