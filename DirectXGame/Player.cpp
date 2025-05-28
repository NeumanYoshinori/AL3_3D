#include "Player.h"
#include <cassert>
#include "Math.h"
#include "worldTransform.h"

worldTransform* worldTransformUpdate_ = new worldTransform;

void Player::Initialize(Model* model, uint32_t textureHandle, Camera* camera) {
	// NULLポインタチェック
	assert(model);
	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	textureHandle_ = textureHandle;
	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_.y = 3.0f;
	// 引数の内容をメンバ変数に記録
	camera_ = camera;
}

void Player::Update() {
	// 行列を定数バッファに転送
	worldTransformUpdate_->WorldTransformUpdate(worldTransform_);
}

void Player::Draw() {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, *camera_);
}
