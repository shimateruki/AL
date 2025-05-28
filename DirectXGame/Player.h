#pragma once
#include"KamataEngine.h"
#include "math.h"
enum class LRDirection {
	kRight,
	kLeft
};

class Player
{
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, KamataEngine::Vector3& position);
	void Update();
	void Draw();
	const KamataEngine::WorldTransform &GetWorldTransformPlayer()const { return worldTransformPlayer_; }

private:
	KamataEngine::WorldTransform worldTransformPlayer_;
	KamataEngine::WorldTransform playerModel_;
	KamataEngine:: Model*  model_ = nullptr;
	uint32_t textureHandle_ = 0;
	KamataEngine::Camera* camera_ = nullptr;
	
	Math* math;
	KamataEngine::Vector3 velosity_ = {};
	static inline const float kAcceleration = 0.01f;//加速度
	static inline const float kAtteunuation = 0.1f;//速度減少量
	static inline const float kLimitRunSpeed = 5.0f;//最大速度
	LRDirection lrDirection_ = LRDirection::kRight;//方向
	float turnFirstRottationY_ = 0.0f;//旋回開始時の角度
	float turnTimer_ = 0.0f;//旋回タイマー
	static inline const float kTimeTurn = 0.3f;//旋回時間
	bool onGround_ = true;//着地フラグ
	static inline const float kGgravityAcceleration = 9.8f;// 重力加速度
	static inline const float kLimitFallSpeed = 5.0f;//最大落下加速度
	static inline const float kJumpAccleration = 0.1f;     // ジャンプ加速

};
