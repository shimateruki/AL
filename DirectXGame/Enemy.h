#pragma once // 同じヘッダーファイルが複数回インクルードされるのを防ぐ
#include "KamataEngine.h" // KamataEngineの基本機能（Model, WorldTransform, Cameraなど）をインクルード
#include "math.h"         // 数学関連のユーティリティ（Mathクラスや関連関数）をインクルード
#include "MapChipField.h" // MapChipFieldクラスの定義をインクルード


using namespace KamataEngine; // KamataEngine名前空間を使用

// 前方宣言
class GameScene; // GameSceneクラスの前方宣言
class Player;
class GameScene1_2; // GameScene1_2クラスの前方宣言
class GameScene1_3;
    // 敵クラス 
class Enemy {

public:
	// 敵の行動状態を表す列挙型
	enum class Behavior {
		kUnKnow,
		kRoot,
		kisDead, // 死亡状態

	};

	// model: 敵のモデル、camera: カメラ、position: 初期位置
	void Initialize(Model* model, Camera* camera, const Vector3& position);

	// 更新処理 
	void Update();

	// 描画処理 
	void Draw();

	Vector3 GetWorldPosition();
	bool GetIsDead(){ return isDead_; } // 敵が死亡しているかどうかを取得するメソッド
	AABB GetAABB();
	void onCollision(const Player* player);
	void OnStomped(const Player* player);
	bool IsWalkable(MapChipType type);
	bool isCollisonDisabled() const { return isCollisDisabled_; } // 衝突無効化フラグを取得
	void SetCollisonDisabled(bool disabled) { isCollisDisabled_ = disabled; }          // 衝突無効化フラグを設定
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; } // ゲームシーンへのポインタを設定
	void SetGameScene1_2(GameScene1_2* GameScene1_2) { gameScene1_2_ = GameScene1_2; } // GameScene1_2へのポインタを設定
	void SetGameScene1_3(GameScene1_3* GameScene1_3) { gameScene1_3_ = GameScene1_3; } // GameScene1_2へのポインタを設定
	bool isDead() const { return isDead_; }  
	  void SetMapChipField(MapChipField* field) { mapChipField_ = field; }
                                          // 敵が死亡しているかどうかを取得するメソッド


private:
	// 敵のワールド変換 
	WorldTransform worldTransformEnemy_;
	MapChipField* mapChipField_ = nullptr;

	    // マップとの当たり判定結果
	struct CollisionMapInfo {
		Vector3 isMovement;
		bool isHitTop = false;
		bool isHitBottom = false;
		bool hitWall = false;
	};

	// プレイヤーと同様の、四隅の座標計算用ユーティリティ
	enum Corner {
		kRightBottom,
		kLeftBottom,
		kRightTop,
		kLeftTop,
	};
	static const uint32_t knumCorner = 4;
	Vector3 CarnerPosition(const Vector3& center, Corner cornter);

	// 各方向のマップチップとの当たり判定
	void MapChipUp(CollisionMapInfo& info);
	void MapChipDown(CollisionMapInfo& info);
	void MapChipLeft(CollisionMapInfo& info);
	void MapChipRight(CollisionMapInfo& info);

	// 状態更新
	void UpdateOnGround(const CollisionMapInfo& info);
	void UpdateOnWall(const CollisionMapInfo& info);

	// 状態フラグ
	bool onGround_ = false; // 接地しているか

	// 定数（幅と高さ）
	const float kWidth = 1.0f;
	const float kHeight = 0.8f;
	const float kBlank = 0.1f;
	const float kGroundSearchHeight = 0.1f;

	// 敵のモデル 
	Model* model_ = nullptr;

	// カメラへのポインタ 
	Camera* camera_ = nullptr;

	// 歩行速度 
	static inline const float kWalkSpeed = 0.02f;

	// 敵の速度
	Vector3 velocity_ = {};

	// 歩行モーションの開始角度 
	static inline const float kWalkMotionAngleStart = 0.0f;

	// 歩行モーションの終了角度 
	static inline const float kWalkMotionAngleEnd = 30.0f;

	// 歩行モーションの時間 
	static inline const float kWalkMotionTime = 1.0f;

	// 歩行タイマー
	float walkTimer = 0.0f;

	// 数学ユーティリティオブジェクト
	Math* math = nullptr;
	bool isDead_ = false;       // 敵が死亡しているかどうかのフラグ
	                            // 振る舞い
	Behavior behavior_ = Behavior::kRoot;
	// 次の振る舞いのリクエスト
	Behavior behaviorRequest_ = Behavior::kUnKnow;

	bool isCollisDisabled_ = false; // 衝突無効化フラグ（敵が死亡しているかどうかを示す）
	GameScene* gameScene_ = nullptr; // ゲームシーンへのポインタ（必要に応じて使用）
	GameScene1_2* gameScene1_2_ = nullptr; // GameScene1_2へのポインタ（必要に応じて使用）
	GameScene1_3* gameScene1_3_ = nullptr; // GameScene1_3へのポインタ（必要に応じて使用）

    float flipCooldownTimer = 0.0f;
	const float kFlipCooldown = 2.0f; // 0.5秒間は反転しない
	bool animationPlaying = false;    // 死亡アニメーション中かどうかのフラグ
};