#include "GameScene.h"
#include "Math.h"

using namespace KamataEngine;
using namespace std;

GameScene::~GameScene() {
	// 3Dモデルデータの解放
	delete model_;
	delete player_;

	delete enemy_;

	// デバッグカメラの更新
	delete debugCamera_;
}

void GameScene::Initialize() {
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// テクスチャを読み込む
	textureHandle_ = TextureManager::Load("mariokart.jpg");
	// 3Dモデルデータの生成
	model_ = Model::Create();

	// カメラの初期化
	camera_.Initialize();
	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1980, 1080);

	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(model_, textureHandle_);

	// 敵の生成
	enemy_ = new Enemy();
	Vector3 enemyPosition = {5.0f, 4.0f, 4.0f};
	// 敵の初期化
	enemy_->Initialize(model_, enemyPosition);
	// 敵キャラに自キャラのアドレスを渡す
	enemy_->SetPlayer(player_);
}

void GameScene::Update() {
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_RETURN)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif
	// カメラの処理
	if (isDebugCameraActive_) {
		// デバッグカメラの更新
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		// ビュープロジェクション行列の転送
		camera_.TransferMatrix();
	} else {
		// ビュープロジェクション行列の更新と転送
		camera_.UpdateMatrix();
	}

	// プレイヤーの更新
	player_->Update();

	// 敵の更新
	enemy_->Update();

	// 衝突判定と応答
	CheckAllCollisions();
}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxCommon->GetCommandList());

	// プレイヤーの描画
	player_->Draw(camera_);

	// 敵の描画
	enemy_->Draw(camera_);

	Model::PostDraw();
}

void GameScene::CheckAllCollisions() {
	// 判定対象AとBの座標
	Vector3 posA, posB;

	// 自弾リストの取得
	const list<PlayerBullet*>& playerBullets = player_->GetBullets();
	// 敵弾リストの取得
	const list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

	#pragma region 自キャラと敵弾の当たり判定
	// 自キャラの座標
	posA = player_->GetWorldPosition();

	// 自キャラと敵弾全ての当たり判定
	for (EnemyBullet* bullet : enemyBullets) {
		// 敵弾の座標
		posB = bullet->GetWorldPosition();

		// 座標Aと座標Bの距離を求める
		float playerToEnemyBullet = Length(posB - posA);
		// 球と球の交差判定
		if (playerToEnemyBullet <= player_->GetRadius() + bullet->GetRadius()) {
			// 自キャラの衝突時コールバックを呼び出す
			player_->OnCollision();
			// 敵弾の衝突時コールバックを呼び出す
			bullet->OnCollision();
		}
	}
	#pragma endregion

	#pragma region 自弾と敵キャラの当たり判定
	// 敵キャラの座標
	posA = enemy_->GetWorldPosition();

	// 自キャラと敵弾全ての当たり判定
	for (PlayerBullet* bullet : playerBullets) {
		// 敵弾の座標
		posB = bullet->GetWorldPosition();

		// 座標Aと座標Bの距離を求める
		float playerBulletToEnemy = Length(posA - posB);
		// 球と球の交差判定
		if (playerBulletToEnemy <= bullet->GetRadius() + enemy_->GetRadius()) {
			// 自弾の衝突時コールバックを呼び出す
			bullet->OnCollision();
			// 敵の衝突時コールバックを呼び出す
			enemy_->OnCollision();
		}
	}
	#pragma endregion

	#pragma region 自弾と敵弾の当たり判定
	// 自キャラと敵弾全ての当たり判定
	for (PlayerBullet* playerBullet : playerBullets) {
		// 自弾の座標
		posA = playerBullet->GetWorldPosition();

		for (EnemyBullet* enemyBullet : enemyBullets) {
			// 敵弾の座標
			posB = enemyBullet->GetWorldPosition();
			// 座標Aと座標Bの距離を求める
			float playerBulletToEnemyBullet = Length(posB - posA);
			// 球と球の交差判定
			if (playerBulletToEnemyBullet <= playerBullet->GetRadius() + enemyBullet->GetRadius()) {
				// 自弾の衝突時コールバックを呼び出す
				playerBullet->OnCollision();
				// 敵の衝突時コールバックを呼び出す
				enemyBullet->OnCollision();
			}
		}
	}
	#pragma endregion
}
