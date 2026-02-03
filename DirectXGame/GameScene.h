#pragma once // インクルードガード：同じヘッダーが複数回読み込まれるのを防ぐ

// ==========================================
// 必要なヘッダーファイルのインクルード
// ==========================================
// --- エンジン・基本機能 ---
#include "KamataEngine.h"
#include "math.h"

// --- ゲームオブジェクト・アクター ---
#include "Block.h"
#include "Enemy.h"
#include "MapChipField.h"
#include "Player.h"
#include "gorl.h"    // ゴール
#include "skydome.h" // スカイドーム

// --- マップギミック・背景 ---
#include "BreakableFloor.h"
#include "CloudPlatform.h"
#include "KabeToge.h"
#include "Toge.h"
#include "tree.h"
#include "yama.h"

// --- システム・演出 ---
#include "CameraController.h"
#include "DeathParticles.h"
#include "Fade.h"
#include "HitEffect.h"
#include "ParticleManager.h"

// ==========================================
// ゲームシーンクラス定義
// ==========================================
class GameScene {
public:
	// -------------------------------------------------
	// 定数・列挙型 (State / Enum)
	// -------------------------------------------------

	// ゲーム進行フェーズ
	enum class Phase {
		kFadeIn,       // フェードイン（開始演出）
		kCountdown,    // カウントダウン
		kPlay,         // ゲームプレイ中
		kBossEntrance, // ボス登場演出
		kDeath,        // 死亡演出中
		kFadeOut,      // フェードアウト（終了演出）
		GameClear      // ゲームクリア
	};

	// ポーズ画面の選択肢
	enum class PauseSelect {
		kContinue,    // ゲーム再開
		kStageSelect, // ステージ選択へ
		kTitle,       // タイトルへ
	};

	// カウントダウンの状態
	enum class CountdownState {
		kOff,      // 無効
		kCounting, // カウント中
		kFinished  // 完了
	};

	// 次に遷移するシーン
	enum class NextScene { kGameOver, kStageSelect, kNone };

	// -------------------------------------------------
	// 主要メソッド (Core Methods)
	// -------------------------------------------------

	// 初期化・更新・描画
	void Initialize(int stageID); // 指定ステージで初期化
	void Update();                // 毎フレームの更新処理
	void Draw();                  // 描画処理
	~GameScene();                 // デストラクタ

	// -------------------------------------------------
	// ゲームロジック・制御 (Game Logic)
	// -------------------------------------------------

	// --- 生成・配置関連 ---
	void GenerrateBlock();                                            // ブロック生成（単体？）
	void GenerateBlocks();                                            // ブロック群の生成
	void SpawnEnemies(int stageID);                                   // 敵の配置
	void AddEnemy(Enemy* enemy) { pendingEnemies_.push_back(enemy); } // 敵リストへの追加待機

	// --- 判定・状態遷移 ---
	void CheekAllcollision();   // 全オブジェクトの当たり判定処理
	void ChangePhase();         // フェーズの切り替え管理
	void LimitPlayerPosition(); // プレイヤーの画面外移動制限

	// --- 演出関連 ---
	void CreateHitEffect(const KamataEngine::Vector3& position); // ヒットエフェクト生成

	// -------------------------------------------------
	// ゲッター (Getters)
	// -------------------------------------------------
	bool isFinished() const { return finished_; }                  // シーン終了フラグ
	NextScene GetNextScene() const { return nextScene_; }          // 次のシーン取得
	int currentSelectIndex() const { return currentSelectIndex_; } // 現在のポーズ選択肢

private:
	// ==========================================
	// メンバ変数
	// ==========================================

	// -------------------------------------------------
	// システム制御・状態管理
	// -------------------------------------------------
	Phase phase_ = Phase::kFadeIn;           // 現在のゲームフェーズ
	NextScene nextScene_ = NextScene::kNone; // 次のシーン
	bool finished_ = false;                  // シーン終了判定
	bool isSceneFinished_ = false;           // フェーズ遷移完了判定
	int finishedTimer = 0;                   // 終了演出用タイマー
	bool isGameClear_ = false;               // クリアフラグ
	bool isTimerFinished_ = false;           // タイマー完了フラグ

	// カウントダウン関連
	CountdownState countdownState_ = CountdownState::kOff;
	float countdownTimer_ = 0.0f;

	// ポーズ画面関連
	bool isPaused_ = false;
	PauseSelect currentSelect_ = PauseSelect::kContinue;
	int currentSelectIndex_ = 0;

	// フェード処理
	Fade* fade_ = nullptr;

	// ステージ管理
	int currentStageID_ = 1;
	bool firstFrame; // 初回フレーム判定用

