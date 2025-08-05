#pragma once // 同じヘッダーファイルが複数回インクルードされるのを防ぐ

// 必要なヘッダーファイルをインクルード
#include "Block.h"            // Blockクラスの定義
#include "CameraController.h" // CameraControllerクラスの定義
#include "DeathParticles.h"
#include "Enemy.h" // Enemyクラスの定義 (02_09 10枚目)
#include "Fade.h"
#include "HitEffect.h"
#include "KamataEngine.h" // KamataEngineの基本機能（Model, WorldTransform, Cameraなど）
#include "MapChipField.h" // MapChipFieldクラスの定義
#include "Player.h"       // Playerクラスの定義
#include "gorl.h"
#include "math.h"    // 数学関連のユーティリティ（おそらくMathクラスや関連関数）
#include "skydome.h" // Skydomeクラスの定義

enum class Phase {

	kFadeIn,
	kPlay,  // ゲームプレイ
	kDeath, // デス演出
	kFadeOut,
	GameClear, // ゲームクリア
};

// ゲームシーンクラス
class GameScene {

public:
	// 初期化処理
	void Initialize();

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// ブロック生成処理
	void GenerrateBlock();

	// すべてのあたり判定を行う
	void CheakAllcollision();

	// フェーズの切り替え
	void ChangePhase();

	bool isFinished() const { return finished_; }
	// エフェクトの生成
	void CreateHitEffect(const KamataEngine::Vector3& position);

	// デストラクタ
	~GameScene();

private:
	uint32_t textureHandel_ = 0;                  // テクスチャハンドル
	KamataEngine::Model* blockModel_ = nullptr;   // ブロックのモデル
	KamataEngine::WorldTransform worldTransform_; // ワールド変換（おそらく単一のオブジェクト用、現状未使用の可能性あり）
	KamataEngine::Camera camera_;                 // ゲーム内メインカメラ
	Player* player_ = nullptr;                    // プレイヤーオブジェクト
	bool isDebugCameraActive_ = false;            // デバッグカメラがアクティブかどうかを示すフラグ
	// デバッグカメラ
	KamataEngine::DebugCamera* debaucamera_ = nullptr; // デバッグカメラオブジェクト

	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_; // マップ内のブロックのワールド変換を格納する2Dベクター
	Skydome* skydome_ = nullptr;                                                   // スカイドームオブジェクト
	KamataEngine::Model* playerModel_ = nullptr;                                   // プレイヤーのモデル
	KamataEngine::Model* modelSkydome_ = nullptr;
	KamataEngine::Model* playerAttackModel_ = nullptr;
	KamataEngine::Model* goalModel_ = nullptr; // ゴールのモデル

	Gorl* gorl_ = nullptr; // gorlクラスのインスタンス

	// エネミークラス
	std::list<Enemy*> enemys_; // 敵オブジェクト
	// エネミーモデル
	KamataEngine::Model* enemy_model_ = nullptr; // 敵のモデル
	const int kEnemyMax = 3;

	DeathParticles* deatparticles_ = nullptr;
	KamataEngine::Model* deatparticlesModel_ = nullptr;

	// マップチップフィールド
	MapChipField* mapChipField_ = nullptr; // マップチップフィールドオブジェクト

	CameraController* CController_ = nullptr; // カメラコントローラーオブジェクト

	Math* math = nullptr; // 数学ユーティリティオブジェクト（Mathクラスのインスタンス）

	// ヒットした時のモデル
	std::list<HitEffect*> hitEffects_;              // ヒットエフェクトオブジェクト
	KamataEngine::Model* hitEffectModel_ = nullptr; // ヒットエフェクトのモデル
	// エフェクト最大数
	const int HitEffectMax = 10; // ヒットエフェクトの最大数

	KamataEngine::Model* GameClearTextModel_;
	KamataEngine::WorldTransform GameClearTextWorldTransform_; // 文字モデルのワールド変

	// 終了フラグ
	bool finished_ = false;
	Phase phase_ = Phase::kFadeIn; // 現在のゲームシーンのフェーズ
	Fade* fade_ = nullptr;         // フェードオブジェクトのポインタ
	bool isSceneFinished_ = false; // シーン終了判定フラグ
	int finishedTimer = 0;         // kDeathフェーズで使っていたタイマー
	bool isGameClear_ = false;     // ゲームクリアフラグ
	bool isTimerFinished_ = false; // タイマーが終了したかどうかのフラグ
	int loadAudioHandle_ = 0;      // オーディオハンドル(曲の再生)
};
