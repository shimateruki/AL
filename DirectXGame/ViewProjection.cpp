#include "ViewProjection.h"
#include <numbers>
void ViewProjection::Initialize()
{
	// まず、内部のcamera_オブジェクトを初期化する
	// これにより、定数バッファなどが作成される
	camera_.Initialize();

	// ビュー行列を計算し、camera_に設定
	KamataEngine::Vector3 eye = {0.0f, 0.0f, -5.0f};
	KamataEngine::Vector3 target = {0.0f, 0.0f, 0.0f};
	KamataEngine::Vector3 up = {0.0f, 1.0f, 0.0f};
	camera_.matView = Math::MakeLookAtMatrix(eye, target, up);
	// プロジェクション行列を計算し、camera_に設定
	float fov = std::numbers::pi_v<float> / 4.0f;
	float aspectRatio = 16.0f / 9.0f;
	float nearClip = 0.1f;
	float farClip = 100.0f;
	camera_.matProjection= Math::MakePerspectiveFovMatrix(fov, aspectRatio, nearClip, farClip);

	// 行列の更新処理を呼び出す
	// KamataEngine::Cameraにこのようなメソッドがあるか確認してください
	camera_.UpdateMatrix();
}

void ViewProjection::Update() {
	// カメラの移動や回転など、動的な更新が必要な場合に実装
	// ここでcamera_の各行列を再計算し、UpdateMatrix()を呼び出す
}
