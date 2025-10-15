#include "Enemy.h"
#include <cassert>
#include <numbers>
#include "Player.h"
#include "GameScene.h"

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
	// NULLポインタチェック
	assert(model);
	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = pi_v<float> * 3.0f / 2.0f;
	// カメラの初期化
	camera_ = camera;
	velocity_ = {-kWalkSpeed, 0, 0};
	// 経過時間
	walkTimer_ = 0.0f;
}

void Enemy::Update() {
	if (behaviorRequest_ != Behavior::kUnknown) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_;

		switch (behavior_) {
		case Behavior::kDeath:
			counter_ = 0;

			break;
		}

		// 振る舞いリクエストをリセット
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
		// 歩行
	case Behavior::kWalk:
		// 移動
		worldTransform_.translation_ += velocity_;

		// タイマーを加算
		walkTimer_ += 1.0f / 60.0f;

		// 回転アニメーション
		worldTransform_.rotation_.x = sin(2 * pi_v<float> * walkTimer_ / kWalkMotionTime);

		// ワールドトランスフォームの行列更新
		matrix_->WorldTransformUpdate(worldTransform_);

		break;
		// デス演出
	case Behavior::kDeath:
		// タイマーを加算
		counter_ += 1.0f / 60.0f;

		// Y軸回りの回転角をイージングで変化させる
		worldTransform_.rotation_.y += 0.3f;
		worldTransform_.rotation_.x = matrix_->EaseOut(matrix_->ToRadians(deathAngleStart), matrix_->ToRadians(deathAngleEnd), counter_ / kDeathTime);

		// ワールドトランスフォームの行列更新
		matrix_->WorldTransformUpdate(worldTransform_);

		if (counter_ >= kDeathTime) {
			isDead_ = true;
		}

		break;
	}

	// ワールド行列の更新
	matrix_->WorldTransformUpdate(worldTransform_);
}

Vector3 Enemy::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

AABB Enemy::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb{};

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Enemy::OnCollision(const Player* player) {
	if (behavior_ == Behavior::kDeath) {
		// 敵がやられているなら何もしない
		return;
	}

	// プレイヤーが攻撃中なら敵が死ぬ
	if (player->IsAttack()) {
		if (gameScene_) {
			Vector3 playerPos = player->GetWorldPosition();

			// 敵と自キャラの中間位置にエフェクトを生成
			Vector3 effectPos;
			effectPos.x = (GetWorldPosition() + playerPos).x / 2.0f;
			effectPos.y = (GetWorldPosition() + playerPos).y / 2.0f;
			effectPos.z = (GetWorldPosition() + playerPos).z / 2.0f;
			gameScene_->CreateHitEffect(effectPos);
		}

		// 敵の振るまいをデス演出に変更
		behaviorRequest_ = Behavior::kDeath;
		isCollisionDisabled_ = true;
	}
}

void Enemy::Draw() {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, *camera_);
}
