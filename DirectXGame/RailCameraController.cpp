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
	Vector3 forward = {};
	for (int i = 0; i < segmentCount; i++) {
		Vector3 eye = pointsDrawing[i];
		Vector3 target = CatmullRomPosition(controlPoints_, t + 0.01f);
		// 敵弾から自キャラへのベクトルを計算
		forward = target - eye;

		const float speed = 1.0f;

		// ベクトルを正規化する
		Normalize(forward);
		Normalize(velocity_);
		// 球面線形補間
		velocity_ = Slerp(velocity_, forward, t) * speed;
	}

	// Y軸回り角度(θy)
	worldTransform_.rotation_.y = atan2(velocity_.x, velocity_.z);
	// 横軸方向の長さを求める
	float velocityXZ = Length(Vector3{velocity_.x, 0.0f, velocity_.z});
	// X回り角度(θx)a
	worldTransform_.rotation_.x = atan2(-velocity_.y, velocityXZ);
	worldTransform_.translation_ += velocity_;
	// スケール、回転、平行移動を合成して行列を計算する
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	// カメラオブジェクトのワールド行列からビュー行列を計算する
	camera_->matView = Inverse(worldTransform_.matWorld_);
}

void RailCameraController::Draw() {
	// 線分の数+1個分の頂点座標を計算
	for (size_t i = 0; i < segmentCount + 1; i++) {
		t = 1.0f / segmentCount * i;
		Vector3 pos = CatmullRomPosition(controlPoints_, t);
		// 描画用頂点リストに追加
		pointsDrawing.push_back(pos);
	}

	PrimitiveDrawer::GetInstance()->SetCamera(camera_);
	for (int i = 0; i < segmentCount; i++) {
		PrimitiveDrawer::GetInstance()->DrawLine3d(pointsDrawing[i], pointsDrawing[i + 1], Vector4{1.0f, 0.0f, 0.0f, 1.0f});
	}
}
