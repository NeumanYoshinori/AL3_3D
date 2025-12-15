#include "Skydome.h"
#include "Math.h"

using namespace KamataEngine;

void Skydome::Initialize(Model* model, Camera* camera) {
	// NULLポインタチェック
	assert(model);

	// モデル
	model_ = model;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	camera_ = camera;
}

void Skydome::Update() {
	// ワールドトランスフォーム更新
	WorldTransformUpdate(worldTransform_);
}

void Skydome::Draw() {
	// 3Dモデル描画
	model_->Draw(worldTransform_, *camera_);
}
