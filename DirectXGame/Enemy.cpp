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

void Enemy::Update() {
	state->Update(this);

	// ワールドトランスフォーム更新
	math_->WorldTransformUpdate(worldTransform_);

	// キャラクターの座標を画面表示する処理
	ImGui::Begin(" ");
	ImGui::DragFloat3("Enemy", &worldTransform_.translation_.x, 0.01f);
	ImGui::End();
}

void Enemy::ChangeState(BaseEnemyState* newState) {
	delete state;
	state = newState;
}

void Enemy::MoveEnemy(Vector3 kSpeed) { worldTransform_.translation_ += kSpeed; }

void Enemy::Draw(const Camera& camera) {
	// モデルの描画
	model_->Draw(worldTransform_, camera, textureHandle_);
}

void EnemyStateApproach::Update(Enemy* pEnemy) {
	// 移動（ベクトルを加算）
	pEnemy->MoveEnemy(kApproachSpeed);
	// 特定の位置に到達したら離脱
	if (pEnemy->GetTranslation().z < 0.0f) {
		pEnemy->ChangeState(new EnemyStateLeave());
	}
}
void EnemyStateLeave::Update(Enemy* pEnemy) {
	// 移動（ベクトルを加算）
	pEnemy->MoveEnemy(kLeaveSpeed);
	pEnemy->ChangeState(new EnemyStateApproach());
}
