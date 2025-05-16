#pragma once

#include "KamataEngine.h"
#include "Player.h"
#include "math.h"
#include "skydome.h"
#include <vector>
// ゲームシーン
class GameScene {
public:
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	~GameScene();

private:
	uint32_t textureHandel_ = 0;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Camera camera_;
	Player* player_ = nullptr;
	bool isDebugCameraActive_ = false;
	// デバッグカメラ
	KamataEngine::DebugCamera* debaucamera_ = nullptr;

	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;
	Skydome* skydome_ = nullptr;
	KamataEngine::Model* playerModel_ = nullptr;
	KamataEngine::Model* modelSkydome_ = nullptr;


	Math* math;
};