	// -------------------------------------------------
	// カメラ (Camera)
	// -------------------------------------------------
	KamataEngine::Camera camera_;             // メインカメラ
	CameraController* CController_ = nullptr; // カメラ制御

	// デバッグカメラ
	bool isDebugCameraActive_ = false;
	KamataEngine::DebugCamera* debaucamera_ = nullptr;

	// カメラ演出用
	KamataEngine::Vector3 cameraOrgPos_; // 演出時の基準位置

	// -------------------------------------------------
	// ゲームオブジェクト (Game Objects)
	// -------------------------------------------------
	// プレイヤー・ゴール
	Player* player_ = nullptr;
	Gorl* gorl_ = nullptr; // ゴール

	// マップ・背景
	MapChipField* mapChipField_ = nullptr;
	Skydome* skydome_ = nullptr;
	Math* math = nullptr;

	// ギミック・障害物
	KabeToge* togeKabe_ = nullptr;                                 // 迫ってくる棘の壁
	Toge* toge_ = nullptr;                                         // 配置された棘
	CloudPlatform* cloudPlatform_ = nullptr;                       // 雲の足場
	std::vector<CloudPlatform*> CloudPlatform;                     // 雲リスト
	std::vector<std::unique_ptr<BreakableFloor>> breakableFloors_; // 壊れる床
	std::vector<Yama*> yama_;                                      // 山（背景）
	std::vector<Tree*> tree_;                                      // 木（背景）

	// 敵キャラクター
	std::vector<Enemy*> enemys_;       // アクティブな敵リスト
	std::list<Enemy*> pendingEnemies_; // 追加待ちの敵リスト

	// コイン・アイテム
	std::list<std::unique_ptr<KamataEngine::WorldTransform>> starCoins_; // スターコイン配置リスト
	int currentStarCoinCount_ = 0;                                       // 現在の所持数
	int currentPlayCoinCount_ = 0;                                       // プレイ中の取得数

	// -------------------------------------------------
	// パーティクル・エフェクト (Effects)
	// -------------------------------------------------
	ParticleManager* particleManager_ = nullptr;
	DeathParticles* deatparticles_ = nullptr; // 死亡時のパーティクル
	std::list<HitEffect*> hitEffects_;        // ヒットエフェクトリスト
	const int HitEffectMax = 10;              // エフェクト最大数

	// -------------------------------------------------
	// ボス戦関連 (Boss Battle)
	// -------------------------------------------------
	bool isBossActive_ = false;   // ボス稼働フラグ
	bool isBossDefeated_ = false; // 撃破フラグ
	float entranceTimer_ = 0.0f;  // 登場演出タイマー
	int bossMaxHp_ = 0;           // HP割合計算用
	bool bossHpInitialized_ = false;

	// -------------------------------------------------
	// 3Dモデルリソース (Models)
	// -------------------------------------------------
	// ※Init等でロードして使い回すモデルデータ
	KamataEngine::Model* dirtModel_ = nullptr;           // 土ブロック
	KamataEngine::Model* grassModel_ = nullptr;          // 草ブロック
	KamataEngine::Model* playerModel_ = nullptr;         // プレイヤー
	KamataEngine::Model* playerAttackModel_ = nullptr;   // 攻撃エフェクト
	KamataEngine::Model* goalModel_ = nullptr;           // ゴール
	KamataEngine::Model* modelSkydome_ = nullptr;        // 背景ドーム
	KamataEngine::Model* deatparticlesModel_ = nullptr;  // 死亡パーティクル
	KamataEngine::Model* hitEffectModel_ = nullptr;      // ヒットエフェクト
	KamataEngine::Model* GameClearTextModel_ = nullptr;  // クリア文字（3Dの場合）
	KamataEngine::Model* togeKabeModel_ = nullptr;       // トゲ壁
	KamataEngine::Model* togeModel_ = nullptr;           // トゲ
	KamataEngine::Model* CloudPlatformModel_ = nullptr;  // 雲足場
	KamataEngine::Model* yamaModel = nullptr;            // 山
	KamataEngine::Model* treeModel_ = nullptr;           // 木
	KamataEngine::Model* breakableBlockModel_ = nullptr; // 破壊可能ブロック
	KamataEngine::Model* iceBlockModel_ = nullptr;       // 氷ブロック
	KamataEngine::Model* kinokoModel_ = nullptr;         // キノコ

	// 敵モデル
	KamataEngine::Model* enemy_model_Walk = nullptr;    // 歩行タイプ
	KamataEngine::Model* enemy_model_Shooter = nullptr; // 射撃タイプ
	KamataEngine::Model* enemy_model_Homing = nullptr;  // 追尾タイプ
	KamataEngine::Model* enemy_model_Fly = nullptr;     // 飛行タイプ

