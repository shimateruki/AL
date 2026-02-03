#pragma once // インクルードガード

// ==========================================
// 必要なヘッダーファイルのインクルード
// ==========================================
// --- エンジン・基本機能 ---
#include "KamataEngine.h"
#include "math.h"

// --- ゲームオブジェクト・アクター ---
#include "MapChipField.h"
#include "Player.h"
#include "Signboard.h"
#include "StageSelectPlayer.h" // ステージセレクト用プレイヤー
#include "keyModel.h"
#include "skydome.h"
#include "tree.h"
#include "yama.h"

// --- システム・UI・演出 ---
#include "CameraController.h"
#include "Fade.h"
#include "ParticleManager.h"
#include "SpriteText.h"

// ==========================================
// ステージセレクトシーンクラス定義
// ==========================================
class StageSelectGameScene {
public:
	// -------------------------------------------------
	// 定数・列挙型
	// -------------------------------------------------
	// ゲーム進行フェーズ
	enum class Phase {
		kFadeIn,  // フェードイン（開始）
		kPlay,    // プレイ中（ステージ選択操作）
		kFadeOut, // フェードアウト（決定後）
	};

	// -------------------------------------------------
	// 主要メソッド (Core Methods)
	// -------------------------------------------------
	void Initialize();       // 初期化
	void Update();           // 更新
	void Draw();             // 描画
	~StageSelectGameScene(); // デストラクタ

	// -------------------------------------------------
	// ロジック・制御 (Logic)
	// -------------------------------------------------
	void ChangePhase();         // フェーズ遷移管理
	void GenerateBlocks();      // 地形ブロック生成
	void CheekAllcollision();   // 当たり判定処理
	void LimitPlayerPosition(); // プレイヤー移動制限

	// 看板上のコイン表示セットアップ
	void SetupDisplayCoins();

	// -------------------------------------------------
	// ゲッター (Getters)
	// -------------------------------------------------
	bool isFinished() const { return finished_; }       // シーン終了判定
	int GetNextStageID() const { return nextStageID_; } // 選択されたステージID取得

private:
	// ==========================================
	// メンバ変数
	// ==========================================

	// -------------------------------------------------
	// システム制御・状態管理
	// -------------------------------------------------
	Phase phase_ = Phase::kFadeIn; // 現在のフェーズ
	Fade* fade_ = nullptr;         // フェード演出クラス

	bool finished_ = false;        // シーン終了フラグ
	bool isSceneFinished_ = false; // フェーズ完了フラグ
	bool isGameClear_ = false;     // （未使用？）
	bool isTimerFinished_ = false; // タイマー判定
	int finishedTimer = 0;         // 終了演出用タイマー

	int nextStageID_ = 0; // 次に遷移するステージID
	bool firstFrame;      // 初回フレーム判定
	bool isSprite;        // スプライト描画フラグ

	// コイン表示セットアップ済みフラグ
	bool isCoinsSetup_ = false;

	// -------------------------------------------------
	// カメラ (Camera)
	// -------------------------------------------------
	KamataEngine::Camera camera_;             // メインカメラ
	CameraController* CController_ = nullptr; // カメラ制御

	// デバッグカメラ
	bool isDebugCameraActive_ = false;
	KamataEngine::DebugCamera* debaucamera_ = nullptr;

	// -------------------------------------------------
	// ワールド変換 (Transform)
	// -------------------------------------------------
	KamataEngine::WorldTransform worldTransform_;                                  // シーン全体基準
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_; // 地形用
	KamataEngine::WorldTransform GameClearTextWorldTransform_;                     // テキスト位置

	// -------------------------------------------------
	// ゲームオブジェクト (Game Objects)
	// -------------------------------------------------
	// プレイヤー関連
	Player* player_ = nullptr;                       // (汎用プレイヤー)
	StageSelectPlayer* stageSelectPlayer_ = nullptr; // ステージ選択専用プレイヤー

	// 背景・マップ
	MapChipField* mapChipField_ = nullptr; // マップデータ
	Skydome* skydome_ = nullptr;           // 天球
	Math* math = nullptr;                  // 数学ヘルパー

	// 配置オブジェクト
	std::vector<Signboard*> signboards_;   // 看板（ステージ入り口）
	std::vector<SpriteText*> spriteTexts_; // テキストオブジェクト
	std::vector<keyModel*> keySprite_;     // キー表示モデル
	std::vector<Yama*> yama_;              // 山
	std::vector<Tree*> tree_;              // 木

