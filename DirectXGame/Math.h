#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class Math {
public:
	// 行列の積
	Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
	// アフィン変換行列の作成
	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rot, const Vector3& translate);
	float EaseInOut(float destinationY, float turnY, float timer);

private:
};
