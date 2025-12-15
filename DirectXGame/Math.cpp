#include "Math.h"
#include <algorithm>

using namespace KamataEngine;
using namespace std;

// 内積
float Dot(const Vector3& v1, const Vector3& v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

// 長さ(ノルム)
float Length(const Vector3& v) { return sqrtf(powf(v.x, 2) + powf(v.y, 2) + powf(v.z, 2)); }

// 線形補間
float Lerp(float a, float b, float t) { return a + (b - a) * t; };

const Vector3 operator+(const Vector3& v1, const Vector3& v2) {
	Vector3 temp(v1);
	return temp += v2;
}

const Vector3 operator-(const Vector3& v1, const Vector3& v2) {
	Vector3 temp(v1);
	return temp -= v2;
}

const Vector3 operator*(const Vector3& v, const float f) {
	Vector3 temp(v);
	return temp *= f;
}

const Vector3 operator*(const float f, const Vector3& v) {
	Vector3 temp(v);
	return temp *= f;
}

const Vector3 operator/(const Vector3& v, float f) {
	Vector3 temp(v);
	return temp /= f;
}

Vector3& operator+=(Vector3& lhv, const Vector3& rhv) {
	lhv.x += rhv.x;
	lhv.y += rhv.y;
	lhv.z += rhv.z;
	return lhv;
}

Vector3& operator-=(Vector3& lhv, const Vector3& rhv) {
	lhv.x -= rhv.x;
	lhv.y -= rhv.y;
	lhv.z -= rhv.z;
	return lhv;
}

Vector3& operator*=(Vector3& v, float s) {
	v.x *= s;
	v.y *= s;
	v.z *= s;
	return v;
}

Vector3& operator/=(Vector3& v, float s) {
	v.x /= s;
	v.y /= s;
	v.z /= s;
	return v;
}

Vector3 operator+(const Vector3& v) { return v; }
Vector3 operator-(const Vector3& v) { return Vector3(-v.x, -v.y, -v.z); }

Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m) {
	return {v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0], v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1], v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2]};
}

// 正規化
Vector3 Normalize(const Vector3& v) {
	float len = Length(v);

	if (len == 0.0f) {
		return {0.0f, 0.0f, 0.0f};
	}

	return v / len;
}

// 座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix4x4) {
	Vector3 result = {};
	result.x = vector.x * matrix4x4.m[0][0] + vector.y * matrix4x4.m[1][0] + vector.z * matrix4x4.m[2][0] + 1.0f * matrix4x4.m[3][0];
	result.y = vector.x * matrix4x4.m[0][1] + vector.y * matrix4x4.m[1][1] + vector.z * matrix4x4.m[2][1] + 1.0f * matrix4x4.m[3][1];
	result.z = vector.x * matrix4x4.m[0][2] + vector.y * matrix4x4.m[1][2] + vector.z * matrix4x4.m[2][2] + 1.0f * matrix4x4.m[3][2];
	float w = vector.x * matrix4x4.m[0][3] + vector.y * matrix4x4.m[1][3] + vector.z * matrix4x4.m[2][3] + 1.0f * matrix4x4.m[3][3];
	assert(w != 0.0f);
	result /= w;

	return result;
}

// 線形補間(Vector3)
Vector3 Lerp(const Vector3& a, const Vector3& b, float t) { return (1.0f - t) * a + t * b; }

Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t) {
	// 正規化ベクトルを求める
	Vector3 nv1 = Normalize(v1);
	Vector3 nv2 = Normalize(v2);

	// 内積を求める
	float dot = Dot(nv1, nv2);

	// 誤差により1.0fを超えるのを防ぐ
	dot = clamp(dot, -1.0f, 1.0f);
	// アークコサインでθの角度を求める
	float theta = acos(dot);
	// θの角度からsinθを求める
	float sinTheta = sin(theta);
	// サイン(θ(1-t))を求める
	float sinThetaFrom = sin((1 - t) * theta);
	// サインθtを求める
	float sinThetaTo = sin(t * theta);

	Vector3 npVector;
	// ゼロ除算を防ぐ
	if (sinTheta < 1.0e-5) {
		npVector = nv1;
	} else {
		// 球面線形保管したベクトル（単位ベクトル）
		npVector = (sinThetaFrom * nv1 + sinThetaTo * nv2) / sinTheta;
	}

	// ベクトルの長さはv1とv2の長さを線形補間
	float length1 = Length(v1);
	float length2 = Length(v2);
	// Lerpで補間ベクトルの長さを求める
	float length = Lerp(length1, length2, t);

	// 長さを反映
	return length * npVector;
}

Matrix4x4& operator*=(Matrix4x4& lhm, const Matrix4x4& rhm) {
	Matrix4x4 result{};

	for (size_t i = 0; i < 4; i++) {
		for (size_t j = 0; j < 4; j++) {
			for (size_t k = 0; k < 4; k++) {
				result.m[i][j] += lhm.m[i][k] * rhm.m[k][j];
			}
		}
	}
	lhm = result;
	return lhm;
}

Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result = m1;

	return result *= m2;
}

// 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale) { return {scale.x, 0.0f, 0.0f, 0.0f, 0.0f, scale.y, 0.0f, 0.0f, 0.0f, 0.0f, scale.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}; }

// X軸回転行列
Matrix4x4 MakeRotateXMatrix(float theta) { return {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, cos(theta), sin(theta), 0.0f, 0.0f, -sin(theta), cos(theta), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}; }

// Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float theta) { return {cos(theta), 0.0f, -sin(theta), 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, sin(theta), 0.0f, cos(theta), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}; }

// Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float theta) { return {cos(theta), sin(theta), 0.0f, 0.0f, -sin(theta), cos(theta), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}; }

// 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate) { return {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, translate.x, translate.y, translate.z, 1.0f}; }

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rot, const Vector3& translate) {
	// 拡大縮小行列
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);

	// X軸回転行列
	Matrix4x4 rotXMatrix = MakeRotateXMatrix(rot.x);
	// Y軸回転行列
	Matrix4x4 rotYMatrix = MakeRotateYMatrix(rot.y);
	// Z軸回転行列
	Matrix4x4 rotZMatrix = MakeRotateZMatrix(rot.z);
	// XYZ軸回転行列
	Matrix4x4 rotXYZMatrix = rotXMatrix * rotYMatrix * rotZMatrix;

	// 平行移動行列
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);

	return scaleMatrix * rotXYZMatrix * translateMatrix;
}

void WorldTransformUpdate(WorldTransform& worldTransform) {
	// スケール、回転、平行移動を合成して行列を計算する
	worldTransform.matWorld_ = MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);
	// 定数バッファへの書き込み
	worldTransform.TransferMatrix();
}