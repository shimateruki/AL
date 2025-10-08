#pragma once

//------------------------------
// 必要なヘッダー
//------------------------------
#include "MapChipField.h" // マップチップフィールド
#include "Math.h"         // 数学ユーティリティ
#include <array>          // std::array 用
#include <numbers>        // 円周率など

using namespace KamataEngine;

//------------------------------
// マップとの当たり判定結果
//------------------------------
struct CollisionMapInfo {
	Vector3 isMovement;       // 計算後の最終移動量
	bool isHitTop = false;    // 天井ヒット
	bool isHitBottom = false; // 地面ヒット
	bool hitWall = false;     // 壁ヒット
};

class Enemy;
class KabeToge; // トゲ壁クラスの前方宣言
class CloudPlatform; // 前方宣言

//------------------------------
// プレイヤークラス定義
//------------------------------
class Player {
public:
	enum class Mode {
		Normal,     // 通常ステージ
		StageSelect // ステージセレクト
	};

	//----------------------------------------
	// 初期化・更新・描画
	//----------------------------------------
	void Initialize(Model* model, Camera* camera, const Vector3& position);
	void Update();
	void Draw();

	//----------------------------------------
	// Getter / Setter
	//----------------------------------------
	void SetMode(Mode mode) { mode_ = mode; }
	void SetStageNodes(const std::vector<Vector3>& nodes); 
	const WorldTransform& GetWorldTransform() const { return worldTransformPlayer_; }
	const Vector3& GetVelocity() const { return velosity_; }
	bool GetisBreak() const { return isbreak; }
	WorldTransform& GetWorldTransform() { return worldTransformPlayer_; }
	Vector3 GetWorldPosition() const;
	Vector3 SetWorldPosition(const Vector3& position) {
		worldTransformPlayer_.translation_ = position;
		return worldTransformPlayer_.translation_;
	}
	AABB GetAABB();
	bool IsDead() const { return isDead_; }
	void SetIsDead(bool isDead) { isDead_ = isDead; }
	void SetMapChipField(MapChipField* mapChipField) { mapchipField_ = mapChipField; }
	float GetWidth() const { return kWidth;}
	float GetHeight() const { return kHeight; }
	float GetTranslationX() const { return worldTransformPlayer_.translation_.x; }
	float SetVelocityY(float y) {
		velosity_.y = y;
		return velosity_.y;
	}
	void SetOnGround(bool ground) { onGround_ = ground; }
	bool GetOnGround() const { return onGround_; }
	// 雲の移動量を設定するためのsetterを追加
	void SetCloudDelta(const Vector3& delta) { cloudDelta_ = delta; }
	bool GetisMove() const { return isMove_; }
	void SetisMove(bool move) { isMove_ = move; }


	//----------------------------------------
	// 衝突
	//----------------------------------------
	void OnCollision(const Enemy* enemy);
	void OnCollision(const KabeToge* togeKabe_);

	std::optional<MapChipField::IndexSet> CheckSteppedBreakable(MapChipField* mapChipField);

	void CheckCloudPlatformCollision(const std::vector<CloudPlatform*>& cloudPlatforms);

	void ApplyCloudDelta();



	//----------------------------------------
	// 行動切り替え
	//----------------------------------------
	void BehaviorRootInitialize();  // 通常行動初期化

private:
	//----------------------------------------
	// 移動処理
	//----------------------------------------
	void Move();      // 通常移動

	//----------------------------------------
	// マップチップ衝突チェック
	//----------------------------------------
	void MapChipUp(CollisionMapInfo& info);
	void MapChipDown(CollisionMapInfo& info);
	void MapChipLeft(CollisionMapInfo& info);
	void MapChipRight(CollisionMapInfo& info);

	//----------------------------------------
	// 状態更新
	//----------------------------------------
	void UpdateOnGround(const CollisionMapInfo& info);
	void UpdateOnWall(const CollisionMapInfo& info);
	void BehaviorRootUpdate();
	void StartMoveToNode(int index);
	void MoveToTarget();
	//----------------------------------------
	// ステージセレクト用Update
	//----------------------------------------
	void UpdateNormal();      // 元のUpdate
	void UpdateStageSelect(); // 新しいステージセレクト用Update
	//----------------------------------------
	// ユーティリティ
	//----------------------------------------
	enum Corner {
		kRightBottom,
		kLeftBottom,
		kRightTop,
		kLeftTop,
	};
	static const uint32_t knumCorner = 4;
	Vector3 CarnerPosition(const Vector3& center, Corner cornter);

