#pragma once
#include "Fade.h"
#include <KamataEngine.h>

using namespace KamataEngine;

class Math;

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene {
public:
	enum class Phase {
		kFadeIn,  // フェードアウト
		kMain,    // メイン部
		kFadeOut, // フェードアウト
	};

	~TitleScene();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// 終了フラグのgetter
	bool IsFinished() const { return finished_; }

private:
	// ワールド変換
	WorldTransform worldTransformTitle_;
	WorldTransform worldTransformPlayer_;

	// 行列
	Math* matrix_ = nullptr;

	// カメラ
	Camera camera_;

	// プレイヤーモデル
	Model* modelPlayer_ = nullptr;

	// タイトルのモデル
	Model* modelTitle_ = nullptr;

	// 経過時間カウント
	float counter_ = 0.0f;

	// アニメーションの同期となる時間
	static inline const float kTitleMoveTime = 2.0f;

	// 終了フラグ
	bool finished_ = false;

	Fade* fade_ = nullptr;

	// 現在のフェード
	Phase phase_ = Phase::kFadeIn;
};
