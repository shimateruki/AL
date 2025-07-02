#pragma once
#include "KamataEngine.h" // KamataEngineの基本機能（Model, WorldTransform, Cameraなど）をインクルード
class HitEffect {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	// 更新
	void Update();
	// 描画
	void Draw();
	static void SetModel(KamataEngine::Model* model) { model_ = model; }
	static void SetCamera(KamataEngine::Camera* camera) { camera_ = camera; }


	private:

	static KamataEngine::Model* model_;             // モデルへのポインタ
	static  KamataEngine::Camera* camera_; // カメラへのポインタ
	
};
