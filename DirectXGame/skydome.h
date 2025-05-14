#pragma once
#include "KamataEngine.h" 
#include "math.h"
class Skydome
{
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera);
	void Update();
	void Draw();

private:
	KamataEngine::WorldTransform worldTransformSkydome_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	Math* math_;
	
};