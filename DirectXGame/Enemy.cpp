#include "Enemy.h"
#include "Math.h"

using namespace KamataEngine;

Model* Enemy::model_ = nullptr;

Enemy::~Enemy() {
	// bullet_の解放
	for (EnemyBullet* bullet : bullets_) {
		delete bullet;
	}
}

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

	// 弾を発射
	//Fire();

	// 接近フェーズ初期化
	ApproachInitialize();
}

// staticで宣言したメンバ関数ポインタテーブルの実体
void (Enemy::*Enemy::spFuncPhaseTable[])() = {
    &Enemy::Approach, // 要素番号0
    &Enemy::Leave     // 要素番号1
};

void Enemy::Update() {
	// デスフラグの立った弾を削除
	bullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	// メンバ関数ポインタに入っている関数を呼び出す
	(this->*spFuncPhaseTable[static_cast<size_t>(phase_)])();

	// 弾更新
	for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
	}

	// ワールドトランスフォーム更新
	math_->WorldTransformUpdate(worldTransform_);

	// キャラクターの座標を画面表示する処理
	ImGui::Begin(" ");
	ImGui::DragFloat3("Enemy", &worldTransform_.translation_.x, 0.01f);
	ImGui::End();
}

void Enemy::ApproachInitialize() {
	// 発射タイマーを初期化
	fireTimer = kFireInterval;
}

void Enemy::Approach() {
	// 移動（ベクトルを加算）
	worldTransform_.translation_ += kApproachSpeed;
	// 特定の位置に到達したら離脱
	/*if (worldTransform_.translation_.z < 0.0f) {
	    isDead_ = true;
	}*/
	// 発射タイマーカウントダウン
	fireTimer--;
	// 指定時間に達した
	if (fireTimer <= 0) {
		// 弾を発射
		Fire();
		// 発射タイマーを初期化
		fireTimer = kFireInterval;
	}
}

void Enemy::Leave() {
	// 移動（ベクトルを加算）
	worldTransform_.translation_ += kLeaveSpeed;
}

void Enemy::Fire() {
	// 自キャラの座標をコピー
	Vector3 position = worldTransform_.translation_;

	// 弾の速度
	const float kBulletSpeed = -1.0f;
	Vector3 velocity(0, 0, kBulletSpeed);

	// 速度ベクトルを自機の向きに合わせて回転させる
	velocity = math_->TransformNormal(velocity, worldTransform_.matWorld_);

	// 弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(model_, position, velocity);

	// 弾を登録する
	bullets_.push_back(newBullet);
}

void Enemy::Draw(const Camera& camera) {
	// モデルの描画
	model_->Draw(worldTransform_, camera, textureHandle_);

	// 弾描画
	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw(camera);
	}
}