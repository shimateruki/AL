#pragma once
#include "KamataEngine.h"
#include "math.h"
using namespace KamataEngine;
class Yama 
{
public:
	// 初期化
	void Initialize(Model* model, Camera* camera, const Vector3& position);
	// 更新
	void Update();
	// 描画
	void Draw();

private:
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::ObjectColor color_;
	Model* model_ = nullptr; // モデル
	Camera* camera_ = nullptr; // カメラ
	Math* math = nullptr; // 数学ユーティリティ
	static inline const float kWidth = 1.0f;
	static inline const float kHeight = 1.0f;

}
;
