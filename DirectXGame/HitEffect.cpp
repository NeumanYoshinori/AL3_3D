#include "HitEffect.h"
#include "Math.h"
#include <cassert>

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
	circleWorldTransform_.translation_ = position;
	circleWorldTransform_.Initialize();
	circleWorldTransform_.translation_.z = -1.0f;
	objectColor_.Initialize();
}

void HitEffect::Update() { matrix_->WorldTransformUpdate(circleWorldTransform_); }

void HitEffect::Draw() {
	assert(model_);
	assert(camera_);

	model_->Draw(circleWorldTransform_, *camera_, &objectColor_);
}
