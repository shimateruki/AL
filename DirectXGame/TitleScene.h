#pragma once
#include "KamataEngine.h"
#include "math.h"
#include "Player.h"
#include "Fade.h"
#include"skydome.h"

class TitleScene 
{
public:
	enum class Phase {
		kFadeIn,
		kMain,
		kfadeOut

	};
	void Initialize();
	void Update();
	void Draw();
	 ~TitleScene();
	bool isfinished() const { return finished_; }

private:
	KamataEngine::Model* playerModel_ = nullptr;
	KamataEngine::WorldTransform worldTransform_; 
	KamataEngine::Camera camera_;
	KamataEngine::Model* modelSkydome_ = nullptr; // スカイドームモデル

	KamataEngine::Sprite* startSprite_ = nullptr; // スタートボタン用スプライト
	KamataEngine::Sprite* enterSprite_ = nullptr; // エンターキー用スプライト
	Math* math;
	Player* player_;
	Skydome* skydome_ = nullptr; // スカイドームオブジェクト
	// ★追加: 文字表示用のModelとWorldTransform
	KamataEngine::Model* titleTextModel_;                  
	KamataEngine::WorldTransform titleTextWorldTransform_; // 文字モデルのワールド変
	KamataEngine::ObjectColor color_;

	// ★イージング用のメンバ変数を変更
	float floatingTimer_;      // 揺れるためのタイマー（時間の経過に使う）
	float floatingAmplitudeY_; // Y軸方向の揺れの振幅（どれくらい上下するか）
	float floatingSpeed_;      // 揺れの速さ（速いほど振動が頻繁になる）
	Vector3 baseTextPos_;      // 文字の基準位置（この位置を中心に揺れる）
	bool finished_ = false;
	Fade* fade_ = nullptr;
	Phase phase_ = Phase::kFadeIn;
	// ★追加: テクスチャハンドル
	uint32_t textureHandleStart_ = 0;      // テクスチャハンドル
	uint32_t textureHandleEnter_ = 0; // テクスチャハンドル
};
