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

	// 天球の解放
	delete skydome_;
	delete modelSkydome_;

	// レールカメラコントローラーの解放
	delete railCamera_;
}

void GameScene::Initialize() {
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// テクスチャを読み込む
	textureHandle_ = TextureManager::Load("mariokart.jpg");
	// 3Dモデルデータの生成
	model_ = Model::Create();

	// カメラのfarZを適度に大きい値に変更する
	camera_.farZ = 560.0f;
	// カメラの初期化
	camera_.Initialize();
	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1980, 1080);

	// 3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	// 天球の生成
	skydome_ = new Skydome();
	// 天球の初期化
	skydome_->Initialize(modelSkydome_, &camera_);

	// レールカメラの生成
	railCamera_ = new RailCameraController();
	// レールカメラの初期化
	railCamera_->Initialize(railCameraPos, railCameraAngle);

	// レティクルのテクスチャ
	TextureManager::Load("lockon.png");

	// 自キャラの生成
	player_ = new Player();
	Vector3 playerPosition(0, 0, 40.0f);
	// 自キャラの初期化
	player_->Initialize(model_, textureHandle_, playerPosition, &camera_);
	// 自キャラとレールカメラの親子関係を結ぶ
	player_->SetParent(&railCamera_->GetWorldTransform());

	// 敵の生成
	enemy_ = new Enemy();
	Vector3 enemyPosition = {5.0f, 4.0f, 4.0f};
	// 敵の初期化
	enemy_->Initialize(model_, enemyPosition);
	// 敵キャラに自キャラのアドレスを渡す
	enemy_->SetPlayer(player_);

	// フェードの初期化
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);
}
	
void GameScene::Update() {
	// フェーズの変更
	ChangePhase();

	switch (phase_) {
	case Phase::kFadeIn:
		// フェードの更新
		fade_->Update();

		// フェードが終わったら次のフェーズに
		if (fade_->IsFinished()) {
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = Phase::kPlay;
		}

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

		// 天球の更新
		skydome_->Update();
		break;
	case Phase::kPlay:

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

		// 天球の更新
		skydome_->Update();

		// プレイヤーの更新
		player_->Update();

		// 敵の更新
		enemy_->Update();

		// 衝突判定と応答
		CheckAllCollisions();

		railCamera_->Update();

		break;

		case Phase::kFadeOut:
		// フェードの更新
		fade_->Update();

		if (fade_->IsFinished()) {
			finished_ = true;
		}

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

		// 天球の更新
		skydome_->Update();

		// プレイヤーの更新
		player_->Update();

		// 敵の更新
		enemy_->Update();

		// 衝突判定と応答
		CheckAllCollisions();

		break;	
	}
}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxCommon->GetCommandList());

	// 天球の描画
	skydome_->Draw();

	// プレイヤーの描画
	player_->Draw();

	// 敵の描画
	enemy_->Draw(camera_);

	// レールカメラの描画
	railCamera_->Draw();

	Model::PostDraw();

	Sprite::PreDraw(dxCommon->GetCommandList());

	// UIの描画
	player_->DrawUI();

	Sprite::PostDraw();
}

void GameScene::CheckAllCollisions() {
	// 自弾リストの取得
	const list<PlayerBullet*>& playerBullets = player_->GetBullets();
	// 敵弾リストの取得
	const list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

	#pragma region 自キャラと敵弾の当たり判定
	// 自キャラと敵弾全ての当たり判定
	for (EnemyBullet* bullet : enemyBullets) {
		CheckCollisionPair(player_, bullet);
	}
	#pragma endregion

	#pragma region 自弾と敵キャラの当たり判定
	// 自弾全てと敵キャラの当たり判定
	for (PlayerBullet* bullet : playerBullets) {
		CheckCollisionPair(bullet, enemy_);
	}
	#pragma endregion

	#pragma region 自弾と敵弾の当たり判定
	// 自キャラと敵弾全ての当たり判定
	for (PlayerBullet* playerBullet : playerBullets) {
		for (EnemyBullet* enemyBullet : enemyBullets) {
			CheckCollisionPair(playerBullet, enemyBullet);
		}
	}
	#pragma endregion
}

void GameScene::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {
	// コライダーAのワールド座標を取得
	Vector3 posA = colliderA->GetWorldPosition();
	// コライダーBのワールド座標を取得
	Vector3 posB = colliderB->GetWorldPosition();

	// 判定に必要な前計算
	float aToB = Length(posB - posA);

	// 球と球の交差判定
	if (aToB <= colliderA->GetRadius() + colliderB->GetRadius()) {
		// コライダーAの衝突時コールバックを呼び出す
		colliderA->OnCollision();
		// コライダーBの衝突時コールバックを呼び出す
		colliderB->OnCollision();
	}
}

void GameScene::ChangePhase() {
	// フェーズの変更
	switch (phase_) {
	case Phase::kPlay:
		if (player_->IsDead() || enemy_->IsDead()) {
			// 死亡演出フェーズに切り替え
			phase_ = Phase::kFadeOut;
		}

		break;
	case Phase::kFadeOut:
		break;
	}
}
