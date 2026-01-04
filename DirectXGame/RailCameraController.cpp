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

void RailCameraController::Draw() {
	// 線分で描画する用の頂点リスト
	vector<Vector3> pointsDrawing;
	// 線分の数
	const size_t segmentCount = 100;
	// 線分の数+1個分の頂点座標を計算
	for (size_t i = 0; i < segmentCount + 1; i++) {
		float t = 1.0f / segmentCount * i;
		Vector3 pos = CatmullRomPosition(controlPoints_, t);
		// 描画用頂点リストに追加
		pointsDrawing.push_back(pos);
	}

	PrimitiveDrawer::GetInstance()->SetCamera(camera_);
	for (int i = 0; i < segmentCount; i++) {
		PrimitiveDrawer::GetInstance()->DrawLine3d(pointsDrawing[i], pointsDrawing[i + 1], Vector4{1.0f, 0.0f, 0.0f, 1.0f});
	}
}
