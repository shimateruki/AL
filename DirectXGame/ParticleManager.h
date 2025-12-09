#pragma once
#include "KamataEngine.h"
#include "math.h"
#include <list>

using namespace KamataEngine;

// 個別のパーティクルのデータ構造
struct Particle {
	WorldTransform worldTransform;
	Vector3 velocity;
	Vector3 acceleration;
	float lifeTimer;
	float maxLife;
	Vector4 color;      // 現在の色
	Vector4 startColor; // 開始時の色
	Vector4 endColor;   // 終了時の色
	float startScale;
	float endScale;
};

class ParticleManager {
public:
	// 初期化
	void Initialize(Model* model, Camera* camera);

	// 更新
	void Update();

	// 描画
	void Draw(ID3D12GraphicsCommandList* commandList);

	// パーティクルを発生させる関数（ここを呼んで演出を作る！）
	// pos: 発生位置, vel: 速度, accel: 加速度, life: 寿命, startScale: 開始サイズ, endScale: 終了サイズ
	void Emit(
	    const Vector3& pos, const Vector3& vel, const Vector3& accel, float life, float startScale, float endScale, const Vector4& startColor = {1, 1, 1, 1}, const Vector4& endColor = {1, 1, 1, 0});

	// 全消去（シーン切り替え時など）
	void Clear();

private:
	std::list<Particle> particles_; // パーティクルのリスト
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;
	Math* math = nullptr;
	ObjectColor objectColor_; // 色変更用
};