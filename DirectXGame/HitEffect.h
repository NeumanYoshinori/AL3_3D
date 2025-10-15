#pragma once
#include <KamataEngine.h>
#include <cstdint>

using namespace KamataEngine;
using namespace std;

class Math;

/// <summary>
/// ヒット演出用エフェクト
/// </summary>
class HitEffect {
public:
	enum class State {
		kSpread, // 拡大中
		kFade, // フェードアウト中
		kDead // 死亡
	};

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

	// 消滅しているか
	bool IsDead() const { return state_ == State::kDead; }

private:
	HitEffect() = default;

	// モデル（借りてくる用）
	static Model* model_;
	// カメラ（借りてくる用）
	static Camera* camera_;

	// 円のワールドトランスフォーム
	WorldTransform circleWorldTransform_;
	
	// 楕円の個数
	static inline const uint32_t kNumEllipse = 2;
	// 楕円の幅
	static inline const float ellipseWidth = 0.4f;
	// 楕円の高さ
	static inline const float ellipseHeight = 0.4f;

	// 楕円のワールドトランスフォーム
	array<WorldTransform, kNumEllipse> ellipseWorldTransforms_;

	ObjectColor objectColor_;

	State state_ = State::kSpread;

	// カウンター
	uint32_t counter_ = 0;

	// 拡大アニメーションの時間
	static inline const uint32_t kSpreadTime = 10;
	// フェードアウトアニメーションの時間
	static inline const uint32_t kFadeTime = 20;
	// エフェクトの寿命
	static inline const uint32_t kLifeTime = kSpreadTime + kFadeTime;

	// 行列
	Math* matrix_ = nullptr;
};
