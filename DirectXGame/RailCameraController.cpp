#include "RailCameraController.h"
#include "Math.h"
#include <Imgui.h>

using namespace KamataEngine;
using namespace std;

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
	t += speed;
	t = min(t, 1.0f);

	Vector3 eye = CatmullRomPosition(controlPoints_, t);

	const float lookAhead = 0.01f;
	float nextT = min(t + lookAhead, 1.0f);
	Vector3 target = CatmullRomPosition(controlPoints_, nextT);

	// 敵弾から自キャラへのベクトルを計算
	Vector3 forward = target - eye;

	Normalize(forward);

	// Y軸回り角度(θy)
	worldTransform_.rotation_.y = atan2(forward.x, forward.z);
	// 横軸方向の長さを求める
	float velocityXZ = Length(Vector3{forward.x, 0.0f, forward.z});
	// X回り角度(θx)a
	worldTransform_.rotation_.x = atan2(-forward.y, velocityXZ);

	worldTransform_.translation_ = eye;

	// スケール、回転、平行移動を合成して行列を計算する
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	// カメラオブジェクトのワールド行列からビュー行列を計算する
	camera_->matView = Inverse(worldTransform_.matWorld_);
}
