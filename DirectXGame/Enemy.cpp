#include "Enemy.h"
#include "Math.h"

using namespace KamataEngine;

Model* Enemy::model_ = nullptr;

void Enemy::Initialize(Model* model, const Vector3& position) {
	// NULLポインタチェック
	assert(model);

	model_ = model;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("enemyPlane.jpg");

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;
}

void Enemy::Update() {
	worldTransform_.translation_.z -= 0.1f;
	math_->WorldTransformUpdate(worldTransform_);
}

void Enemy::Draw(const Camera& camera) {
	// モデルの描画
	model_->Draw(worldTransform_, camera, textureHandle_);
}