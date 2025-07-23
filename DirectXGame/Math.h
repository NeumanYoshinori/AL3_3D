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
	// 線形補間
	Vector3 Lerp(const Vector3& a, const Vector3& b, float t);
	// 内積
	float Dot(const Vector3& v1, const Vector3& v2);
	// 度をラジアンに変換する
	float ToRadians(float degree);

private:
};
