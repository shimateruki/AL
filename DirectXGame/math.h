#pragma once
#include "KamataEngine.h"
#include<cmath>
#include<algorithm>

KamataEngine::Vector3 operator+(const KamataEngine::Vector3& objA, const KamataEngine::Vector3& objB);
KamataEngine::Vector3 operator-(const KamataEngine::Vector3& objA, const KamataEngine::Vector3& objB);
KamataEngine::Vector3 operator+(KamataEngine::Vector3& v, float s);
KamataEngine::Vector3 operator*(const KamataEngine::Vector3& objA, const float objB);
    // 代入演算子オーバーロード
KamataEngine::Vector3& operator+=(KamataEngine::Vector3& lhs, const KamataEngine::Vector3& rhv);
KamataEngine::Vector3& operator-=(KamataEngine::Vector3& lhs, const KamataEngine::Vector3& rhv);
KamataEngine::Vector3& operator*=(KamataEngine::Vector3& v, float s);
KamataEngine::Vector3& operator/=(KamataEngine::Vector3& v, float s);

class Math {
public:



float EaseInOutSine(float t, float x1, float x2);
	KamataEngine::Vector3 Lerp(const KamataEngine::Vector3& a, const KamataEngine::Vector3& b, float t);
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
};