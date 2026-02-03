#pragma once // インクルードガード

// ==========================================
// インクルード
// ==========================================
// --- エンジン・基本機能 ---
#include "KamataEngine.h"
#include "math.h"

// --- ゲームシステム ---
#include "EnemyBullet.h"
#include "MapChipField.h"
#include "ParticleManager.h"

// ==========================================
// 前方宣言 (Forward Declarations)
// ==========================================
class GameScene;
class GameScene1_2;
class GameScene1_3;
class GameScene2_1;
class Player;

// KamataEngine名前空間を使用
using namespace KamataEngine;

// ==========================================
// 敵クラス定義
// ==========================================
class Enemy {
public:
	// -------------------------------------------------
	// 定数・列挙型 (Enums)
	// -------------------------------------------------

	// 敵の種類
	enum class Type {
		kWalk,    // 普通に歩く
		kShooter, // その場で撃ってくる
		kFlying,  // 飛行タイプ
		kHoming,  // ホーミングタイプ(爆発)
		kSplit,   // 分裂タイプ
		kSlime,   // スライムタイプ
		kFlee,    // 逃げるタイプ
		kBoss,    // ボス
	};

	// 飛行パターン
	enum class FlightPattern {
		kVertical,   // 上下移動
		kHorizontal, // 左右移動
	};

	// 基本行動状態
	enum class Behavior {
		kUnKnow,
		kRoot,
		kisDead, // 死亡状態
	};

	// ボスの詳細状態
	enum class BossState {
		kWait,        // 待機（プレイヤーの方を見る）
		kEntrance,    // 登場演出
		kCharge,      // 溜め（震える予兆）
		kJump,        // ジャンプ中
		kCoolDown,    // 着地後の硬直（攻撃チャンス）
		kPhaseChange, // 形態変化（怒り演出）
		kTripleJump,  // 3連ジャンプ（Phase2用）
		kBarrage,     // 拡散弾発射
		kSummon,      // 手下召喚
		kDashCharge,  // 突進の予兆（平たく潰れる）
		kDash,        // 突進中
		kHighJump,    // 画面外へ飛び上がる
		kSkyWait,     // 上空で待機（プレイヤーを狙う）
		kSkyFall,     // 急降下
		kSpinAttack,  // 回転攻撃
		kDead,        // 死亡
	};

	// -------------------------------------------------
	// 主要メソッド (Core Methods)
	// -------------------------------------------------
	// 初期化
	void Initialize(Model* model, Camera* camera, const Vector3& position, Type type = Type::kWalk);
	// 更新
	void Update();
	// 描画
	void Draw();

	// -------------------------------------------------
	// ゲッター・セッター (Getters / Setters)
	// -------------------------------------------------
	// 基本情報
	Vector3 GetWorldPosition();
	Type GetType() const { return type_; }
	int GetHp() const { return hp_; }

	// 状態フラグ系
	bool GetIsDead() { return isDead_; }
	bool isDead() const { return isDead_; } // 重複しているが維持
	void SetDead(bool dead) { isDead_ = dead; }

	bool GetIsSplit() const { return isSplit_; }
	void SetIsSplit(bool flag) { isSplit_ = flag; }

	bool IsReadyToExplode() const { return isReadyToExplode_; }

	// 衝突無効化設定
	bool isCollisonDisabled() const { return isCollisDisabled_; }
	void SetCollisonDisabled(bool disabled) { isCollisDisabled_ = disabled; }

	// 外部参照のセット
	void SetPlayer(Player* player) { player_ = player; }
	void SetMapChipField(MapChipField* field) { mapChipField_ = field; }
	void SetParticleManager(ParticleManager* particleManager) { particleManager_ = particleManager; }

	// シーン参照のセット
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
	void SetGameScene1_2(GameScene1_2* scene) { gameScene1_2_ = scene; }
	void SetGameScene1_3(GameScene1_3* scene) { gameScene1_3_ = scene; }
	void SetGameScene2_1(GameScene2_1* scene) { gameScene2_1_ = scene; }

	// パラメータ設定
	void SetFlightPattern(FlightPattern pattern) { flightPattern_ = pattern; }
	void SetVelocity(const Vector3& velocity) { velocity_ = velocity; }
	void SetBossState(BossState state) { bossState_ = state; }

	// -------------------------------------------------
	// ゲームロジック・判定 (Game Logic)
	// -------------------------------------------------
	// 衝突判定関連
	AABB GetAABB();
	void onCollision(const Player* player);
	void OnStomped(const Player* player); // 踏まれた時の処理
	void TakeDamage(int damage);          // ダメージ処理

	// ユーティリティ
	bool IsWalkable(MapChipType type);
	const std::list<EnemyBullet*>& GetBullets() const { return bullets_; } // 弾リスト取得

private:
	// ==========================================
	// メンバ変数・内部メソッド
	// ==========================================

