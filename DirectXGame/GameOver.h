#pragma once
#include "KamataEngine.h"
#include "Fade.h"
#include "math.h"
#include"Player.h"
using namespace KamataEngine;
class GameOver 
{
public:
	// ★ 追加: ポーズ画面の各選択肢
	enum class PauseSelect {
		kContinue,    // ゲームを続ける
		kStageSelect, // ステージセレクトに戻る
		kTitle,       // タイトルへ戻る
	};

	enum class NextScene { kRetry, kStageSelect, kTitle, kNone };
	NextScene GetNextScene() const { return nextScene_; }
	void Initialize();
	void Update();
	void Draw();
	bool isFinished() const { return finished_; }

	~GameOver();

private:
	enum class Phase {
		kFadeIn,  // フェードイン中
		kDisplay, // 表示中
		kFadeOut, // フェードアウト中
	};
	KamataEngine::Model* playerModel= nullptr;
	KamataEngine::Model* backgroundModel_ = nullptr;
	KamataEngine::Camera camera_;
	NextScene nextScene_ = NextScene::kNone;
		PauseSelect currentSelect_ = PauseSelect::kContinue;
	int currentSelectIndex_ = 0; // 現在の選択肢インデックス
	Phase phase_ = Phase::kFadeIn;
	bool finished_ = false;
	KamataEngine::Sprite* gameOverSprite_ = nullptr;
	Fade* fade_ = nullptr;
	Player* player_ = nullptr;
	Math* math = nullptr;
	KamataEngine::WorldTransform background_;
	bool isTimer;
	int displayTimer_ = 0;
	int textureHandle_ = 0;
	uint32_t TextureHandleYazirusi_ = 0;
	KamataEngine::Sprite* yazirusiSprite = nullptr;
	KamataEngine::ObjectColor bacgroundColor;
};
