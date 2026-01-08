#include "Math.h"
#include "EnemyBullet.h"
#include <cassert>
#include "Player.h"

using namespace KamataEngine;
using namespace std;

void EnemyBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {
	// NULLポインタチェック
	assert(model);

	model_ = model;

	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = velocity;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;
}

void EnemyBullet::Update() {
	// 敵弾から自キャラへのベクトルを計算
	Vector3 toPlayer = player_->GetWorldPosition() - GetWorldPosition();

	// ベクトルを正規化する
	Normalize(toPlayer);
	Normalize(velocity_);
	// 球面線形補間
	velocity_ = Slerp(velocity_, toPlayer, t) * kBulletSpeed;

	// Y軸回り角度(θy)
	worldTransform_.rotation_.y = atan2(velocity_.x, velocity_.z);
	// 横軸方向の長さを求める
	float velocityXZ = Length(Vector3{velocity_.x, 0.0f, velocity_.z});
	// X回り角度(θx)a
	worldTransform_.rotation_.x = atan2(-velocity_.y, velocityXZ);

	// 座標を移動させる（1フレーム分の移動量を足しこむ）
	worldTransform_.translation_ += velocity_;

	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	// ワールドトランスフォームの更新
	WorldTransformUpdate(worldTransform_);
}

void EnemyBullet::Draw(const Camera& camera) {
	// モデルの描画
	model_->Draw(worldTransform_, camera);
}

Vector3 EnemyBullet::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void EnemyBullet::OnCollision() {
	// デスフラグを立てる
	isDead_ = true;
}
