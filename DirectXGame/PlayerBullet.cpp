#include "PlayerBullet.h"
#include <cassert>
#include "Math.h"

using namespace KamataEngine;

Model* PlayerBullet::model_ = nullptr;

void PlayerBullet::Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position) {
	// NULLポインタチェック
	assert(model);

	model_ = model;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("uvChecker.png");

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;
}

void PlayerBullet::Update() {
	// ワールドトランスフォームの更新
	math_->WorldTransformUpdate(worldTransform_);
}

void PlayerBullet::Draw(const KamataEngine::Camera& camera) {
	// モデルの描画
	model_->Draw(worldTransform_, camera, textureHandle_);
}
