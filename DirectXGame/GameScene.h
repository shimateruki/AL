#pragma once // 同じヘッダーファイルが複数回インクルードされるのを防ぐ

// 必要なヘッダーファイルをインクルード
#include "Block.h"        // Blockクラスの定義
#include "KamataEngine.h" // KamataEngineの基本機能（Model, WorldTransform, Cameraなど）
#include "Player.h"       // Playerクラスの定義
#include "math.h"         // 数学関連のユーティリティ（おそらくMathクラスや関連関数）

#include "CameraController.h" // CameraControllerクラスの定義
#include "Enemy.h"            // Enemyクラスの定義 (02_09 10枚目)
#include "MapChipField.h"     // MapChipFieldクラスの定義
#include "skydome.h"          // Skydomeクラスの定義

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
	KamataEngine::Model* modelSkydome_ = nullptr;                                  // スカイドームのモデル

	// エネミークラス
	Enemy* enemy_ = nullptr; // 敵オブジェクト
	// エネミーモデル
	KamataEngine::Model* enemy_model_ = nullptr; // 敵のモデル

	// マップチップフィールド
	MapChipField* mapChipField_ = nullptr; // マップチップフィールドオブジェクト

	CameraController* CController_ = nullptr; // カメラコントローラーオブジェクト

	Math* math = nullptr; // 数学ユーティリティオブジェクト（Mathクラスのインスタンス）
};