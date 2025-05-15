#pragma once
#include "KamataEngine.h"
#include "math.h"
class Block {

public:

	void Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position, KamataEngine::Camera* camera);

	void Update();
	void Draw();

private:
	KamataEngine::WorldTransform worldTransformPlayer_;
	KamataEngine::WorldTransform blockModel_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	Math* math;
};
