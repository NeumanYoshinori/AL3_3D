#include "GameScene.h"

using namespace KamataEngine;

GameScene::~GameScene() {
	// 3Dモデルデータの解放
	delete model_;
	delete player_;

	// デバッグカメラの更新
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
	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1980, 1080);

	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(model_, textureHandle_, &camera_);
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
}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxCommon->GetCommandList());

	// プレイヤーの描画
	player_->Draw();

	Model::PostDraw();
}
