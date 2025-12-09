#pragma once

//------------------------------
// 必要なヘッダー
//------------------------------
#include "MapChipField.h" // マップチップフィールド
#include "Math.h"         // 数学ユーティリティ
#include <array>          // std::array 用
#include <numbers>        // 円周率など
#include "PlayerBullet.h"
#include <list>
#include "ParticleManager.h"
#include"KabeToge.h"

using namespace KamataEngine;

//------------------------------
// マップとの当たり判定結果
//------------------------------
struct CollisionMapInfo {
	Vector3 isMovement;       // 計算後の最終移動量
	bool isHitTop = false;    // 天井ヒット
	bool isHitBottom = false; // 地面ヒット
	bool hitWall = false;     // 壁ヒット
	bool onIce = false;
};

class Enemy;
class KabeToge;      // トゲ壁クラスの前方宣言
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
	float GetWidth() const { return kWidth; }
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
	int GetHp() const { return hp_; }
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }
	//----------------------------------------
	// 衝突
	//----------------------------------------
	void OnCollision(const Enemy* enemy);
	void OnCollision(const KabeToge* togeKabe_);

	std::optional<MapChipField::IndexSet> CheckSteppedBreakable(MapChipField* mapChipField);

	void CheckCloudPlatformCollision(const std::vector<CloudPlatform*>& cloudPlatforms);

	void ApplyCloudDelta();

	MapChipType GetFloorChipType();

	void TakeDamage(int damage);
	void CheckAndResolveTogeKabeCollision(const KabeToge* togeKabe); // ★壁用の新しい衝突関数
	void UpdateSquashAnimation();
	bool GetIsInvincible() const { return isInvincible_; }           // 無敵状態を外部から知るため

	//----------------------------------------
	// 行動切り替え
	//----------------------------------------
	void BehaviorRootInitialize(); // 通常行動初期化

	//アニメーション関数
	void UpdateTitleAnimation();
	void StartCameraJump();  // カメラジャンプを開始する
	void UpdateCameraJump(); // カメラジャンプ中のアニメーション更新
	void StartVictoryPose(); // 勝利ポーズを開始する
	void UpdateVictoryAnimation(); // 勝利ポーズ中のアニメーション更新

	void SetParticleManager(ParticleManager* manager) { particleManager_ = manager; }
private:
	//----------------------------------------
	// 移動処理
	//----------------------------------------
	void Move(); // 通常移動

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
	const float kAcceleration = 0.5f;//加速度
	const float kIceAttenuation = 0.05f;//アイス版摩擦
	const float kAtteunuation = 0.3f;//通常摩擦
	const float kAttenuationLanding = 0.8f;//減衰着地
	const float kAttenuationWall = 0.8f;//減衰壁
	const float kLimitRunSpeed = 0.2f;//リミット実行速度
	const float kJumpAccleration = 15.0f;//ジャンプ加速
	const float kGgravityAcceleration = 0.8f;//重力加速度
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
	bool isOnIce_ = false;
	//----------------------------------------
	// HP・ダメージ関連
	//----------------------------------------
	int hp_ = 3;                            // ★HP
	bool isInvincible_ = false;             // 無敵中か
	float invincibleTimer_ = 0.0f;          // 無敵時間タイマー
	const float kInvincibleDuration = 1.0f; // 無敵時間（1秒）
	 //----------------------------------------
	// アニメーション関連
	//----------------------------------------
	float animationTimer_ = 0.0f; // アニメーション周期用のタイマー
	float originalScaleY_ = 1.0f; // 元のYスケールを保存
	//----------------------------------------
	// 着地アニメーション（ぽよん）
	//----------------------------------------
	bool isSquashing_ = false;          // 着地して「ぽよん」中か
	float squashTimer_ = 0.0f;          // ぽよん用タイマー
	const float kSquashDuration = 0.3f; // ぽよんアニメの総時間（0.3秒）
	//----------------------------------------
	// タイトル画面用アニメーション
	//----------------------------------------
	float titleGroundY_ = 0.0f;   // タイトルでの地面のY座標
	bool isTitleJumping_ = false; // タイトルでジャンプ中か
	float titleJumpTimer_ = 0.0f;
	const float kTitleJumpDuration = 0.7f; // タイトルジャンプの時間（秒）
	const float kTitleJumpHeight = 3.0f;   // タイトルジャンプの高さ
	//----------------------------------------
	// タイトル画面用 カメラジャンプ
	//----------------------------------------
	bool isCameraJumping_ = false; // カメラジャンプ中か
	float cameraJumpTimer_ = 0.0f;
	const float kCameraJumpDuration = 1.8f; // カメラジャンプの時間
	const float kCameraJumpZoom = 15.0f;    // 最終的なZ座標
	const float kCameraJumpScale = 10.0f;   // 最終的なスケール

	// 弾関連
	std::list<PlayerBullet*> bullets_;
	float attackCooldown_ = 0.0f;
	const float kAttackInterval = 0.5f; // 0.5秒に1発

	float chargeTimer_ = 0.0f;
	const float kMaxChargeTime = 1.0f; // 1秒でフルチャージ
	bool isCharging_ = false;          // チャージ中フラグ

	ParticleManager* particleManager_ = nullptr;

};