#include "Enemy.h"
#include "Math.h"
#include "worldTransform.h"
#include <cassert>
#include <numbers>

worldTransform* worldTransformUpdateE_ = new worldTransform;
Math* matrix3 = new Math;

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
	// NULLポインタチェック
	assert(model);
	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
	// カメラの初期化
	camera_ = camera;
	velocity_ = {-kWalkSpeed, 0, 0};
	// 経過時間
	walkTimer_ = 0.0f;
}

void Enemy::Update() {
	// 移動
	worldTransform_.translation_ += velocity_;

	// タイマーを加算
	walkTimer_ += 1.0f / 60.0f;

	// 回転アニメーション
	float param = std::sin(2 * std::numbers::pi_v<float> * walkTimer_ / kWalkMotionTime);
	float degree = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;

	worldTransform_.rotation_.x = matrix3->ToRadians(degree);

	// ワールド行列の更新
	worldTransformUpdateE_->WorldTransformUpdate(worldTransform_);
}

void Enemy::Draw() {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, *camera_);
}
