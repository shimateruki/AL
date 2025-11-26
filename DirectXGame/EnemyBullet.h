#pragma once
#include "KamataEngine.h"
#include"math.h"
#include "MapChipField.h"
class EnemyBullet {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position, const KamataEngine::Vector3& velocity, MapChipField* mapChipField);
	// 更新
	void Update();
	// 描画
	void Draw(KamataEngine::Camera& camera);

	// 死亡フラグ（一定時間経過や壁衝突で死ぬ）
	bool IsDead() const { return isDead_; }
	void OnCollision(); // 何かに当たった時の処理

	// 当たり判定用
	KamataEngine::Vector3 GetWorldPosition() const;
	AABB GetAABB();

private:
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Vector3 velocity_ = {}; // 速度

	bool isDead_ = false;
	float lifeTimer_ = 0.0f;
	const float kLifeTime = 3.0f; // 3秒で消滅
	MapChipField* mapChipField_ = nullptr;

	// 弾のサイズ
	const float kRadius = 0.5f;

	Math* math_;
};