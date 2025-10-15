#include "GameScene.h"
#include "Math.h"

// エフェクトを生成
void GameScene::CreateHitEffect(const Vector3& position) {
	HitEffect* newHitEffect = HitEffect::Create(position);
	hitEffects_.push_back(newHitEffect);
}

GameScene::~GameScene() {
	// 3Dモデルデータの解放
	delete modelPlayer_;
	delete player_;
	delete modelBlock_;
	delete modelAttack_;

	for (vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

	// デバッグカメラの解放
	delete debugCamera_;

	// 天球の解放
	delete skydome_;
	delete modelSkydome_;

	// マップチップフィールドの解放
	delete mapChipField_;

	// カメラコントローラの解放
	delete cameraController_;

	// 敵の解放
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}

	// デスパーティクルの解放
	if (deathParticles_) {
		delete deathParticles_;
	}
	delete modelDeathParticle_;

	// フェードの解放
	delete fade_;

	// ヒットエフェクト用モデルの解放
	delete modelHitEffect_;

	// ヒットエフェクトの解放
	for (HitEffect* hitEffect : hitEffects_) {
		delete hitEffect;
	}
}

void GameScene::Initialize() {
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// カメラの初期化
	camera_.Initialize();

	// マップチップフィールドの生成
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
	// マップチップフィールドの初期化
	GenerateBlocks();

	// 3Dモデルデータの生成
	modelPlayer_ = Model::CreateFromOBJ("player");
	modelAttack_ = Model::CreateFromOBJ("attack_effect");
	// 自キャラの生成
	player_ = new Player();
	// 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(5, 16);
	// 自キャラの初期化
	player_->Initialize(modelPlayer_, modelAttack_, &camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);

	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 天球の生成
	modelSkydome_ = Model::CreateFromOBJ("skydome");
	// 天球の初期化
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);

	// カメラコントローラの初期化
	cameraController_ = new CameraController(); // 生成
	cameraController_->Initialize(&camera_); // 初期化
	cameraController_->SetTarget(player_); // 追従対象セット
	cameraController_->Reset(); // リセット

	// 移動可能エリア
	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	cameraController_->SetMovableArea(cameraArea);

	// 3Dモデルデータの生成
	modelEnemy_ = Model::CreateFromOBJ("enemy");
	for (uint32_t i = 0; i < 3; ++i) {
		// 敵の生成
		Enemy* newEnemy_ = new Enemy();
		// 座標をマップチップ番号で指定
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(14 + i * 4, 18);
		newEnemy_->Initialize(modelEnemy_, &camera_, enemyPosition);

		newEnemy_->SetGameScene(this);
		enemies_.push_back(newEnemy_);
	}

	modelHitEffect_ = new Model();
	// ヒットエフェクト用モデルの読み込み
	modelHitEffect_->CreateFromOBJ("particle");

	// ゲームプレイフェーズから開始
	phase_ = Phase::kFadeIn;

	// フェードの初期化
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);
	HitEffect::SetModel(modelHitEffect_);
	HitEffect::SetCamera(&camera_);
}

