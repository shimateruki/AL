#pragma once // 同じヘッダーファイルが複数回インクルードされるのを防ぐ

#include "KamataEngine.h" // KamataEngineの基本機能（Model, WorldTransform, Cameraなど）をインクルード
#include "math.h"         // 数学関連のユーティリティ（Mathクラスや関連関数）をインクルード

using namespace KamataEngine; // KamataEngine名前空間を使用

// 敵クラス 
class Enemy {

public:

	// model: 敵のモデル、camera: カメラ、position: 初期位置
	void Initialize(Model* model, Camera* camera, const Vector3& position);

	// 更新処理 
	void Update();

	// 描画処理 
	void Draw();

private:
	// 敵のワールド変換 
	WorldTransform worldTransformEnemy_;

	// 敵のモデル 
	Model* model_ = nullptr;

	// カメラへのポインタ 
	Camera* camera_ = nullptr;

	// 歩行速度 
	static inline const float kWalkSpeed = 0.02f;

	// 敵の速度
	Vector3 velocity_ = {};

	// 歩行モーションの開始角度 
	static inline const float kWalkMotionAngleStart = 0.0f;

	// 歩行モーションの終了角度 
	static inline const float kWalkMotionAngleEnd = 30.0f;

	// 歩行モーションの時間 
	static inline const float kWalkMotionTime = 1.0f;

	// 歩行タイマー
	float walkTimer = 0.0f;

	// 数学ユーティリティオブジェクト
	Math* math = nullptr;
};