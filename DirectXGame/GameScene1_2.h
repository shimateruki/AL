#pragma once
// ===== 必要なヘッダーファイル =====
#include "Block.h"
#include "CameraController.h"
#include "DeathParticles.h"
#include "Enemy.h"
#include "Fade.h"
#include "HitEffect.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "gorl.h"
#include "math.h"
#include "skydome.h"
#include"KabeToge.h"
#include"Toge.h"
#include "CloudPlatform.h"
#include"BreakableFloor.h"
#include"yama.h"
#include"tree.h"

class GameScene1_2 
{
public:
	// ===== ゲーム進行フェーズ =====
	enum class Phase {
		kFadeIn,  // フェードイン中
		kCountdown, // 新しいフェーズを追加
		kPlay,    // ゲームプレイ中
		kDeath,   // 死亡演出中
		kFadeOut, // フェードアウト中
		GameClear // ゲームクリア時
	};

		// ★ 追加: ポーズ画面の各選択肢
	enum class PauseSelect {
		kContinue,    // ゲームを続ける
		kStageSelect, // ステージセレクトに戻る
		kTitle,       // タイトルへ戻る
	};
	enum class CountdownState {
		kOff,      // カウントダウンが無効
		kCounting, // カウントダウン中
		kFinished  // カウントダウン完了
	};
	enum class NextScene { kGameOver, kStageSelect, kNone };
	// 初期化・更新・描画
	void Initialize();
	void Update();
	void Draw();

	// 各種ロジック
	void GenerrateBlock();
	void GenerateBlocks();
	// ブロック生成処理
	void CheekAllcollision();                                    // 当たり判定処理
	void ChangePhase();                                          // フェーズ切り替え
	void CreateHitEffect(const KamataEngine::Vector3& position); // ヒットエフェクト生成

	bool isFinished() const { return finished_; } // 終了フラグ取得
	int currentSelectIndex() const { return currentSelectIndex_; } // 現在の選択肢インデックスを取得
	NextScene GetNextScene() const { return nextScene_; }
	void LimitPlayerPosition();                   // 画面外に出ない制限
	~GameScene1_2();

private:
	// ===== モデル関連 =====
	uint32_t textureHandel_ = 0;                        // テクスチャハンドル
	KamataEngine::Model* dirtModel_ = nullptr;          // ブロックのモデル
	KamataEngine::Model* grassModel_ = nullptr;         // 草ブロックのモデル
	KamataEngine::Model* playerModel_ = nullptr;        // プレイヤーのモデル
	KamataEngine::Model* playerAttackModel_ = nullptr;  // プレイヤー攻撃モデル
	KamataEngine::Model* goalModel_ = nullptr;          // ゴールモデル
	KamataEngine::Model* modelSkydome_ = nullptr;       // スカイドームモデル
	KamataEngine::Model* enemy_model_ = nullptr;        // 敵モデル
	KamataEngine::Model* deatparticlesModel_ = nullptr; // デスパーティクルモデル
	KamataEngine::Model* hitEffectModel_ = nullptr;     // ヒットエフェクトモデル
	KamataEngine::Model* GameClearTextModel_ = nullptr; // ゲームクリアテキストモデル
	KamataEngine::Model* togeKabeModel_ = nullptr;      // トゲ壁モデル
	KamataEngine::Model* togeModel_ = nullptr;          // トゲモデル
	KamataEngine::Model* yamaModel = nullptr;           // 山モデル
	KamataEngine::Model* treeModel_ = nullptr;          // 木モデル
	KamataEngine::Sprite* Textmodel1_2 = nullptr;       // ゲームクリアテキスト1-2
	KamataEngine::Sprite* poseSprite = nullptr;         // 数字表示用スプライト
	KamataEngine::Sprite* yazirusiSprite = nullptr;
	KamataEngine::Sprite* enterSprite_ = nullptr; // エンターキー用スプライト
	KamataEngine::Model* CloudPlatformModel_ = nullptr; // 雲プラットフォームモデル
	KamataEngine::Sprite* pauseTextSprite_ = nullptr;   
	KamataEngine::Sprite* GameClearTextSprite_ = nullptr; // ゲームクリアテキスト用スプライト




	// ★ 追加: ポーズ関連のメンバ変数
	bool isPaused_ = false;
	PauseSelect currentSelect_ = PauseSelect::kContinue;
	int currentSelectIndex_ = 0; // 現在の選択肢インデックス