	// -------------------------------------------------
	// 内部処理用構造体・関数
	// -------------------------------------------------
	// マップ当たり判定結果
	struct CollisionMapInfo {
		Vector3 isMovement;
		bool isHitTop = false;
		bool isHitBottom = false;
		bool hitWall = false;
	};

	// 座標計算用
	enum Corner { kRightBottom, kLeftBottom, kRightTop, kLeftTop };
	static const uint32_t knumCorner = 4;
	Vector3 CarnerPosition(const Vector3& center, Corner cornter);

	// マップチップ衝突判定
	void MapChipUp(CollisionMapInfo& info);
	void MapChipDown(CollisionMapInfo& info);
	void MapChipLeft(CollisionMapInfo& info);
	void MapChipRight(CollisionMapInfo& info);

	// 接地・壁判定更新
	void UpdateOnGround(const CollisionMapInfo& info);
	void UpdateOnWall(const CollisionMapInfo& info);

	// AI・行動決定
	void SelectNextAction();
	void EmitExplosion();

	// -------------------------------------------------
	// 基本コンポーネント
	// -------------------------------------------------
	WorldTransform worldTransformEnemy_; // ワールド変換
	Model* model_ = nullptr;             // モデル
	Camera* camera_ = nullptr;           // カメラ
	Math* math = nullptr;                // 数学ヘルパー

	// -------------------------------------------------
	// 外部参照 (References)
	// -------------------------------------------------
	Player* player_ = nullptr;                   // ターゲット（プレイヤー）
	MapChipField* mapChipField_ = nullptr;       // マップ
	ParticleManager* particleManager_ = nullptr; // パーティクル管理者

	// 各シーンへのポインタ
	GameScene* gameScene_ = nullptr;
	GameScene1_2* gameScene1_2_ = nullptr;
	GameScene1_3* gameScene1_3_ = nullptr;
	GameScene2_1* gameScene2_1_ = nullptr;

	// -------------------------------------------------
	// ステータス・状態 (Status)
	// -------------------------------------------------
	Type type_ = Type::kWalk;       // 敵タイプ
	int hp_ = 0;                    // 現在HP
	bool isDead_ = false;           // 死亡フラグ
	bool isCollisDisabled_ = false; // 衝突判定無効フラグ
	bool isSplit_ = false;          // 分裂済みフラグ
	float damageBlinkTimer_ = 0.0f; // ダメージ点滅タイマー

	// 行動状態
	Behavior behavior_ = Behavior::kRoot;
	Behavior behaviorRequest_ = Behavior::kUnKnow;

	// -------------------------------------------------
	// 物理・移動パラメータ (Physics & Movement)
	// -------------------------------------------------
	Vector3 velocity_ = {}; // 速度ベクトル
	bool onGround_ = false; // 接地フラグ

	// コリジョン・サイズ定数
	const float kWidth = 1.0f;
	const float kHeight = 0.8f;
	const float kBlank = 0.1f;
	const float kGroundSearchHeight = 0.1f;

	// 歩行関連
	static inline const float kWalkSpeed = 0.02f;
	static inline const float kWalkMotionAngleStart = 0.0f;
	static inline const float kWalkMotionAngleEnd = 30.0f;
	static inline const float kWalkMotionTime = 1.0f;
	float walkTimer = 0.0f;

	// 反転・アニメーション制御
	float flipCooldownTimer = 0.0f;
	const float kFlipCooldown = 2.0f;
	bool animationPlaying = false;

	// -------------------------------------------------
	// タイプ別固有パラメータ (Specific Mechanics)
	// -------------------------------------------------

	// --- 射撃タイプ (Shooter) ---
	std::list<EnemyBullet*> bullets_;
	float shotTimer_ = 0.0f;
	const float kShotInterval = 2.0f;

	// --- 飛行タイプ (Flying) ---
	FlightPattern flightPattern_ = FlightPattern::kVertical;
	Vector3 startPosition_ = {};
	float flightTimer_ = 0.0f;
	const float kFlightRange = 3.0f;
	const float kFlightSpeed = 2.0f;

	// --- ホーミング・自爆タイプ (Homing) ---
	const float kHomingSpeed = 0.01f;
	const float kDetectionRange = 10.0f;
	const float kExplosionTime = 2.0f;
	float homingTimer_ = 0.0f;
	float explosionTimer_ = 0.0f;
	bool isReadyToExplode_ = false;

	// --- ボス関連 (Boss) ---
	BossState bossState_ = BossState::kWait;
	int maxHp_ = 0;          // 最大HP
	bool isPhase2_ = false;  // 第2形態フラグ
	float bossTimer_ = 0.0f; // 行動用タイマー

	// ジャンプ制御
	int jumpCountBoss_ = 0;
	int jumpCounter_ = 0;
	float jumpWaitTimer_ = 0.0f;
	const float kEnemyJumpSpeed = 12.0f;
};