#include "DeathParticles.h"
#include "worldTransform.h"
#include "Math.h"
#include <algorithm>

void DeathParticles::Initialize(Model* model, Camera* camera, const Vector3& position) {
	// NULLチェック
	assert(model);
	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	// 引数の内容をメンバ変数に記録
	camera_ = camera;

	// ワールド変換の初期化
	for (WorldTransform& worldTransform : worldTransforms_) {
		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}

	objectColor_.Initialize();
	color_ = {1, 1, 1, 1};
}

void DeathParticles::Update() {
	// 終了なら何もしない
	if (isFinished_) {
		return;
	}

	for (uint32_t i = 0; i < kNumParticles; ++i) {
		// 基本となる速度ベクトル
		Vector3 velocity = {kSpeed, 0, 0};
		// 回転角を計算する
		float angle = kAngleUnit * i;
		// Z軸回り回転行列
		Matrix4x4 matrixRotation = matrix_->MakeRotateZMatrix(angle);
		// 基本ベクトルを回転させて速度ベクトルを得る
		velocity = matrix_->Transform(velocity, matrixRotation);
		// 移動処理
		worldTransforms_[i].translation_ += velocity;
	}

	// カウンターを1フレーム分の秒数進める
	counter_ += 1.0f / 60.0f;

	// 存続時間の上限に達したら
	if (counter_ >= kDuration) {
		counter_ = kDuration;
		// 終了扱いにする
		isFinished_ = true;
	}

	color_.w = std::clamp(1.0f - counter_ / kDuration, 0.0f, 1.0f);
	// 色変更オブジェクトに色の数値を設定する
	objectColor_.SetColor(color_);

	// ワールド変換の更新
	for (auto& worldTransform : worldTransforms_) {
		worldTransformUpdate_->WorldTransformUpdate(worldTransform);
	}
}

void DeathParticles::Draw() {
	// 終了なら何もしない
	if (isFinished_) {
		return;
	}

	// モデルの描画
	for (auto& worldTransform : worldTransforms_) {
		model_->Draw(worldTransform, *camera_, &objectColor_);
	}
}
