#pragma once
#include "KamataEngine.h" // KamataEngineの基本機能（Model, WorldTransform, Cameraなど）をインクルード
#include"math.h"
class HitEffect {
public:
	enum class State {
		kSpread, // 拡大中
		kFade,   // フェードアウト中
		kDead    // 死亡
	};


	// 初期化
	void Initialize(KamataEngine::Vector3 postion);
	// 更新
	void Update();
	// 描画
	void Draw();
	static void SetModel(KamataEngine::Model* model) { model_ = model; }
	static void SetCamera(KamataEngine::Camera* camera) { camera_ = camera; }

	bool IsDead() const { return state_ == State::kDead; }
	static HitEffect* create(KamataEngine::Vector3 position);

	~HitEffect();
	private:
	// 拡大アニメーションの時間
	static inline const uint32_t kSpreadTime = 10;
	// フェードアウトアニメーションの時間
	static inline const uint32_t kFadeTime = 120;
	// エフェクトの寿命
	static inline const uint32_t kLifetime = kSpreadTime + kFadeTime;

	static KamataEngine::Model* model_;             // モデルへのポインタ
	static  KamataEngine::Camera* camera_; // カメラへのポインタ

	//円のワールドトランスフォームー
	KamataEngine::WorldTransform circleWorldTransform_; // ワールド変換
	Math* math = nullptr;                               // Mathクラスのインスタンス（シングルトンや依存性注入を推奨）

        
	// エフェクトが現在アクティブ（表示中）かどうか
	// 
	//円形エフェクト

	std::array<KamataEngine::WorldTransform, 3> ellipseWorldTransforms_; // 円形エフェクトのワールド変換配列

	// 楕円エフェクトの数
	static const inline uint32_t kellipseEffectNum = 2;

	State state_ = State::kSpread;
	// カウンター
	uint32_t counter_ = 0;
	KamataEngine::ObjectColor objectColor_;
	
};
