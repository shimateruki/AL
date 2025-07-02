#pragma once // 同じヘッダーファイルが複数回インクルードされるのを防ぐ
#include "KamataEngine.h" // KamataEngineの基本機能（Model, WorldTransform, Cameraなど）をインクルード
#include "math.h"         // 数学関連のユーティリティ（Mathクラスや関連関数）をインクルード

using namespace KamataEngine; // KamataEngine名前空間を使用

class Player;
// 敵クラス 
class Enemy {

public:
	// 敵の行動状態を表す列挙型
	enum class Behavior {
		kUnKnow,
		kRoot,
		kisDead, // 死亡状態

	};

	// model: 敵のモデル、camera: カメラ、position: 初期位置
	void Initialize(Model* model, Camera* camera, const Vector3& position);

	// 更新処理 
	void Update();

	// 描画処理 
	void Draw();

	Vector3 GetWorldPosition();
	bool GetIsDead(){ return isDead_; } // 敵が死亡しているかどうかを取得するメソッド
	AABB GetAABB();
	void onCollision(const Player* player);
	bool isCollisonDisabled() const { return isCollisDisabled_; } // 衝突無効化フラグを取得

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
	const float kWidth = 1.0f;  // 幅
	const float kHeight = 1.0f; // 高さ
	bool isDead_ = false;       // 敵が死亡しているかどうかのフラグ
	                            // 振る舞い
	Behavior behavior_ = Behavior::kRoot;
	// 次の振る舞いのリクエスト
	Behavior behaviorRequest_ = Behavior::kUnKnow;

	bool isCollisDisabled_ = false; // 衝突無効化フラグ（敵が死亡しているかどうかを示す）
};