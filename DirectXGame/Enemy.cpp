#include "Enemy.h"
#include "Math.h"

using namespace KamataEngine;

Model* Enemy::model_ = nullptr;

void Enemy::Initialize(Model* model, const Vector3& position) {
	// NULLポインタチェック
	assert(model);

	model_ = model;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("enemyPlane.jpg");

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;
}

// staticで宣言したメンバ関数ポインタテーブルの実体
void (Enemy::*Enemy::spFuncPhaseTable[])() = {
    &Enemy::Approach, // 要素番号0
    &Enemy::Leave     // 要素番号1
};

void Enemy::Update() {
	// メンバ関数ポインタに入っている関数を呼び出す
	(this->*spFuncPhaseTable[static_cast<size_t>(phase_)])();

	// ワールドトランスフォーム更新
	math_->WorldTransformUpdate(worldTransform_);

	// キャラクターの座標を画面表示する処理
	ImGui::Begin(" ");
	ImGui::DragFloat3("Enemy", &worldTransform_.translation_.x, 0.01f);
	ImGui::End();
}

void Enemy::Approach() {
	// 移動（ベクトルを加算）
	worldTransform_.translation_ += kApproachSpeed;
	// 特定の位置に到達したら離脱
	if (worldTransform_.translation_.z < 0.0f) {
		phase_ = Phase::Leave;
	}
}

void Enemy::Leave() {
	// 移動（ベクトルを加算）
	worldTransform_.translation_ += kLeaveSpeed;
}

void Enemy::Draw(const Camera& camera) {
	// モデルの描画
	model_->Draw(worldTransform_, camera, textureHandle_);
}