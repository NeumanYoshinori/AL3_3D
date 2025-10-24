#pragma once
#include <KamataEngine.h>

const KamataEngine::Vector3 operator+(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2);

const KamataEngine::Vector3 operator*(const KamataEngine::Vector3& v1, const float f);

KamataEngine::Vector3& operator+=(KamataEngine::Vector3& lhv, const KamataEngine::Vector3& rhv);
KamataEngine::Vector3& operator-=(KamataEngine::Vector3& lhv, const KamataEngine::Vector3& rhv);
KamataEngine::Vector3& operator*=(KamataEngine::Vector3& v, float s);
KamataEngine::Vector3& operator/=(KamataEngine::Vector3& v, float s);

// 代入演算子オーバーロード
KamataEngine::Matrix4x4& operator*=(KamataEngine::Matrix4x4& lhm, const KamataEngine::Matrix4x4& rhm);

// 2項演算子オーバーロード
KamataEngine::Matrix4x4 operator*(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2);

KamataEngine::Vector3 operator+(const KamataEngine::Vector3& v);
KamataEngine::Vector3 operator-(const KamataEngine::Vector3& v);

class Math {
public:
	// 拡大縮小行列の作成
	KamataEngine::Matrix4x4 MakeScaleMatrix(const KamataEngine::Vector3& scale);
	// X軸回転行列の作成
	KamataEngine::Matrix4x4 MakeRotateXMatrix(float theta);
	// X軸回転行列の作成
	KamataEngine::Matrix4x4 MakeRotateYMatrix(float theta);
	// X軸回転行列の作成
	KamataEngine::Matrix4x4 MakeRotateZMatrix(float theta);
	// 平行移動行列の作成
	KamataEngine::Matrix4x4 MakeTranslateMatrix(const KamataEngine::Vector3& translate);
	// アフィン変換行列の作成
	KamataEngine::Matrix4x4 MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rot, const KamataEngine::Vector3& translate);
	// ワールド変換
	void WorldTransformUpdate(KamataEngine::WorldTransform& worldTransform);
};
