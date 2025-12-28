#pragma once
#include <KamataEngine.h>

/// <summary>
/// 衝突判定オブジェクト
/// </summary>
class Collider {
public:
	// 半径を取得
	virtual float GetRadius() = 0;
	// 半径を設定
	void SetRadius(float radius) { radius_ = radius; }

	// 衝突時に呼ばれる関数
	virtual void OnCollision() = 0;

	// ワールド座標を取得
	virtual KamataEngine::Vector3 GetWorldPosition() = 0;

private:
	// 衝突半径
	float radius_ = 0.0f;
};
