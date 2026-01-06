#define NOMINMAX

#include "Player.h"
#include <cassert>
#include "Math.h"

using namespace KamataEngine;
using namespace std;

Player::~Player() {
	// bullet_の解放
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}

	delete sprite2DReticle_;
}

void Player::Initialize(Model* model, uint32_t textureHandle, const Vector3& position, Camera* camera) {
	// NULLポインタチェック
	assert(model);

	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	textureHandle_ = textureHandle;

	// 3Dレティクルのワールドトランスフォーム初期化
	worldTransform3DReticle_.Initialize();

	// レティクル用テクスチャ取得
	uint32_t textureReticle = TextureManager::Load("lockon.png");

	// スプライト生成
	sprite2DReticle_ = Sprite::Create(textureReticle, {0.0f}, Vector4{255, 255, 255, 255}, Vector2{0.5f, 0.5f});
	sprite2DReticle_->SetSize(Vector2(100.0f, 100.0f));

	// カメラ
	camera_ = camera;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;
}

void Player::Update() {
	// デスフラグの立った弾を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	// 旋回
	Rotate();

	// プレイヤー旋回処理
	// キャラクターの移動ベクトル
	Vector3 move = {0.0f, 0.0f, 0.0f};

	// キャラクターの移動速さ
	const float kCharacterSpeed = 0.2f;

	// 押した方向で移動ベクトルを変更（左右）
	if (input_->PushKey(DIK_LEFT)) {
		move.x -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {
		move.x += kCharacterSpeed;
	}

	// 押した方向で移動ベクトルを変更（上下）
	if (input_->PushKey(DIK_UP)) {
		move.y += kCharacterSpeed;
	} else if (input_->PushKey(DIK_DOWN)) {
		move.y -= kCharacterSpeed;
	}

	// 座標移動（ベクトルの加算）
	worldTransform_.translation_ += move;

	// 移動限界座標
	const float kMoveLimitX = 35.0f;
	const float kMoveLimitY = 19.4f;

	// 範囲を超えない処理
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);

	// マウス座標(スクリーン座標）を取得する
	GetCursorPos(&mousePosition);

	// クライアントエリア座標に変換する
	HWND hwnd = WinApp::GetInstance()->GetHwnd();
	ScreenToClient(hwnd, &mousePosition);

	// マウス座標を2Dレティクルのスプライトに代入する
	sprite2DReticle_->SetPosition({static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y)});

	// ビューポート行列
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	Matrix4x4 matVPV = camera_->matView * camera_->matProjection * matViewport;
	// 合成行列の逆行列を計算する
	Matrix4x4 matInverseVPV = Inverse(matVPV);

	// スクリーン座標
	posNear = Vector3(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y), 0);
	Vector3 posFar = Vector3(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y), 1);

	// スクリーン座標系からワールド座標系へ
	posNear = Transform(posNear, matInverseVPV);
	posFar = Transform(posFar, matInverseVPV);

	// マウスレイの方向
	mouseDirection = posFar - posNear;
	mouseDirection = Normalize(mouseDirection);
	// カメラから昇順オブジェクトの距離
	const float kDistanceTestObject = -80.0f;
	worldTransform3DReticle_.translation_ = posNear - mouseDirection * kDistanceTestObject;
	WorldTransformUpdate(worldTransform3DReticle_);

	// プレイヤー攻撃処理
	Attack();

	// 弾更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}

	// ワールド行列の更新
	WorldTransformUpdate(worldTransform_);

	// キャラクターの座標を画面表示する処理
	ImGui::Begin(" ");
	ImGui::DragFloat3("Player", &worldTransform_.translation_.x, 0.01f);
	ImGui::End();
}

void Player::Draw() {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, *camera_, textureHandle_);

	// 弾描画
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(*camera_);
	}
}

void Player::DrawUI() {
	sprite2DReticle_->Draw();
}

void Player::Rotate() {
	// 回転速さ[ラジアン/frame]
	const float kRotSpeed = 0.02f;

	// 押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	} else if (input_->PushKey(DIK_D)) {
		worldTransform_.rotation_.y += kRotSpeed;
	}
}

void Player::Attack() {
	if (input_->IsTriggerMouse(0)) {
		// 0〜1 に正規化
		float nx = static_cast<float>(mousePosition.x) / WinApp::kWindowWidth;
		float ny = static_cast<float>(mousePosition.y) / WinApp::kWindowHeight;

		// -1〜+1 に変換しつつ、Y は上が + になるよう反転
		float sx = nx * 2.0f - 1.0f;
		float sy = 1.0f - ny * 2.0f;

		// プレイヤーの移動制限と対応させてワールド座標にマッピング
		const float kMoveLimitX = 25.0f; // Player.cpp と同じ値
		const float kMoveLimitY = 15.0f; // Player.cpp と同じ値

		float worldX = sx * kMoveLimitX;
		float worldY = sy * kMoveLimitY;

		// 自キャラの座標をコピー
		Vector3 position = Vector3{worldX, worldY, -20.0f};

		Vector3 velocity = {0.0f, 0.0f, kBulletSpeed};

		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, position, velocity);

		// 弾を登録する
		bullets_.push_back(newBullet);
	}
}

Vector3 Player::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos = {};
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Player::OnCollision() {
	// 何もしない
	isDead_ = true;
}

void Player::SetParent(const WorldTransform* parent) {
	// 親子関係を結ぶ
	worldTransform_.parent_ = parent;
}
