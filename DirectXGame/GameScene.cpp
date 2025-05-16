#include "GameScene.h"
#include "Math.h"

using namespace KamataEngine;

Math* matrix = new Math;

GameScene::~GameScene() {
	// 3Dモデルデータの解放
	delete model_;
	delete player_;
	delete modelBlock_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

	delete debugCamera_;
}

void GameScene::Initialize() {
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// テクスチャを読み込む
	textureHandle_ = TextureManager::Load("genosekuto.jpg");
	// 3Dモデルデータの生成
	model_ = Model::Create();
	// カメラの初期化
	camera_.Initialize();
	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(model_, textureHandle_, &camera_);
	// 3Dモデルデータの生成
	modelBlock_ = Model::Create();

	// 要素数
	const uint32_t kNumBlockVirtical = 10;
	const uint32_t kNumBlockHorizontal = 20;
	// ブロック1個分の横幅
	const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;

	// 要素数を変更する
	// 列数を設定（縦方向のブロック数）
	worldTransformBlocks_.resize(kNumBlockHorizontal);
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		// 1列の要素数を設定（横方向のブロック数）
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	// ブロックの生成
	for (uint32_t i = 0; i < kNumBlockVirtical; i += 2) {
		for (uint32_t j = 1; j < kNumBlockHorizontal; j += 2) {
			worldTransformBlocks_[i][j] = new WorldTransform();
			worldTransformBlocks_[i][j]->Initialize();
			worldTransformBlocks_[i][j]->translation_.x = kBlockWidth * j;
			worldTransformBlocks_[i][j]->translation_.y = kBlockHeight * i;
		}
	}

	for (uint32_t i = 1; i < kNumBlockVirtical; i += 2) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; j += 2) {
			worldTransformBlocks_[i][j] = new WorldTransform();
			worldTransformBlocks_[i][j]->Initialize();
			worldTransformBlocks_[i][j]->translation_.x = kBlockWidth * j;
			worldTransformBlocks_[i][j]->translation_.y = kBlockHeight * i;
		}
	}

	debugCamera_ = new DebugCamera(1280, 720);
}

void GameScene::Update() {
	player_->Update();

	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			worldTransformBlock->matWorld_ = matrix->MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

			// 定数バッファに転送する
			worldTransformBlock->TransferMatrix();
		}
	}

	debugCamera_->Update();

	#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_0)) {
		isDebugCameraActive_ = true;
	}

	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		camera_.TransferMatrix();
	} else {
		camera_.UpdateMatrix();
	}
	#endif
}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxCommon->GetCommandList());
	player_->Draw();
	Model::PostDraw();

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			Model::PreDraw(dxCommon->GetCommandList());
			modelBlock_->Draw(*worldTransformBlock, camera_);
			Model::PostDraw();
		}
	}
}
