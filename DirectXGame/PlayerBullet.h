#pragma once
#include "KamataEngine.h"
#include "MapChipField.h"
#include"math.h"

class PlayerBullet {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position, const KamataEngine::Vector3& velocity, MapChipField* mapChipField, float scale, int damage);
	// 更新
	void Update();
	// 描画
	void Draw(KamataEngine::Camera& camera);

	bool IsDead() const { return isDead_; }
	void OnCollision(); // 当たった時の処理

	KamataEngine::Vector3 GetWorldPosition() const;
	AABB GetAABB();
	int GetDamage() const { return damage_; }

private:
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	MapChipField* mapChipField_ = nullptr; // 壁判定用
	KamataEngine::Vector3 velocity_ = {};

	bool isDead_ = false;
	float lifeTimer_ = 0.0f;
	const float kLifeTime = 1.0f; // 1秒で消える（射程距離）
	const float kRadius = 0.5f;   // 当たり判定の大きさ
	Math* math_ = nullptr;
	int damage_ = 1;
};