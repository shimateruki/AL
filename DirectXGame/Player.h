#pragma once
#include "MapChipField.h" // マップチップフィールド
#include "Math.h"         // Mathユーティリティクラス


#include <array>   // std::array を使用
#include <numbers> // std::numbers::pi_v を使用
using namespace KamataEngine;

// マップチップとの当たり判定情報
struct CollisionMapInfo {
	Vector3 isMovement;       // 計算後の最終的な移動量 (先生のmoveに相当)
	bool isHitTop = false;    // 天井に当たったか (先生のceilingに相当)
	bool isHitBottom = false; // 地面に当たったか (先生のlandingに相当)
	bool hitWall = false;     // 左右の壁に当たったか (先生のhitWallに相当)
};

class Enemy;

class Player {
public:
	// 初期化
	void Initialize(Model* model, Camera* camera, const Vector3& position, KamataEngine::Model* modelAttack); // const &position に修正

	// 更新

	void Update();

	// 描画
	void Draw();

		// getter(02_06スライド11枚目で追加)
	const WorldTransform& GetWorldTransform() const { return worldTransformPlayer_; }

	// 02_06スライド28枚目で追加
	const Vector3& GetVelocity() const { return velosity_; }

	// 02_07 スライド4枚目
	void SetMapChipField(MapChipField* mapChipField) { mapchipField_ = mapChipField; }
	Vector3 GetWorldPosition();
	AABB GetAABB();
	void OnCollision(const Enemy* enemy);

	bool IsDead() const { return isDead_; }
	// player.h （修正後の宣言）
	WorldTransform& GetWorldTransform() { return worldTransformPlayer_; }

	//通常初期化
	void BehaviorRootInitialize();
	//攻撃行動初期化
	void BehaviorAtcckInitialize();
	bool GetIsAttack() const { return behavior_ == Behavior::kAttack; } // 攻撃中かどうかを取得
 
private:

	// 移動処理 (元のMove関数に、InputMoveのロジックを統合)
	void Move();

	void attckMove();

	// 衝突解決はUpdate内で直接行われるため、専用の関数は不要に（後述）

	// 各方向のマップチップとの当たり判定（先生のCheckMapCollisionUp/Down/Left/Rightに相当）
	void MapChipUp(CollisionMapInfo& info);
	void MapChipDown(CollisionMapInfo& info);
	void MapChipLeft(CollisionMapInfo& info);
	void MapChipRight(CollisionMapInfo& info);

	// 接地状態の更新 (先生のUpdateOnGroundに相当)
	void UpdateOnGround(const CollisionMapInfo& info);
	// 壁接触時の更新 (先生のUpdateOnWallに相当)
	void UpdateOnWall(const CollisionMapInfo& info);
	//通常行動更新
	void BehaviorRootUpdate();
	void BehaviorAttckUpdate();

	// キャラクターの四隅の座標を計算
	enum  Corner {
		kRightBottom,
		kLeftBottom,
		kRightTop,
		kLeftTop,
	};

	enum class Behavior {
		kUnKnow,
		kRoot,
		kAttack, 

	};
	enum class AttackPhase{

		reservoir,//溜め
		rush,
		reverberation//余韻

	};

	static const uint32_t knumCorner = 4;
	Vector3 CarnerPosition(const Vector3& center, Corner cornter);
	//振る舞い
	Behavior behavior_ = Behavior::kRoot;
	//次の振る舞いのリクエスト
	Behavior behaviorRequest_ = Behavior::kUnKnow;
	//現在の攻撃フェーズ
	AttackPhase attackPhase_; 


private:
	// プレイヤーのワールド変換
	WorldTransform worldTransformPlayer_;
	// モデル
	Model* model_ = nullptr;
	// カメラ
	Camera* camera_ = nullptr;

	// 数学ユーティリティ
	Math* math = nullptr; // Initialize()で適切に初期化すること

	// 速度
	Vector3 velosity_ = {};
	Vector3 attckVelosity{};

	// 定数
	const float kAcceleration = 0.5f;         // 加速度
	const float kAtteunuation = 0.2f;         // 減衰率 (走行時の摩擦)
	const float kAttenuationLanding = 0.8f;   // 着地時の減衰率 (新しく追加)
	const float kAttenuationWall = 0.8f;      // 壁接触時の減衰率 (新しく追加)
	const float kLimitRunSpeed = 0.5f;        // 走行速度制限
	const float kJumpAccleration = 20.0f;                                         // ジャンプ力
	const float kGgravityAcceleration = 0.8f; // 重力加速度
	const float kLimitFallSpeed = 0.5f;       // 最大落下速度

	const float kBlank = 0.1f;            // めり込み防止の隙間 (新しく追加)
	const float kGroundSearchHeight = 0.1f; // 地面検索の高さ (新しく追加)
	
	//攻撃ギミックの経過時間カウンター
	uint32_t attckParmeter_ = 0;
	
	// 接地フラグ
	bool onGround_ = false;

	// 旋回制御
	enum class LRDirection {
		kRight,
		kLeft,
	};
	LRDirection lrDirection_ = LRDirection::kRight; // 左右方向
	float turnTimer_ = 0.0f;                        // 旋回タイマー
	const float kTimeTurn = 0.2f;                   // 旋回時間
	float turnFirstRottationY_ = 0.0f;  
	// 旋回開始時のY軸回転

	// プレイヤーのサイズ (当たり判定用)
	const float kWidth = 1.0f;  // 幅
	const float kHeight = 0.8f; // 高さ

	bool isDead_ = false;



	// マップチップフィールドへのポインタ
	MapChipField* mapchipField_ = nullptr; // Initialize()で設定すること
	KamataEngine::Model *modelAttack_;      // 攻撃用のモデル（必要に応じて）
	KamataEngine::WorldTransform worldTransformAttack_; // 攻撃用のワールド変換
	bool isAttack_ = false;                             // 攻撃中かどうかのフラグ
public:
	
};