	//----------------------------------------
	// 行動状態
	//----------------------------------------
	enum class Behavior {
		kUnKnow,
		kRoot,
	};
	Behavior behavior_ = Behavior::kRoot;
	Behavior behaviorRequest_ = Behavior::kUnKnow;

	//----------------------------------------
	// 攻撃状態
	//----------------------------------------
	enum class AttackPhase {
		reservoir,     // 溜め
		rush,          // 突進
		reverberation, // 余韻
	};
	AttackPhase attackPhase_;
	uint32_t attckParmeter_ = 0; // 攻撃カウンター

private:
	//----------------------------------------
	// コンポーネント
	//----------------------------------------
	Model* model_ = nullptr;   // モデル
	Camera* camera_ = nullptr; // カメラ
	Math* math = nullptr;      // 数学ユーティリティ

	//----------------------------------------
	// 移動関連
	//----------------------------------------
	Vector3 velosity_ = {};     // 通常速度
	Vector3 attckVelosity = {}; // 攻撃速度

	//----------------------------------------
	// 定数（移動物理）
	//----------------------------------------
	const float kAcceleration = 0.5f;
	const float kAtteunuation = 0.3f;
	const float kAttenuationLanding = 0.8f;
	const float kAttenuationWall = 0.8f;
	const float kLimitRunSpeed = 0.3f;
	const float kJumpAccleration = 20.0f;
	const float kGgravityAcceleration = 0.8f;
	const float kLimitFallSpeed = 0.5f;

	const float kBlank = 0.1f;              // めり込み防止
	const float kGroundSearchHeight = 0.1f; // 接地検出用

	//----------------------------------------
	// 当たり判定
	//----------------------------------------
	const float kWidth = 1.0f;  // 幅
	const float kHeight = 0.8f; // 高さ

	//----------------------------------------
	// 状態フラグ
	//----------------------------------------
	bool onGround_ = false; // 接地
	bool isDead_ = false;   // 死亡

	//----------------------------------------
	// 向きと旋回
	//----------------------------------------
	enum class LRDirection {
		kRight,
		kLeft,
	};
	LRDirection lrDirection_ = LRDirection::kRight;
	float turnTimer_ = 0.0f;
	float turnFirstRottationY_ = 0.0f;
	const float kTimeTurn = 0.2f;

	//----------------------------------------
	// モデルとワールド変換
	//----------------------------------------
	WorldTransform worldTransformPlayer_;               // プレイヤー本体
	KamataEngine::Model* modelAttack_;                  // 攻撃モデル
	KamataEngine::WorldTransform worldTransformAttack_; // 攻撃ワールド変換
	bool isAttack_ = false;                             // 攻撃中

	//----------------------------------------
	// マップチップとの関連
	//----------------------------------------
	MapChipField* mapchipField_ = nullptr;

	    Mode mode_ = Mode::Normal;

	// ステージ選択用
	std::vector<Vector3> nodes_;
	std::vector<Vector3> signDirections_; // 各ノードで向く方向
	int currentNode_ = 0;
	int targetNode_ = 0;
	bool isMoving_ = false;
	float t_ = 0.0f;
	Vector3 startPos_, targetPos_;

	bool isRotating_ = false;
	Vector3 startDir_, targetDir_;
	float rotT_ = 0.0f;
	bool isbreak;

	  CloudPlatform* onCloud_ = nullptr; // 乗っている雲を覚える
	                                   // 雲の移動量を保持する変数
	Vector3 cloudDelta_;
	  bool isMove_ = false;

	  	int jumpCount_ = 0; // ジャンプした回数を記録

		bool isSpinning_ = false;         // 回転中かどうかのフラグ
	    float spinTimer_ = 0.0f;          // 回転アニメーションのタイマー
	    const float kSpinDuration = 0.5f; // 回転にかかる時間（秒）
};