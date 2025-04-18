#pragma once
#include "KamataEngine.h"

//ゲームシーン
class GameScene
{
public:



	//初期化
	void Initialize();

	//更新
	void Update();

	//描画
	void Draw();
	
	~GameScene();

private:

	uint32_t textureHandle_ = 0;
	uint32_t soundDatahandle_ = 0;
	uint32_t voiceHandle_ = 0;
	float inputFloat3[3] = {0, 0, 0};
	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;

	// カメラ
	KamataEngine::Camera camera_;

	// 3dモデル
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Sprite* sprite_ = nullptr;

	//デバッグカメラ
	KamataEngine::DebugCamera* debugcamera_ = nullptr;
};