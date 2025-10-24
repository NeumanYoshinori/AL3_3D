#pragma once
#include <KamataEngine.h>

class Math;

// 自キャラの弾
class PlayerBullet {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position);

	// 更新
	void Update();

	// 描画
	void Draw(const KamataEngine::Camera& camera);

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	static KamataEngine::Model* model_;

	// テクスチャハンドル
	uint32_t textureHandle_;
	
	// 数学関数
	Math* math_ = nullptr;
};
