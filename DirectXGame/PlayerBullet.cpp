#include "PlayerBullet.h"
#include <cassert>
#include "Math.h"

using namespace KamataEngine;

Model* PlayerBullet::model_ = nullptr;

void PlayerBullet::Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position, const Vector3& velocity) {
	// NULLポインタチェック
	assert(model);

	model_ = model;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("uvChecker.png");

	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = velocity;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;
}

void PlayerBullet::Update() {
	// 座標を移動させる（1フレーム分の移動量を足しこむ）
	worldTransform_.translation_ += velocity_;

	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
	
	// ワールドトランスフォームの更新
	WorldTransformUpdate(worldTransform_);
}

void PlayerBullet::Draw(const KamataEngine::Camera& camera) {
	// モデルの描画
	model_->Draw(worldTransform_, camera, textureHandle_);
}

void PlayerBullet::OnCollision() {
	// デスフラグを立てる
	isDead_ = true;
}
