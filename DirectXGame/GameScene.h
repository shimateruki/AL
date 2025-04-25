#pragma once
#include "KamataEngine.h"
#include "Player.h"
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
	uint32_t textureHandel_ = 0;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Camera camera_;
	Player* player_ = nullptr;
	

};