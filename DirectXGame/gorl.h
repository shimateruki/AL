#pragma once
#include "KamataEngine.h"
#include "math.h"
using namespace KamataEngine;
class Math;
class Gorl {
public:
	// コンストラクタ
	Gorl() = default;
	// デストラクタ
	~Gorl() = default;
	// 初期化関数
	void Initialize(Model* model, Camera* camera, const Vector3& position);
	// 更新関数
	void Update();
	// 描画関数
	void Draw();
	Vector3 GetWorldPosition() const;

	AABB GetAABB();

private:
	// ワールド変換
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// カメラ
	Camera* camera_ = nullptr;

	Math* math = nullptr; // 数学ユーティリティクラスのインスタンス

	const float kWidth = 1.0f;  // 幅
	const float kHeight = 0.8f; // 高さ
};