void GameScene::Update() {
	// デスフラグの立った敵を削除
	enemies_.remove_if([](Enemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});

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

		// 天球の更新
		skydome_->Update();

		// 自キャラの更新
		player_->Update();

		// 敵の更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		// カメラコントローラの更新
		cameraController_->Update();

		// カメラの更新
		UpdateCamera();

		// ブロックの更新
		UpdateBlocks();

		// ヒットエフェクトの更新
		for (HitEffect* hitEffect : hitEffects_) {
			hitEffect->Update();
		}

		break;
	case Phase::kPlay:
		// 天球の更新
		skydome_->Update();

		// 自キャラの更新
		player_->Update();

		// 敵の更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		// カメラコントローラの更新
		cameraController_->Update();

		// カメラの更新
		UpdateCamera();

		// ブロックの更新
		UpdateBlocks();

		// 全ての当たり判定を行う
		CheckAllCollisions();

		// ヒットエフェクトの更新
		for (HitEffect* hitEffect : hitEffects_) {
			hitEffect->Update();
		}

		break;
	case Phase::kDeath:
		// 天球の更新
		skydome_->Update();

		// 敵の更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		// デスパーティクルの更新
		if (deathParticles_) {
			deathParticles_->Update();
		}

		// フェードが終わったら次のフェーズに
		if (deathParticles_ && deathParticles_->IsFinished()) {
			phase_ = Phase::kFadeOut;
		}

		// カメラの更新
		UpdateCamera();

		// ブロックの更新
		UpdateBlocks();

		// ヒットエフェクトの更新
		for (HitEffect* hitEffect : hitEffects_) {
			hitEffect->Update();
		}

		break;
	case Phase::kFadeOut:
		// フェードの更新
		fade_->Update();

		if (fade_->IsFinished()) {
			finished_ = true;
		}

		// 天球の更新
		skydome_->Update();

		// 敵の更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		// カメラの更新
		UpdateCamera();

		// ブロックの更新
		UpdateBlocks();

		// ヒットエフェクトの更新
		for (HitEffect* hitEffect : hitEffects_) {
			hitEffect->Update();
		}

		break;
	}
}

void GameScene::Draw() {
	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dオブジェクト描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	if (!player_->IsDead()) {
		// 自キャラの描画
		player_->Draw();
	}

	// 天球描画
	skydome_->Draw();

	// ブロックの描画
	for (vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			modelBlock_->Draw(*worldTransformBlock, camera_);
		}
	}

	for (Enemy* enemy : enemies_) {
		// 敵の描画
		enemy->Draw();
	}

	// デスパーティクルの描画
	if (deathParticles_) {
		deathParticles_->Draw();
	}

	for (HitEffect* hitEffect : hitEffects_) {
		// ヒットエフェクトの描画
		hitEffect->Draw();
	}

	Model::PostDraw();

	// フェードの描画
	fade_->Draw();
}

void GameScene::GenerateBlocks() {
	// 3Dモデルデータの生成
	modelBlock_ = Model::CreateFromOBJ("block", true);

	// 要素数
	const uint32_t kNumBlockVirtical = mapChipField_->GetNumBlockVirtical();
	const uint32_t kNumBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 要素数を変更する
	// 列数を設定（縦方向のブロック数）
	worldTransformBlocks_.resize(kNumBlockHorizontal);
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		// 1列の要素数を設定（横方向のブロック数）
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	// ブロックの生成
	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

void GameScene::CheckAllCollisions() {
#pragma region playerAndEnemy
	// 判定対象1と2の座標
	AABB aabb1, aabb2;

	// 自キャラの座標
	aabb1 = player_->GetAABB();

	// 自キャラと敵全ての当たり判定
	for (Enemy* enemy : enemies_) {
		if (enemy->IsCollisionDisabled())
			continue;
		
		// 敵の座標
		aabb2 = enemy->GetAABB();

		// AABB同士の交差判定
		if (matrix_->IsCollision(aabb1, aabb2)) {
			// 自キャラの衝突時関数を呼び出す
			player_->OnCollision(enemy);
			// 敵の衝突時関数を呼び出す
			enemy->OnCollision(player_);
		}
	}

#pragma endregion
}

void GameScene::ChangePhase() {
	// フェーズの変更
	switch (phase_) {
	case Phase::kPlay:
		if (player_->IsDead()) {
			// 死亡演出フェーズに切り替え
			phase_ = Phase::kDeath;
			// 自キャラの座標を取得
			const Vector3& deathParticlesPosition = player_->GetWorldPosition();

			modelDeathParticle_ = Model::CreateFromOBJ("deathParticle", true);
			deathParticles_ = new DeathParticles;
			deathParticles_->Initialize(modelDeathParticle_, &camera_, deathParticlesPosition);
		}
		
		break;
	case Phase::kDeath:

		break;
	}
}

void GameScene::UpdateCamera() {
	// カメラの更新
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		camera_.TransferMatrix();
	} else {
		camera_.UpdateMatrix();
	}
}

void GameScene::UpdateBlocks() {
	// ブロックの更新
	for (vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform*& worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			matrix_->WorldTransformUpdate(*worldTransformBlock);
		}
	}
}
