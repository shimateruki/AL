#pragma once
#include "KamataEngine.h"

#include "Math.h"
class ViewProjection
{
public:
	KamataEngine::Matrix4x4 viewMatrix;           // ビュー変換行列
	KamataEngine::Matrix4x4 projectionMatrix;     // プロジェクション変換行列
	KamataEngine::Matrix4x4 viewProjectionMatrix; // ビュープロジェクション変換行列
	KamataEngine::Vector3 cameraPosition;         // カメラ位置
	KamataEngine::Vector3 targetPosition;         // ターゲット位置
	KamataEngine::Vector3 upDirection;            // 上方向ベクトル
	void Initialize();
	Math* math;
};
