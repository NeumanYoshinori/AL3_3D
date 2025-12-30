#include "RailCameraController.h"
#include "Math.h"
#include <Imgui.h>

using namespace KamataEngine;

void RailCameraController::Initialize(const Vector3& position, const Vector3& radian) {
	// ワールドトランスフォームの初期設定
	worldTransform_.translation_ = position;
	worldTransform_.rotation_ = radian;

	camera_ = new Camera();
	camera_->farZ = 1280.0f;
	// カメラの初期化
	camera_->Initialize();
}

void RailCameraController::Update() {
	worldTransform_.translation_ += velocity_;
	worldTransform_.rotation_ += radian_;
	// スケール、回転、平行移動を合成して行列を計算する
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	// カメラオブジェクトのワールド行列からビュー行列を計算する
	camera_->matView = Inverse(worldTransform_.matWorld_);

	// カメラの座標を画面表示する処理
	ImGui::Begin("Camera");
	ImGui::SliderFloat3("translation", &worldTransform_.translation_.x, 0.0f, 1.0f);
	ImGui::SliderFloat3("rotation", &worldTransform_.rotation_.x, 0.0f, 1.0f);
	ImGui::End();
}
