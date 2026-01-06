#pragma once // 同じヘッダーファイルが複数回インクルードされるのを防ぐ

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
#include"CloudPlatform.h"
#include "yama.h"
#include"tree.h"
#include "BreakableFloor.h"
#include "ParticleManager.h"

// ===== ゲームシーンクラス定義 =====
class GameScene {
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

	// ポーズ画面の各選択肢
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
	void Initialize(int stageID);
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
	void LimitPlayerPosition();                   // 画面外に出ない制限
	int currentSelectIndex() const { return currentSelectIndex_; } // 現在の選択肢インデックスを取得
	NextScene GetNextScene() const { return nextScene_; }

	~GameScene(); // デストラクタ

private:
	//ポーズ関連のメンバ変数
	bool isPaused_ = false;
	PauseSelect currentSelect_ = PauseSelect::kContinue;
	int currentSelectIndex_ = 0; // 現在の選択肢インデックス
	// ===== モデル関連 =====
	uint32_t textureHandel_ = 0;                        // テクスチャハンドル
	KamataEngine::Model* dirtModel_ = nullptr;         // ブロックのモデル
	KamataEngine::Model* grassModel_ = nullptr;         // 草ブロックのモデル
	KamataEngine::Model* playerModel_ = nullptr;        // プレイヤーのモデル
	KamataEngine::Model* playerAttackModel_ = nullptr;  // プレイヤー攻撃モデル
	KamataEngine::Model* goalModel_ = nullptr;          // ゴールモデル
	KamataEngine::Model* modelSkydome_ = nullptr;       // スカイドームモデル
	KamataEngine::Model* deatparticlesModel_ = nullptr; // デスパーティクルモデル
	KamataEngine::Model* hitEffectModel_ = nullptr;     // ヒットエフェクトモデル
	KamataEngine::Model* GameClearTextModel_ = nullptr; // ゲームクリアテキストモデル
	KamataEngine::Model* togeKabeModel_ = nullptr;      // トゲ壁モデル
	KamataEngine::Model* togeModel_ = nullptr;          // トゲモデル
	KamataEngine::Model* CloudPlatformModel_ = nullptr; // 雲プラットフォームモデル
	KamataEngine::Model* yamaModel = nullptr;           // 山モデル
	KamataEngine::Model* treeModel_ = nullptr;          // 木モデル
	KamataEngine::Model* breakableBlockModel_ = nullptr; // 破壊可能ブロックモデル
	KamataEngine::Model* iceBlockModel_ = nullptr;       // 氷ブロックモデル
	KamataEngine::Model* kinokoModel_ = nullptr;         // キノコモデル
	KamataEngine::Model* enemy_model_Walk = nullptr;     // 敵モデル
	KamataEngine::Model* enemy_model_Shooter = nullptr; // 敵のモデル(弾打つ敵)
	KamataEngine::Model* enemy_model_Homing = nullptr; // 敵のモデル(ホーミングして爆発する敵)
	KamataEngine::Model* enemy_model_Fly = nullptr;//敵のモデル
	KamataEngine::Model* umbrellaModel_ = nullptr;     // 傘モデル
	KamataEngine::Model* hasigoModel_ = nullptr; // はしごモデル
	KamataEngine::Model* kumoModel_ = nullptr;         // 雲モデル
	KamataEngine::Model* iwaModel_ = nullptr;    // 岩モデル
	Model* starCoinModel_ = nullptr;
	Model* particleModel_ = nullptr; // パーティクル用の汎用モデル（球や板ポリ）
	KamataEngine::Sprite* TextSprite1_1;
	KamataEngine::Sprite* poseSprite = nullptr; // 数字表示用スプライト
	KamataEngine::Sprite* yazirusiSprite = nullptr;
	KamataEngine::Sprite* enterSprite_ = nullptr; // エンターキー用スプライト
	KamataEngine::Sprite* GameClearTextSprite_ = nullptr; // ゲームクリアテキスト用スプライト
	KamataEngine::Sprite* pauseTextSprite_ = nullptr;  
		// 各ステージ画像のスプライト
	KamataEngine::Sprite* sprite1_1_ = nullptr;
	KamataEngine::Sprite* sprite1_2_ = nullptr;
	KamataEngine::Sprite* sprite1_3_ = nullptr;

