#pragma once
#include <KamataEngine.h>
#include "PlayerBullet.h"
#include "Collider.h"

// 自キャラ
class Player : public Collider {
public:
	 // デストラクタ
	~Player();

	// 初期化
	void Initialize(KamataEngine::Model* model, uint32_t texturehandle, const KamataEngine::Vector3& position);

	// 更新
	void Update();

	// 描画
	void Draw(KamataEngine::Camera& camera);

	// 旋回
	void Rotate();

	// 攻撃
	void Attack();

	// 衝突時に呼ばれる関数
	KamataEngine::Vector3 GetWorldPosition() override;

	// 衝突時に呼ばれる関数をオーバーライド
	void OnCollision() override;

	// 弾リストを取得
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }

	// 半径を取得
	float GetRadius() override { return radius; }

	// 親となるワールドトランスフォームをセット
	void SetParent(const KamataEngine::WorldTransform* parent);

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

	// 半径
	float radius = 0.5f;
};
