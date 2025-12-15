#pragma once
#include <KamataEngine.h>

// 内積
float Dot(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2);

// 長さ（ノルム)
float Length(const KamataEngine::Vector3& v);

// 線形補間（スカラー）
float Lerp(float a, float b, float t);

const KamataEngine::Vector3 operator+(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2);
const KamataEngine::Vector3 operator-(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2);

const KamataEngine::Vector3 operator*(const KamataEngine::Vector3& v, const float f);
const KamataEngine::Vector3 operator*(const float f, const KamataEngine::Vector3& v);

KamataEngine::Vector3& operator+=(KamataEngine::Vector3& lhv, const KamataEngine::Vector3& rhv);
KamataEngine::Vector3& operator-=(KamataEngine::Vector3& lhv, const KamataEngine::Vector3& rhv);
KamataEngine::Vector3& operator*=(KamataEngine::Vector3& v, float s);
KamataEngine::Vector3& operator/=(KamataEngine::Vector3& v, float s);

KamataEngine::Vector3 operator+(const KamataEngine::Vector3& v);
KamataEngine::Vector3 operator-(const KamataEngine::Vector3& v);

// Vector3 + float
const KamataEngine::Vector3 operator+(const KamataEngine::Vector3& v, float f);

// Vector3 - float
const KamataEngine::Vector3 operator-(const KamataEngine::Vector3& v, float f);

// ベクトル変換
KamataEngine::Vector3 TransformNormal(const KamataEngine::Vector3& v, const KamataEngine::Matrix4x4& m);
// 正規化
KamataEngine::Vector3 Normalize(const KamataEngine::Vector3& v);
// 座標変換
KamataEngine::Vector3 Transform(const KamataEngine::Vector3& vector, const KamataEngine::Matrix4x4& matrix4x4);
// 線形補間（ベクトル）
KamataEngine::Vector3 Lerp(const KamataEngine::Vector3& a, const KamataEngine::Vector3& b, float t);
// 球面線形補間
KamataEngine::Vector3 Slerp(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2, float t);

// 代入演算子オーバーロード
KamataEngine::Matrix4x4& operator*=(KamataEngine::Matrix4x4& lhm, const KamataEngine::Matrix4x4& rhm);

// 2項演算子オーバーロード
KamataEngine::Matrix4x4 operator*(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2);

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