	// コイン演出用リスト
	std::list<std::unique_ptr<KamataEngine::WorldTransform>> uiDisplayCoins_;

	// パーティクル
	ParticleManager* particleManager_ = nullptr;

	// -------------------------------------------------
	// 3Dモデルリソース (Models)
	// -------------------------------------------------
	// 環境・地形
	KamataEngine::Model* dirtModel_ = nullptr;    // 土
	KamataEngine::Model* grassModel_ = nullptr;   // 草
	KamataEngine::Model* treeModel_ = nullptr;    // 木
	KamataEngine::Model* hasigoModel_ = nullptr;  // 梯子
	KamataEngine::Model* kumoModel_ = nullptr;    // 雲
	KamataEngine::Model* iwaModel_ = nullptr;     // 岩
	KamataEngine::Model* kinokoModel_ = nullptr;  // キノコ
	KamataEngine::Model* yamaModel = nullptr;     // 山
	KamataEngine::Model* modelSkydome_ = nullptr; // スカイドーム

	// キャラクター・アイテム
	KamataEngine::Model* playerModel_ = nullptr;       // プレイヤー
	KamataEngine::Model* playerAttackModel_ = nullptr; // 攻撃エフェクト
	KamataEngine::Model* umbrellaModel_ = nullptr;     // 傘
	KamataEngine::Model* starCoinModel_ = nullptr;     // スターコイン
	KamataEngine::Model* particleModel_ = nullptr;     // 汎用パーティクル

	// UI・看板・テキスト（3Dモデルで表示するもの）
	KamataEngine::Model* signboardModel_ = nullptr; // 看板
	KamataEngine::Model* TitleTextModel = nullptr;  // タイトルロゴ
	KamataEngine::Model* WModel = nullptr;          // 'W'キー
	KamataEngine::Model* keyHeimenModel = nullptr;  // キー平面

	// ステージ番号テキストモデル
	KamataEngine::Model* Textmodel1_1 = nullptr;
	KamataEngine::Model* Textmodel1_2 = nullptr;
	KamataEngine::Model* Textmodel1_3 = nullptr;
	KamataEngine::Model* Textmodel1_4 = nullptr;
	KamataEngine::Model* Textmodel1_5 = nullptr;

	// -------------------------------------------------
	// 2Dスプライト・UI (Sprites)
	// -------------------------------------------------
	// 操作ガイド・装飾
	KamataEngine::Sprite* SpriteMove = nullptr;      // 移動操作ガイド
	KamataEngine::Sprite* SpriteJump = nullptr;      // ジャンプ操作ガイド
	KamataEngine::Sprite* SpriteSutage = nullptr;    // "Stage" 文字
	KamataEngine::Sprite* FKeysBulletText = nullptr; // "F Key" ガイド

	// ステージ番号スプライト
	KamataEngine::Sprite* Sprite1_1 = nullptr;
	KamataEngine::Sprite* Sprite1_2 = nullptr;
	KamataEngine::Sprite* Sprite1_3 = nullptr;
	KamataEngine::Sprite* Sprite1_4 = nullptr;
	KamataEngine::Sprite* Sprite1_5 = nullptr;

	// アクティブ状態管理
	KamataEngine::Sprite* activeSprite_ = nullptr;

	// スターコインUI（所持数表示用）
	bool isShowCoinUI_ = false;
	KamataEngine::Sprite* uiStarCoins_[3] = {nullptr};

	// -------------------------------------------------
	// テクスチャハンドル (Textures)
	// -------------------------------------------------
	// ステージ番号
	int32_t textureHandel1_1 = 0;
	int32_t textureHandel1_2 = 0;
	int32_t textureHandel1_3 = 0;
	int32_t textureHandel1_4 = 0;
	int32_t textureHandel1_5 = 0;

	// UIアイコン・文字
	int32_t textureHandleMove = 0;   // 移動
	int32_t textureHandleJump = 0;   // ジャンプ
	int32_t textureHandleSutage = 0; // ステージ
	int32_t textureHandleBullet = 0; // 弾発射

	// コインUI画像
	uint32_t texHandleCoinEmpty_ = 0; // 未取得
	uint32_t texHandleCoinGet_ = 0;   // 取得済み

	// -------------------------------------------------
	// オーディオ (Audio)
	// -------------------------------------------------
	uint32_t bgmHandle_ = 0;      // BGMデータ
	uint32_t bgmVoiceHandle_ = 0; // BGM再生ハンドル
};