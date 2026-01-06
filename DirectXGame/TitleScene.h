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
	enum class NextScene {
		kGame,    // 通常のゲーム本編へ
		kTutorial // チュートリアルへ
	};
	void Initialize();
	void Update();
	void Draw();
	 ~TitleScene();
	bool isfinished() const { return finished_; }
	 NextScene GetNextScene() const { return nextScene_; }

private:
	KamataEngine::Model* playerModel_ = nullptr;
	KamataEngine::WorldTransform worldTransform_; 
	KamataEngine::Camera camera_;
	KamataEngine::Model* modelSkydome_ = nullptr; // スカイドームモデル

	KamataEngine::Sprite* startSprite_ = nullptr; // スタートボタン用スプライト
	KamataEngine::Sprite* spaceSprite_ = nullptr; // エンターキー用スプライト
	KamataEngine::Sprite* tutorialSprite_ = nullptr;
	Math* math;
	Player* player_;
	Skydome* skydome_ = nullptr; // スカイドームオブジェクト
	// 文字表示用のModelとWorldTransform
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
	//  テクスチャハンドル
	uint32_t textureHandleStart_ = 0;      // テクスチャハンドル
	uint32_t textureHandleEnter_ = 0; // テクスチャハンドル
	uint32_t textureHandleTutorial_ = 0;
	//----------------------------------------
	// オーディオハンドル
	//----------------------------------------
	// BGM
	uint32_t bgmHandle_ = 0;
	uint32_t bgmVoiceHandle_ = 0; // 再生中のBGMを停止するために必要
	bool isMusic;
	Audio *bgmAudio;
	//Se
	uint32_t enterSeHandle = 0;
	uint32_t enterVoice = 0;

	Audio* seAudio;


	float blinkTimer_;
	KamataEngine::Sprite* arrowSprite_ = nullptr;
	uint32_t textureHandleArrow_ = 0;
	//-- 選択管理 ---
	int currentSelection_ = 0;               // 0: Start, 1: Tutorial
	NextScene nextScene_ = NextScene::kGame; // デフォルトはゲーム
};