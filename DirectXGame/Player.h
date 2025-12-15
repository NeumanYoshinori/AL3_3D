#pragma once
#include "KamataEngine.h"
#include "PlayerBullet.h"
#include "Math.h"

// 自キャラ
class Player {
public:
	 // デストラクタ
	~Player();

	// 初期化
	void Initialize(KamataEngine::Model* model, uint32_t texturehandle);

	// 更新
	void Update();

	// 描画
	void Draw(KamataEngine::Camera& camera);

	// 旋回
	void Rotate();

	// 攻撃
	void Attack();
	
	// ワールド座標を取得
	KamataEngine::Vector3 GetWorldPosition();

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision();

	// 弾リストを取得
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }

	// 半径を取得
	float GetRadius() const { return radius; }

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	// モデル
	KamataEngine::Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// シングルトンインスタンスを取得する
	KamataEngine::Input* input_ = KamataEngine::Input::GetInstance();

	// 弾
	std::list<PlayerBullet*> bullets_;

	float radius = 0.5f;
};
