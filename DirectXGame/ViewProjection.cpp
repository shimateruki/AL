#include "ViewProjection.h"
#include"Math.h"
#include <numbers>
void ViewProjection::Initialize() 
{
	// カメラ位置、ターゲット位置、上方向ベクトルの初期化
	cameraPosition = {0.0f, 0.0f, -5.0f};
	targetPosition = {0.0f, 0.0f, 0.0f};
	upDirection = {0.0f, 1.0f, 0.0f};
	// ビュー変換行列の計算
	viewMatrix = math-> MakeLookAtMatrix(cameraPosition, targetPosition, upDirection);
	// プロジェクション変換行列の計算
	projectionMatrix = math-> MakePerspectiveFovMatrix (std::numbers::pi_v<float>/ 4.0f, 16.0f / 9.0f, 0.1f, 100.0f);
	// ビュープロジェクション変換行列の計算
	viewProjectionMatrix = math-> Multiply(viewMatrix, projectionMatrix);
}