	std::vector<std::unique_ptr<BreakableFloor>> breakableFloors_;

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
	CloudPlatform* cloudPlatform_ = nullptr; // 雲プラットフォーム
	std::vector<Enemy*> enemys_;
	std::vector<Yama*> yama_;                // 山リスト
	std::vector<Tree*> tree_;                // 木リスト
	std::list<std::unique_ptr<WorldTransform>> starCoins_; // 配置されたコインのリスト
	
		ParticleManager* particleManager_ = nullptr;


	std::vector<CloudPlatform*> CloudPlatform; // 雲リスト

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
	uint32_t clearbgmHandle_ = 0;      // BGMデータのハンドル
	uint32_t clearbgmVoiceHandle_ = 0; // BGM再生のハンドル

	uint32_t textureHandle;
	uint32_t textureHandle1_1_ = 0;
	uint32_t textureHandle1_2_ = 0;
	uint32_t textureHandle1_3_ = 0;
	uint32_t textureHandlePhose_ = 0; 
	uint32_t TextureHandleYazirusi_ = 0;
	uint32_t textureHandleEnter_ = 0; // テクスチャハンドル
	uint32_t textureHandleGameClearText_ = 0; // ゲームクリアテキスト用テクスチャハンドル
	uint32_t textureHandlePauseText_ = 0;     // ポーズテキスト用テクスチャハンドル

	   NextScene nextScene_ = NextScene::kNone;
	bool isSprite;
	bool firstFrame;

		CountdownState countdownState_ = CountdownState::kOff;
	float countdownTimer_ = 0.0f;
	    // スターコイン関連のメンバ変数
	    uint32_t texHandleCoinEmpty_ = 0; // 未取得
	    uint32_t texHandleCoinGet_ = 0;   // 取得済み

	    // コインのUIスプライト（3枚分）
	    KamataEngine::Sprite* uiStarCoins_[3] = {nullptr};

	    // 現在の獲得枚数
	    int currentStarCoinCount_ = 0;


	// カウントダウン表示用のスプライトハンドル
	uint32_t textureHandleCountdown3_ = 0;
	uint32_t textureHandleCountdown2_ = 0;
	uint32_t textureHandleCountdown1_ = 0;
	uint32_t textureHandleCountdownGo_ = 0;

	//----------------------------------------
	// HPアイコン
	//----------------------------------------
	uint32_t textureHandleHpIconNormal_ = 0;
	uint32_t textureHandleHpIconDamage_ = 0;
	KamataEngine::Sprite* spriteHpIconNormal_ = nullptr;
	KamataEngine::Sprite* spriteHpIconDamage_ = nullptr;

	//----------------------------------------
	// HPハート
	//----------------------------------------
	uint32_t textureHandleHeart_ = 0; // 満タンのハートの画像

	// HPの最大値（Player.cppのhp_ = 3; と合わせる）
	static const int kMaxPlayerHp = 3;

	// 満タンハートのスプライトだけを持つ
	std::vector<KamataEngine::Sprite*> spriteHearts_;

	//BGM
	uint32_t bgmHandle;
	uint32_t bgmVoiceHandle;

	//SE
	uint32_t SeHandle;
	uint32_t seVoiceHandle;
	uint32_t consorlSelectHandle;
	uint32_t consorlVoiceSelectHandle;


	KamataEngine::Sprite* spriteCountdown_ = nullptr;
	// ：敵配置用の関数
	void SpawnEnemies(int stageID);

	// 現在のステージ番号を覚えておく
	int currentStageID_ = 1;
	// 現在のプレイで取った枚数
	int currentPlayCoinCount_ = 0;
};