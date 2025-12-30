#pragma once
#include <KamataEngine.h>

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
	KamataEngine::Vector3 velocity_ = {0.0f, 0.0f, -0.1f};
	// 角度
	KamataEngine::Vector3 radian_ = {0.0f};
};
