#include "TitleScene.h"
#include "worldTransform.h"
#include <numbers>

TitleScene::~TitleScene() {
	// 3Dモデルデータの解放
	delete modelPlayer_;
	delete modelTitle_;
}

void TitleScene::Initialize() {
	// 3Dモデルデータの生成
	modelTitle_ = Model::CreateFromOBJ("titleFont", true);
	modelPlayer_ = Model::CreateFromOBJ("player");

	// ワールドトランスフォームの初期化
	worldTransformTitle_.Initialize();

	// タイトルのスケール
	const float titleScale = 2.0f;
	worldTransformTitle_.scale_ = { titleScale, titleScale, titleScale };

	// ワールドトランスフォームの初期化
	worldTransformPlayer_.Initialize();

	// プレイヤーのスケール
	const float playerScale = 10.0f;
	worldTransformPlayer_.scale_ = { playerScale, playerScale, playerScale };

	// プレイヤーの角度
	worldTransformPlayer_.rotation_.y = std::numbers::pi_v<float> * 0.95f;

	// プレイヤーの位置
	worldTransformPlayer_.translation_.x = -2.0f;
	worldTransformPlayer_.translation_.y = -10.0f;

	// カメラの初期化
	camera_.Initialize();
}

void TitleScene::Update() {
	if (Input::GetInstance()->PushKey(DIK_SPACE)) {
		finished_ = true;
	}

	// カウンターを1フレーム分の秒数進める
	counter_ += 1.0f / 60.0f;

	// 角度
	float angle = std::sin(2.0f * std::numbers::pi_v<float> * counter_ / kTitleMoveTime);

	// タイトルを移動
	worldTransformTitle_.translation_.y = angle + 10.0f;

	camera_.TransferMatrix();

	// アフィン変換
	worldTransformUpdate_->WorldTransformUpdate(worldTransformTitle_);
	worldTransformUpdate_->WorldTransformUpdate(worldTransformPlayer_);
}

void TitleScene::Draw() {
	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dオブジェクト描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	// タイトルの描画
	modelTitle_->Draw(worldTransformTitle_, camera_);

	// 自キャラの描画
	modelPlayer_->Draw(worldTransformPlayer_, camera_);

	Model::PostDraw();
}