	// ===== ワールド変換関連 =====
	KamataEngine::WorldTransform worldTransform_;                                  // 共通ワールド変換
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_; // ブロック配置用
	KamataEngine::WorldTransform GameClearTextWorldTransform_;                     // ゲームクリアテキスト位置

	// ===== カメラ =====
	KamataEngine::Camera camera_;                      // メインカメラ
	bool isDebugCameraActive_ = false;                 // デバッグカメラ有効フラグ
	KamataEngine::DebugCamera* debaucamera_ = nullptr; // デバッグカメラ
	CameraController* CController_ = nullptr;          // カメラコントローラー

	// ===== キャラクター・オブジェクト =====
	Player* player_ = nullptr;             // プレイヤー
	Gorl* gorl_ = nullptr;                 // ゴール（Gorl）
	MapChipField* mapChipField_ = nullptr; // マップチップ
	Skydome* skydome_ = nullptr;           // スカイドーム
	Math* math = nullptr;                  // 数学ユーティリティ
	KabeToge* togeKabe_ = nullptr;         // 棘の壁
	Toge* toge_ = nullptr;                 // 棘
	std::vector<Yama*> yama_;              // 山リスト
	std::vector<Tree*> tree_;              // 木リスト

	std::vector<Enemy*> enemys_;

		std::vector<CloudPlatform*> CloudPlatform_; // 雲リスト


	// ===== パーティクル・エフェクト =====
	DeathParticles* deatparticles_ = nullptr; // 死亡エフェクト
	std::list<HitEffect*> hitEffects_;        // ヒットエフェクト
	const int HitEffectMax = 10;              // ヒットエフェクト最大数

	// ===== フェーズ管理・シーン状態 =====
	Phase phase_ = Phase::kFadeIn; // 現在のフェーズ
	Fade* fade_ = nullptr;         // フェード演出
	bool finished_ = false;        // シーン終了フラグ
	bool isSceneFinished_ = false; // フェーズ遷移完了フラグ
	int finishedTimer = 0;         // 終了タイマー
	bool isGameClear_ = false;     // ゲームクリアフラグ
	bool isTimerFinished_ = false; // タイマー完了フラグ

	// ===== オーディオ =====
	uint32_t bgmHandle_ = 0;      // BGMデータのハンドル
	uint32_t bgmVoiceHandle_ = 0; // BGM再生のハンドル

	int textureHandle = 0; // テクスチャハンドル
	uint32_t textureHandlePhose_ = 0;
	uint32_t TextureHandleYazirusi_ = 0;
	uint32_t textureHandlePauseText_ = 0; // ポーズテキスト用テクスチャハンドル
	uint32_t textureHandleGameClearText_ = 0; // ゲームクリアテキスト用テクスチャハンドル

	bool isSprite = false; // スプライト表示フラグ
	uint32_t textureHandleEnter_ = 0; // テクスチャハンドル

		 KamataEngine::Vector3 playerPreviousPosition_; // 前フレームのプレイヤー座標
	bool isOnPlatform_ = false;                    // 足場に乗っているかどうか

	   // ★ 壊れる床のための構造体を追加
	struct BreakableBlock {
		WorldTransform* worldTransform;
		bool isSteppedOn = false;      // プレイヤーが乗ったかどうか
		float timer = 0.0f;            // 崩れるまでのタイマー
		const float kBreakTime = 3.0f; // 崩れるまでの時間（3秒）
	};
	NextScene nextScene_ = NextScene::kNone;
	// ★ 壊れる床のリスト
	KamataEngine::Model* breakableBlockModel_ = nullptr; // 壊れる床のモデル
	std::vector<std::unique_ptr<BreakableFloor>> breakableFloors_;

	CountdownState countdownState_ = CountdownState::kOff;
	float countdownTimer_ = 0.0f;

	// カウントダウン表示用のスプライトハンドル
	uint32_t textureHandleCountdown3_ = 0;
	uint32_t textureHandleCountdown2_ = 0;
	uint32_t textureHandleCountdown1_ = 0;
	uint32_t textureHandleCountdownGo_ = 0;

	bool firstFrame;

	KamataEngine::Sprite* spriteCountdown_ = nullptr;

};
