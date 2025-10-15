#include "HitEffect.h"
#include "Math.h"
#include <cassert>
#include <random>

// 静的メンバ変数の実体化
Model* HitEffect::model_ = nullptr;
Camera* HitEffect::camera_ = nullptr;

HitEffect* HitEffect::Create(const Vector3& position) {
	// インスタンス生成
	HitEffect* instance = new HitEffect();
	// new失敗を検出
	assert(instance);
	// インスタンスの初期化
	instance->Initialize(position);
	// 初期化し他インスタンスを返す
	return instance;
}

void HitEffect::Initialize(const Vector3& position) {
	// 乱数生成エンジン
	random_device seedGenerator;
	// メルセンヌ・ツイスターエンジン(64bit版)
	mt19937_64 randomEngine;
	// メルセンヌ・ツイスターエンジンの初期化
	randomEngine.seed(seedGenerator());
	// 指定範囲の乱数生成器（浮動小数点数用）
	uniform_real_distribution<float> rotationDistribution(-pi_v<float>, pi_v<float>);

	// 楕円エフェクト
	for (WorldTransform& slashWorldTransform : ellipseWorldTransforms_) {
		slashWorldTransform.rotation_ = {0.0f, 0.0f, rotationDistribution(randomEngine)};
		slashWorldTransform.translation_ = position;
		slashWorldTransform.translation_.z = -1.0f;
		slashWorldTransform.Initialize();
	}

	circleWorldTransform_.translation_ = position;
	circleWorldTransform_.Initialize();
	circleWorldTransform_.translation_.z = -1.0f;

	objectColor_.Initialize();
}

void HitEffect::Update() {
	switch (state_) {
	case State::kSpread: {
		++counter_;
		float scale = 0.5f + static_cast<float>(counter_) / kSpreadTime * 0.5f;
		const float slashScale = 2.0f;

		// 楕円エフェクト
		for (WorldTransform& slashWorldTransform : ellipseWorldTransforms_) {
			slashWorldTransform.scale_ = {0.1f, scale * slashScale, 1.0f};
		}

		const float circleScale = 1.0f;

		circleWorldTransform_.scale_ = {scale * circleScale, scale * circleScale, 1.0f};

		if (counter_ >= kSpreadTime) {
			state_ = State::kFade;
			counter_ = 0; // カウンターをリセット
		}
		break;
	}
	case State::kFade: {
		++counter_;
		objectColor_.SetColor(Vector4{1.0f, 1.0f, 1.0f, 1.0f - static_cast<float>(counter_) / kFadeTime});

		if (++counter_ >= kFadeTime) {
			state_ = State::kDead;
		}

		break;
	}
	default:
		break;
	}

	for (auto& slashWorldTransform : ellipseWorldTransforms_) {
		matrix_->WorldTransformUpdate(slashWorldTransform);
	}

	matrix_->WorldTransformUpdate(circleWorldTransform_);
}

void HitEffect::Draw() {
	assert(model_);
	assert(camera_);

	// すでに消滅している場合は描画しない
	if (IsDead()) {
		return;
	}

	// 楕円エフェクト
	for (auto& slashWorldTransform : ellipseWorldTransforms_) {
		model_->Draw(slashWorldTransform, *camera_, &objectColor_);
	}

	model_->Draw(circleWorldTransform_, *camera_, &objectColor_);
}
