#pragma once
#include <KamataEngine.h>
#include <cstdint>

using namespace KamataEngine;

class Math;

/// <summary>
/// ヒット演出用エフェクト
/// </summary>
class HitEffect {
public:
	// setter
	static void SetModel(Model* model) { model_ = model; }

	static void SetCamera(Camera* camera) { camera_ = camera; }

	// インスタンス生成と初期化
	static HitEffect* Create(const Vector3& position);

	// 初期化
	void Initialize(const Vector3& position);

	// 更新
	void Update();

	// 描画
	void Draw();

private:
	HitEffect() = default;

	// モデル（借りてくる用）
	static Model* model_;
	// カメラ（借りてくる用）
	static Camera* camera_;

	// 円のワールドトランスフォーム
	WorldTransform circleWorldTransform_;

	ObjectColor objectColor_;

	// 行列
	Math* matrix_ = nullptr;
};
