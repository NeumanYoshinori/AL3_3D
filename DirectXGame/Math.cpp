#include "Math.h"

Matrix4x4 Math::MakeScaleMatrix(const Vector3& scale) {
	// 拡大縮小行列
	Matrix4x4 result = { scale.x, 0.0f, 0.0f, 0.0f, 0.0f, scale.y, 0.0f, 0.0f, 0.0f, 0.0f, scale.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };

	return result;
}

Matrix4x4 Math::MakeRotateXMatrix(float theta) {
	// X軸回転行列
	Matrix4x4 result = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, cos(theta), sin(theta), 0.0f, 0.0f, -sin(theta), cos(theta), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };

	return result;
}

Matrix4x4 Math::MakeRotateYMatrix(float theta) {
	// Y軸回転行列
	Matrix4x4 result = { cos(theta), 0.0f, -sin(theta), 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, sin(theta), 0.0f, cos(theta), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };

	return result;
}

Matrix4x4 Math::MakeRotateZMatrix(float theta) {
	// Z軸回転行列
	Matrix4x4 result = { cos(theta), sin(theta), 0.0f, 0.0f, -sin(theta), cos(theta), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };

	return result;
}

Matrix4x4 Math::MakeTranslateMatrix(const Vector3& translate) {
	// 平行移動行列
	Matrix4x4 result = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, translate.x, translate.y, translate.z, 1.0f };

	return result;
}

Matrix4x4 Math::MakeAffineMatrix(const Vector3& scale, const Vector3& rot, const Vector3& translate) {
	// 拡大縮小行列
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);

	// X軸回転行列
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rot.x);
	// Y軸回転行列
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rot.y);
	// Z軸回転行列
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rot.z);
	// XYZ軸回転行列
	Matrix4x4 rotateXYZMatrix = rotateXMatrix * rotateYMatrix * rotateZMatrix;

	// 平行移動行列
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);

	Matrix4x4 result = scaleMatrix * rotateXYZMatrix * translateMatrix;

	return result;
}

float Math::EaseOut(float x1, float x2, float t) {
	float easedT = 1.0f - powf(1.0f - t, 3.0f);

	return Lerp(x1, x2, easedT);
}

float Math::EaseInOut(float x1, float x2, float t) {
	float easedT = -(cosf(pi_v<float> * t) - 1.0f) / 2.0f;

	return Lerp(x1, x2, easedT);
}

// 線形補間(Vector3)
Vector3 Math::Lerp(const Vector3& a, const Vector3& b, float t) {
	Vector3 result{};
	result.x = (1.0f - t) * a.x + t * b.x;
	result.y = (1.0f - t) * a.y + t * b.y;
	result.z = (1.0f - t) * a.z + t * b.z;

	return result;
}

// 線形補間(float)
float Math::Lerp(float x1, float x2, float t) { return (1.0f - t) * x1 + t * x2; }

// 内積
float Math::Dot(const Vector3& v1, const Vector3& v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

// 度をラジアンに変換
float Math::ToRadians(float degree) { return degree * pi_v<float> / 180.0f; }

// 座標変換
Vector3 Math::Transform(const Vector3& vector, const Matrix4x4& matrix4x4) {
	Vector3 result = {};
	result.x = vector.x * matrix4x4.m[0][0] + vector.y * matrix4x4.m[1][0] + vector.z * matrix4x4.m[2][0] + 1.0f * matrix4x4.m[3][0];
	result.y = vector.x * matrix4x4.m[0][1] + vector.y * matrix4x4.m[1][1] + vector.z * matrix4x4.m[2][1] + 1.0f * matrix4x4.m[3][1];
	result.z = vector.x * matrix4x4.m[0][2] + vector.y * matrix4x4.m[1][2] + vector.z * matrix4x4.m[2][2] + 1.0f * matrix4x4.m[3][2];
	float w = vector.x * matrix4x4.m[0][3] + vector.y * matrix4x4.m[1][3] + vector.z * matrix4x4.m[2][3] + 1.0f * matrix4x4.m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}

const Vector3 operator+(const Vector3& v1, const Vector3& v2) {
	Vector3 temp(v1);
	return temp += v2;
}


const Vector3 operator*(const Vector3& v1, const float f) {
	Vector3 temp(v1);
	return temp *= f;
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

Vector3 operator+(const Vector3& v) { return v; }
Vector3 operator-(const Vector3& v) { return Vector3(-v.x, -v.y, -v.z); }