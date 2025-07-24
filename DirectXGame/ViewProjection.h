#pragma once
#include "KamataEngine.h"
#include "Math.h"


class ViewProjection {
public:
	// 初期化
	void Initialize();

	// カメラ情報の更新
	void Update();

	// 内部のCameraオブジェクトへの参照を返す
	// constオーバーロードも追加して安全性を高める
	KamataEngine::Camera& GetCamera() { return camera_; }
	const KamataEngine::Camera& GetCamera() const { return camera_; }

private:
	KamataEngine::Camera camera_; // Cameraオブジェクトをメンバ変数として持つ
};