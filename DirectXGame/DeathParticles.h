#pragma once
#include "KamataEngine.h"
#include <array>
#include "math.h"
#include <numbers> // std::numbers::pi_v を使用するためにインクルード
using namespace KamataEngine;
class Player;

class DeathParticles 
{
public:
	
	void Initialize(Model* model, Camera* camera, const Player* player, const Vector3& position);

	// 更新処理
	void Update();

	// 描画処理
	void Draw()
		;

private:
	// 敵のモデル
	Model* model_ = nullptr;

	// カメラへのポインタ
	Camera* camera_ = nullptr;
	//パーティクルの個数
	static inline const uint32_t kNumParticles = 8;
	std::array<WorldTransform, kNumParticles> worldTransform_;
	Math* math;
	//存在時間
	static inline const float kDuration = 5.0f;
	//移動のスピード
	static inline const float kSpeed = 0.1f;
	//分割した一個分の角度
	static inline const float kAngleUnit = 2 * std::numbers::pi_v<float> / kNumParticles;

	//終了フラグ
	bool isFinished_ = false;
	//時間経過カウント
	float counter_ = 0.0f;

};
