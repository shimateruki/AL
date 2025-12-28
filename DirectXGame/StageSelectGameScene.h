#pragma once
#include "KamataEngine.h"
#include "math.h"
#include "Fade.h"
#include"Player.h"
#include "MapChipField.h"
#include"CameraController.h"
#include"skydome.h"
#include "Signboard.h"
#include"StageSelectPlayer.h"
#include"SpriteText.h"
#include "keyModel.h"
#include"yama.h"
#include"tree.h"
class StageSelectGameScene 
{
public:
	// ===== ゲーム進行フェーズ =====
	enum class Phase {
		kFadeIn,  // フェードイン中
		kPlay,    // ゲームプレイ中
		kFadeOut, // フェードアウト中
	};

	// 初期化・更新・描画
	void Initialize();
	void Update();
	void Draw();
	// フェーズ切り替え
	void ChangePhase();
	// ブロック生成処理
	void GenerateBlocks();
	void CheekAllcollision(); // 当たり判定処理 
	// ゲーム終了フラグ取得
	bool isFinished() const { return finished_; }
	void LimitPlayerPosition(); 
	    int GetNextStageID() const { return nextStageID_; }
	~StageSelectGameScene(); // デストラクタ
private:
	// ===== モデル関連 =====
	KamataEngine::Model* dirtModel_ = nullptr;          // ブロックのモデル
	KamataEngine::Model* grassModel_ = nullptr;         // 草ブロックのモデル
	KamataEngine::Model* playerModel_ = nullptr;        // プレイヤーのモデル
	KamataEngine::Model* playerAttackModel_ = nullptr;  // プレイヤー攻撃モデル
	KamataEngine::Model* modelSkydome_ = nullptr;       // スカイドームモデル
	KamataEngine::Model* signboardModel_ = nullptr;     // 看板モデル
	KamataEngine::Model* treeModel_ = nullptr;          // 木モデル

	KamataEngine::Model* Textmodel1_1 = nullptr;  
	KamataEngine::Model* Textmodel1_2 = nullptr;       
	KamataEngine::Model* Textmodel1_3 = nullptr; 
	KamataEngine::Model* TitleTextModel;
	KamataEngine::Model* WModel = nullptr; // Wモデル
	KamataEngine::Model* keyHeimenModel = nullptr; // キーモデル
	KamataEngine::Model* yamaModel = nullptr;      // 山モデル
	KamataEngine::Model* umbrellaModel_ = nullptr; // 傘モデル

	KamataEngine::Sprite* Sprite1_1 = nullptr; // 数字表示用スプライト
	KamataEngine::Sprite* Sprite1_2 = nullptr; // 数字表示用スプライト
	KamataEngine::Sprite* Sprite1_3 = nullptr; // 数字表示用スプライト
	KamataEngine::Sprite* SpriteMove = nullptr; // 移動用スプライト
	KamataEngine::Sprite* SpriteJump = nullptr; // ジャンプ用スプライト
	KamataEngine::Sprite* SpriteSutage = nullptr; // ステージ用スプライト
	KamataEngine::Sprite* FKeysBulletText = nullptr;//Fキーで弾発射を伝えるbullets





	//========================
	// 🎥 カメラの設定
	//========================
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
	Player* player_ = nullptr; // プレイヤー
	StageSelectPlayer* stageSelectPlayer_ = nullptr; // ステージ選択用プレイヤー
	MapChipField* mapChipField_ = nullptr; // マップチップ
	Skydome* skydome_ = nullptr;           // スカイドーム
	Math* math = nullptr;                  // 数学ユーティリティ
	std::vector<Signboard*> signboards_;   // 看板リスト
	std::vector<SpriteText*> spriteTexts_; // スプライトテキストリスト
	std::vector<keyModel*> keySprite_; // スプライトテキストリスト
	std::vector<Yama*> yama_;        // 山リスト
	std::vector<Tree*> tree_;          // 木リスト

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
	// ===== ステージ関連 =====
	int nextStageID_;             // 次のステージID

	 bool firstFrame;
	 
	 int32_t textureHandel1_1 = 0; // テクスチャハンドル
	 int32_t textureHandel1_2 = 0; // テクスチャハンドル
	 int32_t textureHandel1_3 = 0; // テクスチャハンドル
	 int32_t textureHandleMove = 0;    // テクスチャハンドル
	 int32_t textureHandleJump = 0;  // テクスチャハンドル
	 int32_t textureHandleSutage = 0;     // テクスチャハンドル
	 int32_t textureHandleBullet =  0;

	   // 現在アクティブなスプライトを追跡するためのポインタ
	 Sprite* activeSprite_ = nullptr;

	 
		bool isSprite;

	    KamataEngine::Model* starCoinModel_ = nullptr; // コインのモデル

	    // コインを表示するためのリスト（場所などのデータ）
	    std::list<std::unique_ptr<KamataEngine::WorldTransform>> uiDisplayCoins_;

	    // 看板のリストを見てコインを配置する関数
	    void SetupDisplayCoins();
	    bool isCoinsSetup_ = false;

};
