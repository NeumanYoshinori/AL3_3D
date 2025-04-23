#include "GameScene.h"

using namespace KamataEngine;

GameScene::~GameScene() {
	delete sprite_;
	delete model_;
	delete debugCamera_;
}

void GameScene::Initialize() {
	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("genosekuto.jpg"); 
	// スプライトの生成
	sprite_ = Sprite::Create(textureHandle_, {100, 50});
	// 3Dモデルの生成
	model_ = Model::Create();
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// カメラの初期化
	camera_.Initialize();
	// サウンドデータの読み込み
	soundDataHandle_ = Audio::GetInstance()->LoadWave("fanfare.wav");
	// 音声再生
	Audio::GetInstance()->PlayWave(soundDataHandle_);
	// ライン描画が参照するカメラを指定する（アドレス渡し）
	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);
	debugCamera_ = new DebugCamera(1280, 720);
	// 軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定する（アドレス渡し）
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());
}

void GameScene::Update() {
	// スプライトの今の座標を取得
	Vector2 position = sprite_->GetPosition();
	// 座標を{ 2, 1 } 移動
	position.x += 2.0f;
	position.y += 1.0f;
	// 移動した座標をスプライトに反映
	sprite_->SetPosition(position);
	// スペースキーを押した瞬間
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
	// 音声停止
		Audio::GetInstance()->StopWave(voiceHandle_);
	}
	#ifdef _DEBUG
	// デバッグテキストの表示
	ImGui::Begin("Debug1");
	ImGui::Text("Kamata Tarou %d.%d.%d", 2050, 12, 31);
	ImGui::End();
	ImGui::Begin("Debug2");
	// float3入力ボックス
	ImGui::InputFloat3("InputFloat3", inputFloat3);
	// float3スライダー
	ImGui::SliderFloat3("SliderFloat3", inputFloat3, 0.0f, 1.0f);
	// デモウィンドウの表示を有効化
	ImGui::ShowDemoWindow();
	ImGui::End();
	#endif
	debugCamera_->Update();
}

void GameScene::Draw() {
	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	//スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());

	// スプライト描画
	sprite_->Draw();

	// スプライト後処理
	Sprite::PostDraw();

	// 3Dモデル前描画処理
	Model::PreDraw(dxCommon->GetCommandList());

	// 3Dモデル描画
	model_->Draw(worldTransform_, debugCamera_->GetCamera(), textureHandle_);

	// 3Dモデル描画後処理
	Model::PostDraw();

	// ラインを描画する
	for (int i = 0; i < 11; i++) {
		PrimitiveDrawer::GetInstance()->DrawLine3d({static_cast<float>(i), 0, 0}, {static_cast<float>(i), 10, 0}, {1.0f, 0.0f, 0.0f, 1.0f});
		PrimitiveDrawer::GetInstance()->DrawLine3d({0, static_cast<float>(i), 0}, {10, static_cast<float>(i), 0}, {0.0f, 0.0f, 255.0f, 1.0f});
	}
}
