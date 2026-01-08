#include "Enemy.h"
#include "Math.h"
#include <cassert>
#include "Player.h"

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
	WorldTransformUpdate(worldTransform_);
}

void Enemy::ApproachInitialize() {
	// 発射タイマーを初期化
	fireTimer = kFireInterval;
}

void Enemy::Approach() {
	// 移動（ベクトルを加算）
	//worldTransform_.translation_ += kApproachSpeed;
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
	assert(player_);

	// 弾の速さ
	const float kBulletSpeed = 2.0f;
	Vector3 velocity = {};
	
	// 自キャラのワールド座標を取得する
	Vector3 playerPos = player_->GetWorldPosition();
	// 敵キャラのワールド座標を取得する
	Vector3 enemyPos = GetWorldPosition();
	// 敵キャラから自キャラへの差分ベクトルを求める
	Vector3 e2p = playerPos - enemyPos;
	// ベクトルの正規化
	Normalize(e2p);
	// ベクトルの長さを、速さに合わせる
	velocity = e2p * kBulletSpeed;

	bulletModel_ = Model::CreateFromOBJ("spiderWeb", true);
	// 弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(bulletModel_, worldTransform_.translation_, velocity);
	newBullet->SetPlayer(player_);

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

Vector3 Enemy::GetWorldPosition() {
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Enemy::OnCollision() {
	// 何もしない
	isDead = true;
}