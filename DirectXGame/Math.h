#pragma once
#include "KamataEngine.h"
#include <algorithm>
#include <cmath>

constexpr float PI = 3.14159265358979323846f; // 定数PIを追加

KamataEngine::Vector3 operator+(const KamataEngine::Vector3& objA, const KamataEngine::Vector3& objB);
KamataEngine::Vector3 operator-(const KamataEngine::Vector3& objA, const KamataEngine::Vector3& objB);
KamataEngine::Vector3 operator+(KamataEngine::Vector3& v, float s);
KamataEngine::Vector3 operator+=(KamataEngine::Vector3& v, KamataEngine::Vector2 s);
KamataEngine::Vector3 operator*(const KamataEngine::Vector3& objA, const float objB);

KamataEngine::Vector3 operator/(const KamataEngine::Vector3& objA, const float objB);
// 代入演算子オーバーロード
KamataEngine::Vector3& operator+=(KamataEngine::Vector3& lhs, const KamataEngine::Vector3& rhv);
KamataEngine::Vector3& operator-=(KamataEngine::Vector3& lhs, const KamataEngine::Vector3& rhv);
KamataEngine::Vector3& operator*=(KamataEngine::Vector3& v, float s);
KamataEngine::Vector3& operator/=(KamataEngine::Vector3& v, float s);

struct AABB {
	KamataEngine::Vector3 min;
	KamataEngine::Vector3 max;
};



class Math {
public:
static	float EaseInOutSine(float t, float x1, float x2);
static	KamataEngine::Vector3 Lerp(const KamataEngine::Vector3& a, const KamataEngine::Vector3& b, float t);
static	KamataEngine::Matrix4x4 makeIdentity4x4();
static	KamataEngine::Matrix4x4 MakeScaleMatrix(const KamataEngine::Vector3& scale);
static	KamataEngine::Matrix4x4 MakeRotateXMatrix(float theta);
static	KamataEngine::Matrix4x4 MakeRotateYMatrix(float theta);
static	KamataEngine::Matrix4x4 MakeRotateZMatrix(float theta);
static	KamataEngine::Matrix4x4 MakeTranslateMatrix(const KamataEngine::Vector3& translate);
static	KamataEngine::Matrix4x4 Multiply(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2);
static	KamataEngine::Matrix4x4 MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotate, const KamataEngine::Vector3& translate);
static	KamataEngine::Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);
static	KamataEngine::Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
static	void worldTransFormUpdate(KamataEngine::WorldTransform& worldTransform);
static	bool IsCollision(const AABB& aabb1, const AABB& aabb2);
static	KamataEngine::Vector3 Transform(const KamataEngine::Vector3& vector, const KamataEngine::Matrix4x4& matrix);
static	KamataEngine::Vector3 Normalize(const KamataEngine::Vector3& v);
static	float Dot(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2);
static	KamataEngine::Vector3 Cross(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2);
static	KamataEngine::Matrix4x4 MakeLookAtMatrix(const KamataEngine::Vector3& eye, const KamataEngine::Vector3& target, const KamataEngine::Vector3& up);
static	KamataEngine::Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearZ, float farZ);

};
