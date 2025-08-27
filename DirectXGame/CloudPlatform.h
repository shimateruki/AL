#pragma once
#include "KamataEngine.h"
#include "math.h"
using namespace KamataEngine;

class CloudPlatform 
{
public:
	// 初期化
	void Initialize(Model* model, Camera* camera, const Vector3& position);
	// 更新
	void Update();
	// 描画
	void Draw();
	
	// ワールド座標取得
	Vector3 GetWorldPosition() const;
	// AABB取得
	AABB GetAABB() const;

	Vector3 GetDelta() const;

	
	// ★ 追加: 動く足場用の設定
	void SetMovement(const Vector3& start, const Vector3& end, float speed);

	// ★ 追加: 前フレームの位置を取得
	const Vector3& GetPreviousPosition() const { return previousPosition_; }

private:
	Model* model_ = nullptr;               // モデル
	Camera* camera_ = nullptr;             // カメラ
	WorldTransform worldTransform_;        // ワールドトランスフォーム
	Math* math;
	// ★ 追加: 移動のための変数
	Vector3 startPosition_;
	Vector3 endPosition_;
	float speed_ = 0.0f;
	Vector3 direction_;
	Vector3 previousPosition_;
	float t_;
	float directionSign_ = 1; // 1ならend方向へ、-1ならstart方向へ
	Vector3 delta_;
	KamataEngine::ObjectColor color_;
	static constexpr float kWidth = 3.5f;  // 幅
	static constexpr float kHeight = 2.0f; // 高さ


};
