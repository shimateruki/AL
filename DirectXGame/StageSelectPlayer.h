#pragma once

#include "KamataEngine.h"
#include <vector>
#include "math.h"
using namespace KamataEngine;
class StageSelectPlayer {
public:
	void Initialize(Model* model, Camera* camera, const Vector3& position);
	void HandleInput();
	void Update();
	void Draw();

	// ノード（移動先ポイント）を追加
	void AddNode(const Vector3& nodePos);

	Vector3 GetWorldPosition() const;

	AABB GetAABB();
	void MoveTo(const Vector3& target);         // ←追加
	bool IsMoving() const { return isMoving_; } // 移動中かどうか

	// ==== カメラ追従用 Getter ====
	const WorldTransform& GetWorldTransform() const { return worldTransformPlayer_; }
	const Vector3& GetVelocity() const { return velocity_; }

private:
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;

	WorldTransform worldTransformPlayer_;
	std::vector<Vector3> nodes_; // ステージ選択のポイント

	// 速度計算用
	Vector3 prevPosition_{0, 0, 0};
	Vector3 velocity_{0, 0, 0};
	Math* math = nullptr; // 数学ユーティリティクラスのインスタンス

	const float kWidth = 1.0f; // AABBの幅
	const float kHeight = 0.8f; // AABBの高さ

	size_t currentNodeIndex_ = 0; // 今いるノード
	float moveSpeed_ = 0.1f;      // 移動速度
	Vector3 targetPosition_;
	float targetRotationY_ = 0.0f;
	bool isMoving_ = false;

};