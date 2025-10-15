#pragma once
#include <KamataEngine.h>
#include "Player.h"
#include <vector>
#include "Skydome.h"
#include "MapChipField.h"
#include "CameraController.h"
#include "Enemy.h"
#include "DeathParticles.h"
#include "Fade.h"
#include "HitEffect.h"

class Math;
class Aabb;

class GameScene {
public:
	// ゲームのフェーズ（型）
	enum class Phase {
		kFadeIn,  // フェードイン
		kPlay,    // ゲームプレイ
		kDeath,   // デス演出
		kFadeOut, // フェードアウト
	};

	~GameScene();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// ブロックの生成
	void GenerateBlocks();

	// 全ての当たり判定を行う
	void CheckAllCollisions();

	// フェーズの切り替え
	void ChangePhase();

	// カメラの更新
	void UpdateCamera();

	// ブロックの更新
	void UpdateBlocks();

	bool IsFinished() const { return finished_; }

	// エフェクトを生成
	void CreateHitEffect(const Vector3& position);

private:
	// ワールド変換データ
	WorldTransform worldTransform_;

	// カメラ
	Camera camera_;

	// 自キャラ
	Player* player_ = nullptr;
	// プレイヤーモデル
	Model* modelPlayer_ = nullptr;

	// 攻撃モデル
	Model* modelAttack_ = nullptr;

	// 3Dモデルデータ
	Model* modelBlock_ = nullptr;
	vector<vector<WorldTransform*>> worldTransformBlocks_;

	// 行列
	Math* matrix_ = nullptr;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;
	// デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;

	// 天球
	Skydome* skydome_ = nullptr;
	// 3Dモデル
	Model* modelSkydome_ = nullptr;

	// マップチップフィールド
	MapChipField* mapChipField_;

	// カメラコントローラ
	CameraController* cameraController_ = nullptr;

	// 敵
	list<Enemy*> enemies_;
	// 敵モデル
	Model* modelEnemy_ = nullptr;

	// デスパーティクル
	DeathParticles* deathParticles_ = nullptr;
	// デスパーティクルモデル
	Model* modelDeathParticle_ = nullptr;

	// ゲームの現在のフェーズ
	Phase phase_;

	// 終了フラグ
	bool finished_ = false;

	Fade* fade_ = nullptr;

	// ヒットエフェクトモデル
	Model* modelHitEffect_ = nullptr;
	// ヒットエフェクト
	list<HitEffect*> hitEffects_;
};
