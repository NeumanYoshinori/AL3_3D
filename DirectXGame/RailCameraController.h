#pragma once
#include <KamataEngine.h>
#include <vector>

class RailCameraController {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const KamataEngine::Vector3& position, const KamataEngine::Vector3& radian);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	// カメラを取得
	KamataEngine::Camera* GetCamera() { return camera_; }

	// ワールドトランスフォームを取得
	KamataEngine::WorldTransform& GetWorldTransform() { return worldTransform_; }

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// 速度
	KamataEngine::Vector3 velocity_ = {0.1f, 0.1f, 0.1f};
	// 角度
	KamataEngine::Vector3 radian_ = {0.0f};

	std::vector<KamataEngine::Vector3> controlPoints_ = {
	    {0,  0,  0},
        {10, 10, 0},
        {10, 15, 0},
        {20, 15, 0},
        {20, 0,  0},
        {30, 0,  0},
	};

	// 線分の数
	const size_t segmentCount = 100;

	// 線分で描画する用の頂点リスト
	std::vector<KamataEngine::Vector3> pointsDrawing;
	// 目標
	std::vector<KamataEngine::Vector3> frontPointsDrawing;

	float t = 0.0f;
};
