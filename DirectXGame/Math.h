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
	float EaseInOutSine(float t, float x1, float x2);
	KamataEngine::Vector3 Lerp(const KamataEngine::Vector3& a, const KamataEngine::Vector3& b, float t);
	KamataEngine::Matrix4x4 makeIdentity4x4();
	KamataEngine::Matrix4x4 MakeScaleMatrix(const KamataEngine::Vector3& scale);

	KamataEngine::Matrix4x4 MakeRotateXMatrix(float theta);

	KamataEngine::Matrix4x4 MakeRotateYMatrix(float theta);

	KamataEngine::Matrix4x4 MakeRotateZMatrix(float theta);

	KamataEngine::Matrix4x4 MakeTranslateMatrix(const KamataEngine::Vector3& translate);

	KamataEngine::Matrix4x4 Multiply(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2);

	KamataEngine::Matrix4x4 MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotate, const KamataEngine::Vector3& translate);

	KamataEngine::Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

	KamataEngine::Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
	void worldTransFormUpdate(KamataEngine::WorldTransform& worldTransform);
	bool IsCollision(const AABB& aabb1, const AABB& aabb2);
	KamataEngine::Vector3 Transform(const KamataEngine::Vector3& vector, const KamataEngine::Matrix4x4& matrix);
	Vector3 Normalize(const KamataEngine::Vector3& v);
	KamataEngine::Matrix4x4 MakeLookAtMatrix(const KamataEngine::Vector3& eye, const KamataEngine::Vector3& target, const KamataEngine::Vector3& up);

	KamataEngine::Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearZ, float farZ);

};
