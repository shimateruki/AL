#pragma once
#include "KamataEngine.h"
#include<cmath>


KamataEngine::Matrix4x4 MakeScaleMatrix(const KamataEngine::Vector3& scale);

KamataEngine::Matrix4x4 MakeRotateXMatrix(float theta);

KamataEngine::Matrix4x4 MakeRotateYMatrix(float theta);

KamataEngine::Matrix4x4 MakeRotateZMatrix(float theta);

KamataEngine::Matrix4x4 MakeTranslateMatrix(const KamataEngine::Vector3& translate);

KamataEngine::Matrix4x4 Multiply(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2);


KamataEngine::Matrix4x4 MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotate, const KamataEngine::Vector3& translate);


KamataEngine::Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);



KamataEngine::Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);