	// 装飾・その他モデル
	KamataEngine::Model* umbrellaModel_ = nullptr; // 傘
	KamataEngine::Model* hasigoModel_ = nullptr;   // 梯子
	KamataEngine::Model* kumoModel_ = nullptr;     // 雲
	KamataEngine::Model* iwaModel_ = nullptr;      // 岩
	KamataEngine::Model* starCoinModel_ = nullptr; // コイン
	KamataEngine::Model* particleModel_ = nullptr; // 汎用パーティクル

	// -------------------------------------------------
	// 2Dスプライト・UI (Sprites)
	// -------------------------------------------------
	bool isSprite; // スプライト描画フラグ？

	// UI・テキスト表示用
	KamataEngine::Sprite* TextSprite1_1;
	KamataEngine::Sprite* poseSprite = nullptr;           // ポーズ画面
	KamataEngine::Sprite* yazirusiSprite = nullptr;       // 矢印カーソル
	KamataEngine::Sprite* enterSprite_ = nullptr;         // エンターキーガイド
	KamataEngine::Sprite* GameClearTextSprite_ = nullptr; // クリアテキスト
	KamataEngine::Sprite* pauseTextSprite_ = nullptr;     // ポーズテキスト
	KamataEngine::Sprite* spriteBossName_ = nullptr;      // ボス名表示
	KamataEngine::Sprite* spriteCountdown_ = nullptr;     // カウントダウン

	// ステージ数表示など
	KamataEngine::Sprite* sprite1_1_ = nullptr;
	KamataEngine::Sprite* sprite1_2_ = nullptr;
	KamataEngine::Sprite* sprite1_3_ = nullptr;

	// HP表示関連
	static const int kMaxPlayerHp = 3;
	KamataEngine::Sprite* spriteHpIconNormal_ = nullptr; // 通常アイコン
	KamataEngine::Sprite* spriteHpIconDamage_ = nullptr; // ダメージアイコン
	std::vector<KamataEngine::Sprite*> spriteHearts_;    // ハートリスト

	// ボスHPゲージ
	KamataEngine::Sprite* spriteHPGauge_ = nullptr; // 前面（赤）
	KamataEngine::Sprite* spriteHPBack_ = nullptr;  // 背景（黒）

	// コインUI
	KamataEngine::Sprite* uiStarCoins_[3] = {nullptr};

	// -------------------------------------------------
	// テクスチャハンドル (Textures)
	// -------------------------------------------------
	uint32_t textureHandel_ = 0; // ※変数名のTypo注意（Block用？）
	uint32_t textureHandle;      // 汎用

	// UI・フォント画像
	uint32_t textureHandle1_1_ = 0;
	uint32_t textureHandle1_2_ = 0;
	uint32_t textureHandle1_3_ = 0;
	uint32_t textureHandlePhose_ = 0;
	uint32_t TextureHandleYazirusi_ = 0;
	uint32_t textureHandleEnter_ = 0;
	uint32_t textureHandleGameClearText_ = 0;
	uint32_t textureHandlePauseText_ = 0;
	uint32_t texHandleBossName_ = 0; // ボス名

	// カウントダウン数字
	uint32_t textureHandleCountdown3_ = 0;
	uint32_t textureHandleCountdown2_ = 0;
	uint32_t textureHandleCountdown1_ = 0;
	uint32_t textureHandleCountdownGo_ = 0;

	// HP・アイテム関連
	uint32_t textureHandleHpIconNormal_ = 0;
	uint32_t textureHandleHpIconDamage_ = 0;
	uint32_t textureHandleHeart_ = 0;
	uint32_t texHandleWhite_ = 0;     // HPゲージ用白画像
	uint32_t texHandleCoinEmpty_ = 0; // コイン未取得
	uint32_t texHandleCoinGet_ = 0;   // コイン取得済み
	uint32_t textureHandleBossName_ = 0;

	// -------------------------------------------------
	// サウンドハンドル (Audio)
	// -------------------------------------------------
	// BGM
	uint32_t bgmHandle;
	uint32_t bgmVoiceHandle;
	uint32_t clearbgmHandle_ = 0;
	uint32_t clearbgmVoiceHandle_ = 0;

	// SE
	uint32_t SeHandle;
	uint32_t seVoiceHandle;
	uint32_t consorlSelectHandle;      // カーソル移動音
	uint32_t consorlVoiceSelectHandle; // 再生ハンドル

	// -------------------------------------------------
	// ワールドトランスフォーム (Transform)
	// -------------------------------------------------
	KamataEngine::WorldTransform worldTransform_;                                  // シーン基準
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_; // ブロック配置用
	KamataEngine::WorldTransform GameClearTextWorldTransform_;                     // クリアテキスト位置
};