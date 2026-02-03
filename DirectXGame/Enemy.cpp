#define NOMINMAX
#include "Enemy.h"
#include "GameScene.h"
#include "Player.h"

// STL / Standard
#include <algorithm> // std::clamp, std::max, std::min
#include <cassert>
#include <numbers>

using namespace KamataEngine;

// ==========================================
// 定数定義
// ==========================================
namespace {
const float kWalkSpeed = 0.1f;
const float kLimitFallSpeed = 0.5f;
const float kGgravityAcceleration = 0.8f;
} // namespace

// ==========================================
// 初期化処理
// ==========================================
void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position, Type type) {
	assert(model);

	// --- 基本コンポーネント設定 ---
	model_ = model;
	camera_ = camera;
	gameScene_ = nullptr;

	// --- トランスフォーム初期化 ---
	worldTransformEnemy_.Initialize();
	worldTransformEnemy_.translation_ = position;
	worldTransformEnemy_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f; // 左向き

	// --- パラメータ初期化 ---
	type_ = type;
	velocity_ = {-kWalkSpeed, 0, 0};
	walkTimer = 0.0f;
	shotTimer_ = 0.0f;
	startPosition_ = position;
	flightTimer_ = 0.0f;

	// --- タイプ別ステータス設定 ---
	if (type_ == Type::kSplit) {
		isSplit_ = true;
		worldTransformEnemy_.scale_ = {2.0f, 2.0f, 2.0f};
		hp_ = 5;
	} else {
		isSplit_ = false;
		worldTransformEnemy_.scale_ = {1.0f, 1.0f, 1.0f};
	}

	// HPと挙動の個別設定
	switch (type_) {
	case Type::kWalk:
		hp_ = 2;
		break;
	case Type::kShooter:
		hp_ = 2;
		break;
	case Type::kFlying:
		hp_ = 3;
		break;
	case Type::kHoming:
		hp_ = 1;
		break;
	case Type::kSlime:
		hp_ = 3;
		jumpWaitTimer_ = 0.0f;
		velocity_ = {0, 0, 0};
		break;
	case Type::kBoss:
		hp_ = 150;
		maxHp_ = hp_;
		isPhase2_ = false;
		worldTransformEnemy_.scale_ = {1.0f, 1.0f, 1.0f};
		bossState_ = BossState::kWait;
		bossTimer_ = 0.0f;
		velocity_ = {0, 0, 0};
		break;
	default:
		break;
	}
}

// ==========================================
// 更新処理 (Main Update)
// ==========================================
void Enemy::Update() {
	// ------------------------------------------
	// 1. 弾の更新と削除
	// ------------------------------------------
	for (auto it = bullets_.begin(); it != bullets_.end();) {
		(*it)->Update();
		if ((*it)->IsDead()) {
			delete (*it);
			it = bullets_.erase(it);
		} else {
			++it;
		}
	}

	// ------------------------------------------
	// 2. 行動状態遷移 (Behavior Transition)
	// ------------------------------------------
	if (flipCooldownTimer > 0) {
		flipCooldownTimer -= 1.0f / 60.0f;
	}

	// リクエストがあれば状態を変更
	if (behaviorRequest_ != Behavior::kUnKnow) {
		behavior_ = behaviorRequest_;
		if (behavior_ == Behavior::kisDead) {
			walkTimer = 0.0f; // 死亡アニメーション用リセット
		}
		behaviorRequest_ = Behavior::kUnKnow;
	}

	// ------------------------------------------
	// 3. 行動ごとの更新 (Behavior Update)
	// ------------------------------------------
	switch (behavior_) {
	case Enemy::Behavior::kUnKnow:
		break;

	// === 生存中の挙動 ===
	case Enemy::Behavior::kRoot: {
		// --- 重力処理 (共通) ---
		if (!onGround_) {
			velocity_.y += -kGgravityAcceleration / 60.0f;
			velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
		} else {
			velocity_.y = 0.0f;
		}

		// ==========================================
		// タイプ別ロジック (Type Specific Logic)
		// ==========================================

		// --- 歩行タイプ & 分裂タイプ ---
		if (type_ == Type::kWalk || type_ == Type::kSplit) {
			// 崖の端判定
			if (onGround_) {
				Vector3 pos = GetWorldPosition();
				Vector3 positionsCheck = pos + Vector3(0, -kGroundSearchHeight, 0);
				positionsCheck.x += (velocity_.x > 0 ? kWidth / 2.0f : -kWidth / 2.0f);

				MapChipType mapchipType = MapChipType::kBlank_;
				MapChipField::IndexSet indexSet;
				if (mapChipField_) {
					indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsCheck);
					mapchipType = mapChipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
				}
				// 足場がなければ反転
				if (!IsWalkable(mapchipType)) {
					if (flipCooldownTimer <= 0) {
						velocity_.x *= -1;
						flipCooldownTimer = kFlipCooldown;
					}
				}
			}
			// 歩行アニメーション
			walkTimer += 1.0f / 60.0f;
			worldTransformEnemy_.rotation_.y = (velocity_.x > 0) ? std::numbers::pi_v<float> / 2.0f : std::numbers::pi_v<float> * 3.0f / 2.0f;
			worldTransformEnemy_.rotation_.x = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer / 1.0f);
		}

		// --- 射撃タイプ ---
		else if (type_ == Type::kShooter) {
			velocity_.x = 0.0f;
			shotTimer_ += 1.0f / 60.0f;

			float timeToShoot = kShotInterval - shotTimer_;
			const float kTelegraphTime = 0.5f;

			// 予兆アニメーション (発射直前)
			if (timeToShoot <= kTelegraphTime && timeToShoot > 0.0f) {
				float t = 1.0f - (timeToShoot / kTelegraphTime);
				float squashY = 1.0f - (t * 0.4f);
				float stretchXZ = 1.0f + (t * 0.3f);
				worldTransformEnemy_.scale_ = {stretchXZ, squashY, stretchXZ};

				float shakeAmount = t * 0.05f;
				float offsetX = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * shakeAmount;
				worldTransformEnemy_.translation_.x = startPosition_.x + offsetX;
			} else {
				// 待機アニメーション
				float breathe = std::sin(shotTimer_ * 5.0f) * 0.05f;
				worldTransformEnemy_.scale_ = {1.0f - breathe, 1.0f + breathe, 1.0f - breathe};
				worldTransformEnemy_.translation_.x = startPosition_.x;
			}

			// プレイヤー検知と発射
			if (player_) {
				Vector3 myPos = worldTransformEnemy_.translation_;
				Vector3 targetPos = player_->GetWorldPosition();
				Vector3 diff = targetPos - myPos;

				// 向き調整
				worldTransformEnemy_.rotation_.y = (diff.x > 0) ? std::numbers::pi_v<float> / 2.0f : std::numbers::pi_v<float> * 3.0f / 2.0f;
				worldTransformEnemy_.rotation_.x = 0.0f;

				float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
				const float kAttackRange = 15.0f;

				if (dist <= kAttackRange) {
					if (shotTimer_ >= kShotInterval) {
						shotTimer_ = 0.0f;
						if (dist > 0.0f) {
							// 弾生成
							EnemyBullet* newBullet = new EnemyBullet();
							Vector3 velocity = diff;
							velocity *= (0.2f / dist); // 正規化 * 速度

							newBullet->Initialize(model_, myPos, velocity, mapChipField_);
							bullets_.push_back(newBullet);

							// 姿勢リセット
							worldTransformEnemy_.scale_ = {1.0f, 1.0f, 1.0f};
							worldTransformEnemy_.translation_.x = startPosition_.x;
						}
					}
				} else {
					shotTimer_ += 1.0f / 60.0f;
				}
			}
		}

		// --- 飛行タイプ ---
		else if (type_ == Type::kFlying) {
			velocity_ = {0, 0, 0};
			flightTimer_ += 1.0f / 60.0f;
			float offset = std::sin(flightTimer_ * kFlightSpeed) * kFlightRange;

			if (flightPattern_ == FlightPattern::kVertical) {
				worldTransformEnemy_.translation_.x = startPosition_.x;
				worldTransformEnemy_.translation_.y = startPosition_.y + offset;
				worldTransformEnemy_.rotation_.x = 0.0f;
			} else if (flightPattern_ == FlightPattern::kHorizontal) {
				worldTransformEnemy_.translation_.x = startPosition_.x + offset;
				worldTransformEnemy_.translation_.y = startPosition_.y;

				float moveDir = std::cos(flightTimer_ * kFlightSpeed);
				worldTransformEnemy_.rotation_.y = (moveDir > 0) ? std::numbers::pi_v<float> / 2.0f : std::numbers::pi_v<float> * 3.0f / 2.0f;
			}

			// ：飛行用ジェット噴射パーティクル 
			if (particleManager_) {
				// 1フレームに1回〜2回出して勢いをつける
				for (int i = 0; i < 2; i++) {
					Vector3 pos = worldTransformEnemy_.translation_;
					pos.y -= 0.5f; // 敵のお尻（下側）から出す

					// 少し散らす（エンジンの幅）
					pos.x += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.4f;
					pos.z += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.4f;

					// 下方向に勢いよく噴射！
					Vector3 vel = {0.0f, -0.15f, 0.0f};
					Vector3 accel = {0.0f, 0.0f, 0.0f};
					Vector4 startColor = {1.0f, 0.6f, 0.0f, 1.0f};
					Vector4 endColor = {1.0f, 0.0f, 0.0f, 0.0f};

					// サイズ：大きめに出して小さく消す
					particleManager_->Emit(pos, vel, accel, 0.3f, 0.6f, 0.0f, startColor, endColor);
				}
			}
			// ▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲

			math->worldTransFormUpdate(worldTransformEnemy_);
			return;
		}

		// --- ホーミング（自爆）タイプ ---
		else if (type_ == Type::kHoming) {
			// 重力処理は共通部分で行われているが、ここでも追加制御あり
			if (!onGround_) {
				velocity_.y += -kGgravityAcceleration / 60.0f;
				velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
			} else {
				velocity_.y = 0.0f;
			}

			// パーティクル演出
			if (particleManager_ && !isDead_) {
				if (isReadyToExplode_) {
					// 爆発寸前
					if (rand() % 2 == 0) {
						Vector3 pos = worldTransformEnemy_.translation_;
						pos.x += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 1.0f;
						pos.y += (static_cast<float>(rand()) / RAND_MAX) * 1.0f;
						Vector3 vel = {(static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.2f, (static_cast<float>(rand()) / RAND_MAX) * 0.2f, 0.0f};
						particleManager_->Emit(pos, vel, {0, 0, 0}, 0.4f, 0.5f, 0.0f, {1.0f, 0.8f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 0.0f});
					}
				} else {
					// 通常時
					if (rand() % 3 == 0) {
						Vector3 pos = worldTransformEnemy_.translation_;
						pos.x += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.5f;
						pos.y += (static_cast<float>(rand()) / RAND_MAX) * 0.8f;
						Vector3 vel = {0.0f, 0.05f, 0.0f};
						particleManager_->Emit(pos, vel, {0, 0.002f, 0}, 0.8f, 0.4f, 0.8f, {0.8f, 0.0f, 0.0f, 0.8f}, {0.2f, 0.0f, 0.0f, 0.0f});
					}
				}
			}

			// 索敵と追尾
			if (player_) {
				Vector3 myPos = worldTransformEnemy_.translation_;
				Vector3 targetPos = player_->GetWorldPosition();
				Vector3 diff = targetPos - myPos;
				float distSq = (diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z);

				if (distSq < (kDetectionRange * kDetectionRange)) {
					// 自爆判定
					if (distSq < 1.0f) {
						EmitExplosion();

						// 攻撃判定用の不可視弾を生成
						EnemyBullet* hitBox = new EnemyBullet();
						Vector3 hitPos = worldTransformEnemy_.translation_;
						hitPos.y += 0.5f;
						hitBox->Initialize(model_, hitPos, {0, 0, 0}, mapChipField_);
						bullets_.push_back(hitBox);

						behaviorRequest_ = Behavior::kisDead;
						velocity_ = {0, 0, 0};
					}

					// 移動
					float runSpeed = 0.08f;
					if (diff.x > 0) {
						velocity_.x = runSpeed;
						worldTransformEnemy_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
					} else {
						velocity_.x = -runSpeed;
						worldTransformEnemy_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
					}

					// 爆発警告タイマー
					explosionTimer_ += 1.0f / 60.0f;
					if (explosionTimer_ >= kExplosionTime)
						isReadyToExplode_ = true;

					// 震え演出
					float ratio = std::min(explosionTimer_ / 3.0f, 1.0f);
					float shakeSpeed = 40.0f + (ratio * 60.0f);
					float shakeAmount = 0.1f + (ratio * 0.2f);
					homingTimer_ += 1.0f / 60.0f;
					float shake = std::sin(homingTimer_ * shakeSpeed) * shakeAmount;
					worldTransformEnemy_.scale_ = {1.0f + shake, 1.0f + shake, 1.0f + shake};

				} else {
					// 範囲外：停止
					velocity_.x = 0.0f;
					homingTimer_ += 1.0f / 60.0f;
					float breathe = std::sin(homingTimer_ * 3.0f) * 0.05f;
					worldTransformEnemy_.scale_ = {1.0f + breathe, 1.0f - breathe, 1.0f + breathe};
				}
			}

			// ホーミング独自のコリジョン処理
			CollisionMapInfo collisionInfo = {};
			collisionInfo.isMovement = velocity_;
			MapChipUp(collisionInfo);
			MapChipDown(collisionInfo);
			MapChipLeft(collisionInfo);
			MapChipRight(collisionInfo);
			worldTransformEnemy_.translation_ += collisionInfo.isMovement;
			if (collisionInfo.isHitTop)
				velocity_.y = 0;
			if (collisionInfo.hitWall && onGround_)
				velocity_.y = 0.35f; // 壁で小ジャンプ
			UpdateOnGround(collisionInfo);

			math->worldTransFormUpdate(worldTransformEnemy_);
			return; // 処理終了
		}

		// --- スライムタイプ ---
		else if (type_ == Type::kSlime) {
			if (onGround_) {
				velocity_.x = 0.0f;
				jumpWaitTimer_ += 1.0f;
				const float kJumpInterval = 60.0f;

				// 定期的にジャンプ移動
				if (jumpWaitTimer_ >= kJumpInterval) {
					velocity_.y = 0.6f;
					onGround_ = false;

					float moveSpeed = 0.15f;
					if (player_) {
						if (player_->GetWorldPosition().x > worldTransformEnemy_.translation_.x)
							velocity_.x = moveSpeed;
						else
							velocity_.x = -moveSpeed;
					} else {
						velocity_.x = -moveSpeed;
					}
					jumpWaitTimer_ = 0.0f;
				}
			}
		}

		// --- 逃走タイプ ---
		else if (type_ == Type::kFlee) {
			float runSpeed = 0.15f;
			if (player_) {
				// プレイヤーから逃げる方向へ
				if (worldTransformEnemy_.translation_.x > player_->GetWorldPosition().x)
					velocity_.x = runSpeed;
				else
					velocity_.x = -runSpeed;
			}

			// 崖チェック（落ちそうなら止まる）
			if (onGround_) {
				Vector3 pos = GetWorldPosition();
				Vector3 checkPos = pos + Vector3(velocity_.x > 0 ? kWidth / 2.0f : -kWidth / 2.0f, -kGroundSearchHeight, 0);

				MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(checkPos);
				MapChipType type = mapChipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);

				if (!IsWalkable(type)) {
					velocity_.x = 0.0f;
				}
			}

			if (velocity_.x != 0.0f) {
				walkTimer += 1.0f / 60.0f;
				worldTransformEnemy_.rotation_.y = (velocity_.x > 0) ? std::numbers::pi_v<float> / 2.0f : std::numbers::pi_v<float> * 3.0f / 2.0f;
				worldTransformEnemy_.rotation_.x = std::sin(std::numbers::pi_v<float> * 4.0f * walkTimer);
			}
		}

		// ==========================================
		// ★ボス (Type::kBoss) ロジック
		// ==========================================
		if (type_ == Type::kBoss) {
			// --- ボスの常時オーラ演出 ---
			if (particleManager_ && !isDead_) {
				Vector4 auraColorStart = isPhase2_ ? Vector4{0.8f, 0.0f, 0.2f, 1.0f} : Vector4{1.0f, 0.4f, 0.7f, 0.8f};
				Vector4 auraColorEnd = isPhase2_ ? Vector4{0.2f, 0.0f, 0.0f, 0.0f} : Vector4{1.0f, 0.0f, 0.5f, 0.0f};
				int frequency = isPhase2_ ? 1 : 2;

				if (rand() % frequency == 0) {
					Vector3 pos = worldTransformEnemy_.translation_;
					pos.x += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 1.5f;
					pos.y += (static_cast<float>(rand()) / RAND_MAX) * 1.5f;
					pos.z += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 1.5f;
					Vector3 vel = {0.0f, 0.05f, 0.0f};
					Vector3 accel = {0.0f, 0.001f, 0.0f};
					particleManager_->Emit(pos, vel, accel, 0.8f, 0.5f, 0.0f, auraColorStart, auraColorEnd);
				}
			}

			// --- ボス状態遷移 ---
			switch (bossState_) {

			// 1. 登場演出
			case BossState::kEntrance:
				if (!onGround_) {
					worldTransformEnemy_.scale_ = {0.8f, 1.3f, 0.8f}; // 落下中の伸び
				} else {
					// 接地瞬間
					if (bossTimer_ == 0.0f) {
						velocity_.y = 0.6f;
						onGround_ = false;
						bossTimer_ = 1.0f;
						worldTransformEnemy_.scale_ = {1.5f, 0.5f, 1.5f}; // 接地時の潰れ

						// 衝撃エフェクト
						if (particleManager_) {
							for (int i = 0; i < 20; i++) {
								Vector3 pPos = worldTransformEnemy_.translation_;
								float angle = (std::numbers::pi_v<float> * 2.0f / 20.0f) * i;
								Vector3 pVel = {std::cos(angle) * 0.3f, 0.0f, std::sin(angle) * 0.3f};
								particleManager_->Emit(pPos, pVel, {0, 0, 0}, 0.5f, 1.0f, 0.0f, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.5f, 0.0f});
							}
						}
					} else {
						velocity_.x = 0.0f;
						worldTransformEnemy_.scale_ = {1.0f, 1.0f, 1.0f};
					}
				}
				break;

			// 2. 待機
			case BossState::kWait:
				velocity_.x = 0.0f;
				bossTimer_ += 1.0f / 60.0f;
				if (player_) {
					worldTransformEnemy_.rotation_.y =
					    (player_->GetWorldPosition().x > worldTransformEnemy_.translation_.x) ? std::numbers::pi_v<float> / 2.0f : std::numbers::pi_v<float> * 3.0f / 2.0f;
				}
				{
					float waitTime = isPhase2_ ? 0.8f : 1.5f;
					if (bossTimer_ >= waitTime)
						SelectNextAction();
				}
				break;

			// 3. 形態変化（怒り）
			case BossState::kPhaseChange:
				bossTimer_ += 1.0f / 60.0f;
				velocity_.x = 0.0f;
				{
					float shake = std::sin(bossTimer_ * 100.0f) * 0.1f;
					worldTransformEnemy_.scale_ = {1.0f + shake, 1.0f - shake, 1.0f + shake};
				}
				// 怒りの炎
				if (particleManager_) {
					for (int i = 0; i < 3; i++) {
						Vector3 pos = worldTransformEnemy_.translation_;
						pos.x += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f;
						Vector3 vel = {0.0f, 0.2f, 0.0f};
						particleManager_->Emit(pos, vel, {0, 0, 0}, 1.0f, 0.5f, 0.0f, {1.0f, 0.0f, 0.0f, 1.0f}, {0.2f, 0.0f, 0.0f, 0.0f});
					}
				}
				if (bossTimer_ >= 2.0f) {
					isPhase2_ = true;
					bossState_ = BossState::kWait;
					worldTransformEnemy_.scale_ = {1.0f, 1.0f, 1.0f};
				}
				break;

			// 4. 溜め（ジャンプ予兆）
			case BossState::kCharge:
				bossTimer_ += 1.0f / 60.0f;
				{
					float shake = std::sin(bossTimer_ * 50.0f) * 0.05f;
					worldTransformEnemy_.scale_ = {1.0f + shake, 1.0f * 0.8f, 1.0f + shake};
				}
				// エネルギー吸引
				if (particleManager_ && rand() % 2 == 0) {
					Vector3 center = worldTransformEnemy_.translation_;
					center.y += 1.0f;
					Vector3 offset = {
					    (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 3.0f, (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 3.0f, (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 3.0f};
					particleManager_->Emit(center + offset, offset * -0.05f, {0, 0, 0}, 0.5f, 0.3f, 0.0f, {1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.5f, 0.0f, 0.0f});
				}
				{
					float chargeTime = isPhase2_ ? 0.5f : 1.0f;
					if (bossTimer_ >= chargeTime) {
						bossState_ = BossState::kJump;
						bossTimer_ = 0.0f;
						onGround_ = false;
						velocity_.y = 0.5f;
						if (player_) {
							float kJumpSpeed = isPhase2_ ? 0.25f : 0.15f;
							velocity_.x = (player_->GetWorldPosition().x > worldTransformEnemy_.translation_.x) ? kJumpSpeed : -kJumpSpeed;
						}
						worldTransformEnemy_.scale_ = {1.0f, 1.0f, 1.0f};
					}
				}
				break;

			// 5. ジャンプ中 & 着地
			case BossState::kJump:
				worldTransformEnemy_.rotation_.z += 0.1f;
				if (onGround_) {
					bossState_ = BossState::kCoolDown;
					bossTimer_ = 0.0f;
					velocity_.x = 0.0f;
					worldTransformEnemy_.rotation_.z = 0.0f;

					// 着地エフェクト
					if (particleManager_) {
						for (int i = 0; i < 10; i++) {
							Vector3 pPos = worldTransformEnemy_.translation_;
							pPos.x += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f;
							particleManager_->Emit(pPos, {0.0f, 0.1f, 0.0f}, {0, 0, 0}, 0.6f, 0.5f, 0.0f, {0.8f, 0.7f, 0.6f, 1.0f}, {0.8f, 0.7f, 0.6f, 0.0f});
						}
					}
					// Phase2なら衝撃波生成
					if (isPhase2_) {
						for (int i = -1; i <= 1; i += 2) {
							EnemyBullet* shockwave = new EnemyBullet();
							shockwave->Initialize(model_, worldTransformEnemy_.translation_ + Vector3(0, 0.5f, 0), {(float)i * 0.3f, 0, 0}, mapChipField_);
							bullets_.push_back(shockwave);
						}
					}
				}
				break;

			// 6. 3連ジャンプ (Phase2)
			case BossState::kTripleJump:
				if (!onGround_) {
					worldTransformEnemy_.rotation_.x += 0.2f;
				} else {
					if (velocity_.y <= 0.0f && bossTimer_ == 0.0f && particleManager_) {
						// 接地エフェクト
						for (int i = 0; i < 5; i++) {
							Vector3 pPos = worldTransformEnemy_.translation_ + Vector3((static_cast<float>(rand()) / RAND_MAX - 0.5f) * 1.5f, 0, 0);
							particleManager_->Emit(pPos, {0, 0.1f, 0}, {0, 0, 0}, 0.5f, 0.3f, 0.0f, {1.0f, 1.0f, 1.0f, 0.8f}, {1.0f, 1.0f, 1.0f, 0.0f});
						}
					}
					bossTimer_ += 1.0f / 60.0f;
					if (bossTimer_ <= 0.1f && velocity_.y <= 0.0f) {
						jumpCounter_++;
						if (jumpCounter_ >= 3) {
							bossState_ = BossState::kCoolDown;
							jumpCounter_ = 0;
							worldTransformEnemy_.rotation_.x = 0.0f;
						} else {
							onGround_ = false;
							velocity_.y = 0.45f;
							if (player_)
								velocity_.x = (player_->GetWorldPosition().x > worldTransformEnemy_.translation_.x) ? 0.25f : -0.25f;
						}
					}
				}
				break;

			// 7. 拡散弾 (Barrage)
			case BossState::kBarrage:
				bossTimer_ += 1.0f / 60.0f;
				velocity_.x = 0.0f;
				if (player_) {
					worldTransformEnemy_.rotation_.y =
					    (player_->GetWorldPosition().x > worldTransformEnemy_.translation_.x) ? std::numbers::pi_v<float> / 2.0f : std::numbers::pi_v<float> * 3.0f / 2.0f;
				}
				worldTransformEnemy_.rotation_.x = -0.5f;
				worldTransformEnemy_.translation_.x += std::sin(bossTimer_ * 60.0f) * 0.05f;

				if (bossTimer_ >= 1.0f) {
					int bulletCount = 5;
					float speed = 0.4f;
					float spreadAngle = 15.0f * (std::numbers::pi_v<float> / 180.0f);
					Vector3 pos = worldTransformEnemy_.translation_ + Vector3(0, 0.5f, 0);

					// 発射エフェクト
					if (particleManager_) {
						for (int k = 0; k < 10; k++) {
							Vector3 pVel = {(static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.5f, (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.5f, 0};
							particleManager_->Emit(pos, pVel, {0, 0, 0}, 0.5f, 0.3f, 0.0f, {1.0f, 1.0f, 0.5f, 1.0f}, {1.0f, 0.0f, 0.0f, 0.0f});
						}
					}

					float baseAngle = 0.0f;
					if (player_) {
						Vector3 diff = player_->GetWorldPosition() + Vector3(0, 0.5f, 0) - pos;
						baseAngle = std::atan2(diff.y, diff.x);
					} else {
						baseAngle = (worldTransformEnemy_.rotation_.y == std::numbers::pi_v<float> / 2.0f) ? 0.0f : std::numbers::pi_v<float>;
					}

					for (int i = 0; i < bulletCount; i++) {
						float angleOffset = (i - (bulletCount / 2)) * spreadAngle;
						Vector3 vel = {std::cos(baseAngle + angleOffset) * speed, std::sin(baseAngle + angleOffset) * speed, 0.0f};
						EnemyBullet* bullet = new EnemyBullet();
						bullet->Initialize(model_, pos, vel, mapChipField_);
						bullets_.push_back(bullet);
					}
					bossState_ = BossState::kCoolDown;
					bossTimer_ = 0.0f;
					worldTransformEnemy_.rotation_.x = 0.0f;
				}
				break;

			// 8. 手下召喚
			case BossState::kSummon:
				bossTimer_ += 1.0f / 60.0f;
				velocity_.x = 0.0f;
				worldTransformEnemy_.scale_.y = 1.0f - std::sin(bossTimer_ * 10.0f) * 0.3f;

				if (particleManager_ && rand() % 5 == 0) {
					particleManager_->Emit(worldTransformEnemy_.translation_ + Vector3(0, 1.5f, 0), {0, 0.1f, 0}, {0, 0, 0}, 0.6f, 0.5f, 0.0f, {0.5f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 0.0f});
				}

				if (bossTimer_ >= 1.5f) {
					if (gameScene_) {
						for (int i = 0; i < 2; i++) {
							Enemy* minion = new Enemy();
							minion->Initialize(model_, camera_, worldTransformEnemy_.translation_ + Vector3(0, 1.0f, 0), Enemy::Type::kSlime);
							minion->SetPlayer(player_);
							minion->SetMapChipField(mapChipField_);
							minion->SetGameScene(gameScene_);
							minion->SetParticleManager(particleManager_);
							minion->SetVelocity({((i == 0) ? -1.0f : 1.0f) * 0.2f, 0.4f, 0.0f});
							gameScene_->AddEnemy(minion);
						}
					}
					bossState_ = BossState::kCoolDown;
					bossTimer_ = 0.0f;
					worldTransformEnemy_.scale_ = {1.0f, 1.0f, 1.0f};
				}
				break;

			// 9. 突進準備
			case BossState::kDashCharge:
				bossTimer_ += 1.0f / 60.0f;
				{
					float chargeDuration = isPhase2_ ? 0.6f : 1.0f;
					float t = bossTimer_ / chargeDuration;
					float currentScaleY = 1.0f * (1.0f - t * 0.4f);
					worldTransformEnemy_.scale_ = {1.0f * (1.0f + t * 0.3f), currentScaleY, 1.0f};

					if (particleManager_ && rand() % 5 == 0) {
						Vector3 vel = {(static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.2f, 0, 0};
						particleManager_->Emit(worldTransformEnemy_.translation_, vel, {0, 0, 0}, 0.4f, 0.4f, 0.0f, {0.8f, 0.7f, 0.6f, 1.0f}, {0.8f, 0.7f, 0.6f, 0.0f});
					}

					float offsetY = (1.0f - currentScaleY) / 2.0f;
					worldTransformEnemy_.translation_.y -= offsetY * 0.1f;

					if (player_) {
						float backStepSpeed = 0.05f;
						if (player_->GetWorldPosition().x > worldTransformEnemy_.translation_.x) {
							worldTransformEnemy_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
							velocity_.x = -backStepSpeed;
						} else {
							worldTransformEnemy_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
							velocity_.x = backStepSpeed;
						}
					}

					if (bossTimer_ >= chargeDuration) {
						bossState_ = BossState::kDash;
						bossTimer_ = 0.0f;
						float dashSpeed = isPhase2_ ? 0.8f : 0.5f;
						velocity_.x = (worldTransformEnemy_.rotation_.y == std::numbers::pi_v<float> / 2.0f) ? dashSpeed : -dashSpeed;
						worldTransformEnemy_.scale_ = {1.2f, 0.8f, 1.0f};
						worldTransformEnemy_.rotation_.z = (velocity_.x > 0) ? -0.2f : 0.2f;
					}
				}
				break;

			// 10. 突進中
			case BossState::kDash:
				bossTimer_ += 1.0f / 60.0f;
				if (particleManager_) {
					particleManager_->Emit(
					    worldTransformEnemy_.translation_ + Vector3(0, 0.5f, 0), {-velocity_.x * 0.5f, 0, 0}, {0, 0, 0}, 0.8f, 0.4f, 0.0f, {1.0f, 0.4f, 0.7f, 0.8f}, {1.0f, 0.4f, 0.7f, 0.0f});
				}
				if (onGround_ && std::fmod(bossTimer_, 0.2f) < 0.05f) {
					velocity_.y = 0.2f;
					onGround_ = false;
				}
				if (bossTimer_ >= 0.4f || std::abs(velocity_.x) < 0.001f) {
					bossState_ = BossState::kCoolDown;
					bossTimer_ = 0.0f;
					velocity_.x = 0.0f;
					worldTransformEnemy_.scale_ = {1.0f, 1.0f, 1.0f};
					worldTransformEnemy_.rotation_.z = 0.0f;
				}
				break;

			// 11. メテオ・上昇
			case BossState::kHighJump:
				onGround_ = false;
				velocity_ = {0.0f, 0.8f, 0.0f};
				if (particleManager_) {
					particleManager_->Emit(worldTransformEnemy_.translation_, {0, -0.2f, 0}, {0, 0, 0}, 0.8f, 0.5f, 0.0f, {1.0f, 1.0f, 1.0f, 0.8f}, {1.0f, 1.0f, 1.0f, 0.0f});
				}
				if (worldTransformEnemy_.translation_.y > startPosition_.y + 15.0f) {
					bossState_ = BossState::kSkyWait;
					bossTimer_ = 0.0f;
					velocity_ = {0, 0, 0};
				}
				break;

			// 12. メテオ・上空待機
			case BossState::kSkyWait:
				bossTimer_ += 1.0f / 60.0f;
				velocity_ = {0, 0, 0};
				if (player_) {
					worldTransformEnemy_.translation_.x += (player_->GetWorldPosition().x - worldTransformEnemy_.translation_.x) * 0.1f;
				}
				worldTransformEnemy_.rotation_.y += 0.3f;
				if (bossTimer_ >= 1.5f) {
					bossState_ = BossState::kSkyFall;
					bossTimer_ = 0.0f;
					velocity_.y = -0.5f;
				}
				break;

			// 13. メテオ・急降下
			case BossState::kSkyFall:
				worldTransformEnemy_.rotation_.y += 0.5f;
				if (particleManager_) {
					particleManager_->Emit(worldTransformEnemy_.translation_, {0, 0.2f, 0}, {0, 0, 0}, 1.0f, 0.5f, 0.0f, {1.0f, 0.5f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 0.0f});
				}
				if (onGround_) {
					bossState_ = BossState::kCoolDown;
					bossTimer_ = 0.0f;
					velocity_.x = 0.0f;
					worldTransformEnemy_.rotation_.y = std::numbers::pi_v<float> * 1.5f;

					// メテオ着弾爆発
					if (particleManager_) {
						for (int i = 0; i < 30; i++) {
							Vector3 vel = {
							    (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 1.0f, (static_cast<float>(rand()) / RAND_MAX) * 0.8f, (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 1.0f};
							particleManager_->Emit(worldTransformEnemy_.translation_, vel, {0, -0.02f, 0}, 1.0f, 1.5f, 0.0f, {1.0f, 0.2f, 0.0f, 1.0f}, {0.5f, 0.0f, 0.0f, 0.0f});
						}
					}
					if (isPhase2_) {
						for (int i = -1; i <= 1; i += 2) {
							EnemyBullet* shockwave = new EnemyBullet();
							shockwave->Initialize(model_, worldTransformEnemy_.translation_ + Vector3(0, 0.5f, 0), {(float)i * 0.4f, 0, 0}, mapChipField_);
							bullets_.push_back(shockwave);
						}
					}
				}
				break;

			// 14. 回転攻撃
			case BossState::kSpinAttack:
				bossTimer_ += 1.0f / 60.0f;
				worldTransformEnemy_.rotation_.y += 0.5f;
				velocity_.x = std::sin(bossTimer_ * 10.0f) * 0.4f;

				if (particleManager_) {
					Vector3 pos = worldTransformEnemy_.translation_ + Vector3(std::cos(worldTransformEnemy_.rotation_.y), 0, std::sin(worldTransformEnemy_.rotation_.y));
					particleManager_->Emit(pos, {0, 0.1f, 0}, {0, 0, 0}, 0.5f, 0.3f, 0.0f, {1.0f, 1.0f, 1.0f, 0.8f}, {1.0f, 1.0f, 1.0f, 0.0f});
				}
				if (onGround_) {
					velocity_.y = 0.2f;
					onGround_ = false;
				}
				if (bossTimer_ >= 3.0f) {
					bossState_ = BossState::kCoolDown;
					bossTimer_ = 0.0f;
					velocity_.x = 0.0f;
					if (player_)
						worldTransformEnemy_.rotation_.y =
						    (player_->GetWorldPosition().x > worldTransformEnemy_.translation_.x) ? std::numbers::pi_v<float> / 2.0f : std::numbers::pi_v<float> * 3.0f / 2.0f;
				}
				break;

			// 15. 硬直 (CoolDown)
			case BossState::kCoolDown:
				bossTimer_ += 1.0f / 60.0f;
				worldTransformEnemy_.scale_ = {1.2f, 0.7f, 1.0f}; // 疲れた感じ

				if (particleManager_ && rand() % 10 == 0) {
					Vector3 pos = worldTransformEnemy_.translation_ + Vector3((static_cast<float>(rand()) / RAND_MAX - 0.5f), 1.0f, 0);
					particleManager_->Emit(pos, {0, -0.05f, 0}, {0, 0, 0}, 0.3f, 0.5f, 0.0f, {0.0f, 0.5f, 1.0f, 0.8f}, {0.0f, 0.5f, 1.0f, 0.0f});
				}
				{
					float coolDownTime = isPhase2_ ? 0.8f : 1.5f;
					if (bossTimer_ >= coolDownTime) {
						bossState_ = BossState::kWait;
						bossTimer_ = 0.0f;
						worldTransformEnemy_.scale_ = {1.0f, 1.0f, 1.0f};
					}
				}
				break;

			// 16. 死亡演出
			case BossState::kDead:
				bossTimer_ += 1.0f / 60.0f;
				velocity_ = {0, 0, 0};

				// フェーズ1: 振動
				if (bossTimer_ < 2.0f) {
					float shakeMagnitude = 0.1f + (bossTimer_ * 0.2f);
					worldTransformEnemy_.translation_ += Vector3(
					    (static_cast<float>(rand()) / RAND_MAX - 0.5f) * shakeMagnitude, (static_cast<float>(rand()) / RAND_MAX - 0.5f) * shakeMagnitude,
					    (static_cast<float>(rand()) / RAND_MAX - 0.5f) * shakeMagnitude);

					if (particleManager_) {
						int count = (bossTimer_ > 1.0f) ? 4 : 1;
						for (int i = 0; i < count; i++) {
							Vector3 pos = worldTransformEnemy_.translation_ + Vector3((static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f, (static_cast<float>(rand()) / RAND_MAX) * 2.0f, 0);
							Vector3 vel = {(static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.5f, (static_cast<float>(rand()) / RAND_MAX) * 0.5f, 0};
							particleManager_->Emit(pos, vel, {0, 0, 0}, 0.6f, 0.4f, 0.0f, {0.8f, 0.0f, 0.5f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f});
						}
					}
				}
				// フェーズ2: 膨張
				else if (bossTimer_ < 3.0f) {
					float t = bossTimer_ - 2.0f;
					float currentScale = 1.0f + (t * 1.5f);
					float distort = std::sin(t * 50.0f) * 0.2f;
					worldTransformEnemy_.scale_ = {currentScale + distort, currentScale - distort, currentScale + distort};

					if (particleManager_) {
						for (int i = 0; i < 5; i++) {
							Vector3 vel = {(static_cast<float>(rand()) / RAND_MAX - 0.5f), (static_cast<float>(rand()) / RAND_MAX - 0.5f), 0};
							particleManager_->Emit(worldTransformEnemy_.translation_, vel, {0, 0, 0}, 1.2f, 0.1f, 0.0f, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 0.0f});
						}
					}
				}
				// フェーズ3: 消滅
				else {
					isDead_ = true;
					if (particleManager_) {
						// 衝撃波
						for (int i = 0; i < 60; i++) {
							float angle = (std::numbers::pi_v<float> * 2.0f / 60.0f) * i;
							particleManager_->Emit(
							    worldTransformEnemy_.translation_, {std::cos(angle), 0, std::sin(angle)}, {0, 0, 0}, 2.0f, 1.0f, 0.0f, {1.0f, 0.5f, 0.8f, 1.0f}, {1.0f, 0.0f, 0.5f, 0.0f});
						}
						// 破片
						for (int i = 0; i < 100; i++) {
							Vector3 vel = {
							    (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.5f, (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.5f, (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.5f};
							particleManager_->Emit(worldTransformEnemy_.translation_, vel, {0, -0.05f, 0}, 1.0f, 2.0f, 0.0f, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.8f, 0.0f});
						}
					}
				}
				break;
			}
		}

		// ==========================================
		// 共通: マップチップ衝突処理 (Physics)
		// ==========================================
		CollisionMapInfo collisionInfo = {};
		collisionInfo.isMovement = velocity_;

		MapChipUp(collisionInfo);
		MapChipDown(collisionInfo);
		MapChipLeft(collisionInfo);
		MapChipRight(collisionInfo);

		worldTransformEnemy_.translation_ += collisionInfo.isMovement;

		if (collisionInfo.isHitTop) {
			velocity_.y = 0;
		}
		if (collisionInfo.hitWall && (type_ == Type::kWalk || type_ == Type::kSplit)) {
			velocity_.x *= -1; // 歩く敵は壁で反転
		}
		UpdateOnGround(collisionInfo);
		break;
	}

	// === 死亡演出中の挙動 ===
	case Enemy::Behavior::kisDead: {
		walkTimer += 1.0f / 60.0f;
		// 回転演出
		worldTransformEnemy_.rotation_.y = math->EaseInOutSine(walkTimer / 1.0f, 0.0f, -std::numbers::pi_v<float> * 4.0f);
		worldTransformEnemy_.rotation_.x = math->EaseInOutSine(walkTimer / 1.0f, 0.0f, -std::numbers::pi_v<float> / 2.0f);

		worldTransformEnemy_.scale_ = (isSplit_) ? Vector3{2.0f, 2.0f, 2.0f} : Vector3{1.0f, 1.0f, 1.0f};

		// 煙パーティクル
		if (particleManager_ && rand() % 5 == 0) {
			Vector3 pos = worldTransformEnemy_.translation_ +
			              Vector3((static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.5f, (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.5f, (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.5f);
			particleManager_->Emit(pos, {0.0f, 0.05f, 0.0f}, {0, 0, 0}, 0.5f, 0.5f, 0.0f, {0.5f, 0.5f, 0.5f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f});
		}

		// 完全に消滅
		if (walkTimer >= 1.0f) {
			isDead_ = true;
			// 最後の爆発
			if (particleManager_) {
				for (int i = 0; i < 8; i++) {
					Vector3 vel = {(static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.5f, (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.5f, (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.5f};
					particleManager_->Emit(worldTransformEnemy_.translation_, vel, {0, 0, 0}, 0.4f, 0.8f, 0.0f, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 0.0f});
				}
			}
		}
		break;
	}
	} // End Switch(Behavior)

	// --- 共通: ダメージ点滅タイマー ---
	if (damageBlinkTimer_ > 0.0f) {
		damageBlinkTimer_ -= 1.0f / 60.0f;
	}

	// --- ボス移動制限 (Arena Boundaries) ---
	if (type_ == Type::kBoss && mapChipField_) {
		float blockW = mapChipField_->GetBlockWidth();
		float minX = blockW * 1.5f;
		float maxX = (mapChipField_->GetNumBlockHorizonal() - 1.5f) * blockW;
		float minY = mapChipField_->GetBlockHeight() * 1.0f;

		if (worldTransformEnemy_.translation_.x < minX) {
			worldTransformEnemy_.translation_.x = minX;
			if (velocity_.x < 0.0f)
				velocity_.x = 0.0f;
			if (bossState_ == BossState::kDash) { /* ダッシュ中断処理 */
				bossState_ = BossState::kCoolDown;
				bossTimer_ = 0.0f;
				worldTransformEnemy_.scale_ = {1, 1, 1};
				worldTransformEnemy_.rotation_.z = 0;
			}
		} else if (worldTransformEnemy_.translation_.x > maxX) {
			worldTransformEnemy_.translation_.x = maxX;
			if (velocity_.x > 0.0f)
				velocity_.x = 0.0f;
			if (bossState_ == BossState::kDash) { /* ダッシュ中断処理 */
				bossState_ = BossState::kCoolDown;
				bossTimer_ = 0.0f;
				worldTransformEnemy_.scale_ = {1, 1, 1};
				worldTransformEnemy_.rotation_.z = 0;
			}
		}

		if (worldTransformEnemy_.translation_.y < minY) {
			worldTransformEnemy_.translation_.y = minY;
			if (velocity_.y < 0.0f)
				velocity_.y = 0.0f;
			onGround_ = true;
		}
	}

	// ------------------------------------------
	// 4. 行列更新 (Update Matrix)
	// ------------------------------------------
	math->worldTransFormUpdate(worldTransformEnemy_);
}

// ==========================================
// 描画処理
// ==========================================
void Enemy::Draw() {
	// 弾
	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw(*camera_);
	}
	// 点滅処理
	if (damageBlinkTimer_ > 0.0f) {
		if (std::fmod(damageBlinkTimer_, 0.1f) < 0.05f) {
			return; // 描画スキップ
		}
	}
	// 本体
	model_->Draw(worldTransformEnemy_, *camera_);
}

// ==========================================
// Getter / Setter / Helpers
// ==========================================
Vector3 Enemy::GetWorldPosition() { return {worldTransformEnemy_.matWorld_.m[3][0], worldTransformEnemy_.matWorld_.m[3][1], worldTransformEnemy_.matWorld_.m[3][2]}; }

AABB Enemy::GetAABB() {
	Vector3 worldPos = GetWorldPosition();
	float w = kWidth * worldTransformEnemy_.scale_.x;
	float h = kHeight * worldTransformEnemy_.scale_.y;
	return {
	    {worldPos.x - w / 2.0f, worldPos.y - h / 2.0f, worldPos.z - w / 2.0f},
        {worldPos.x + w / 2.0f, worldPos.y + h / 2.0f, worldPos.z + w / 2.0f}
    };
}

// ==========================================
// 衝突・イベント処理
// ==========================================
void Enemy::onCollision(const Player* player) {
	if (behavior_ == Behavior::kisDead)
		return;

	isCollisDisabled_ = true;
	behaviorRequest_ = Behavior::kisDead;
	Vector3 effectPos = (GetWorldPosition() + player->GetWorldPosition()) / 2.0f;
	if (gameScene_)
		gameScene_->CreateHitEffect(effectPos);
}

void Enemy::OnStomped(const Player* player) {
	if (behavior_ == Behavior::kisDead)
		return;

	isCollisDisabled_ = true;
	behaviorRequest_ = Behavior::kisDead;
	Vector3 effectPos = (GetWorldPosition() + player->GetWorldPosition()) / 2.0f;
	effectPos.y -= 1.5f;
	if (gameScene_)
		gameScene_->CreateHitEffect(effectPos);
}

void Enemy::TakeDamage(int damage) {
	if (isDead_ || behavior_ == Behavior::kisDead || (type_ == Type::kBoss && bossState_ == BossState::kDead))
		return;

	hp_ -= damage;
	damageBlinkTimer_ = 0.2f;

	if (hp_ <= 0) {
		hp_ = 0;
		isCollisDisabled_ = true;
		if (type_ == Type::kBoss) {
			bossState_ = BossState::kDead;
			bossTimer_ = 0.0f;
		} else {
			behaviorRequest_ = Behavior::kisDead;
		}
		if (gameScene_)
			gameScene_->CreateHitEffect(GetWorldPosition());
	}
}

// ==========================================
// AI / 演出ロジック
// ==========================================
void Enemy::SelectNextAction() {
	if (!isPhase2_ && hp_ <= maxHp_ / 2) {
		bossState_ = BossState::kPhaseChange;
		bossTimer_ = 0.0f;
		return;
	}

	int dice = rand() % 100;
	if (!isPhase2_) {
		if (dice < 40)
			bossState_ = BossState::kCharge;
		else if (dice < 60)
			bossState_ = BossState::kDashCharge;
		else if (dice < 80)
			bossState_ = BossState::kBarrage;
		else
			bossState_ = BossState::kHighJump;
	} else {
		if (dice < 20)
			bossState_ = BossState::kTripleJump;
		else if (dice < 40)
			bossState_ = BossState::kDashCharge;
		else if (dice < 60)
			bossState_ = BossState::kSpinAttack;
		else if (dice < 80)
			bossState_ = BossState::kBarrage;
		else
			bossState_ = BossState::kSummon;
	}
	bossTimer_ = 0.0f;
}

void Enemy::EmitExplosion() {
	if (!particleManager_)
		return;
	for (int i = 0; i < 30; i++) {
		Vector3 vel = {(static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.8f, (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.8f, (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.8f};
		particleManager_->Emit(worldTransformEnemy_.translation_, vel, {0.0f, -0.01f, 0.0f}, 0.6f, 1.5f, 0.0f, {1.0f, 1.0f, 0.2f, 1.0f}, {1.0f, 0.0f, 0.0f, 0.0f});
	}
}

// ==========================================
// 物理・判定ユーティリティ
// ==========================================
bool Enemy::IsWalkable(MapChipType type) {
	return (
	    type == MapChipType::kDirt_ || type == MapChipType::kGrass_ || type == MapChipType::kJumpPad_ || type == MapChipType::kBreakable_ || type == MapChipType::kCloud_ ||
	    type == MapChipType::kWallBreak_);
}

Vector3 Enemy::CarnerPosition(const Vector3& center, Corner cornter) {
	float w = kWidth * worldTransformEnemy_.scale_.x;
	float h = kHeight * worldTransformEnemy_.scale_.y;
	Vector3 offSetTable[] = {
	    {+w / 2.0f, -h / 2.0f, 0},
        {-w / 2.0f, -h / 2.0f, 0},
        {+w / 2.0f, +h / 2.0f, 0},
        {-w / 2.0f, +h / 2.0f, 0}
    };
	return center + offSetTable[static_cast<uint32_t>(cornter)];
}

void Enemy::MapChipUp(CollisionMapInfo& info) {
	if (info.isMovement.y <= 0)
		return;
	std::array<Vector3, knumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i)
		positionsNew[i] = CarnerPosition(worldTransformEnemy_.translation_ + info.isMovement, static_cast<Corner>(i));

	bool hit = false;
	MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	if (IsWalkable(mapChipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex)))
		hit = true;
	if (!hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
		if (IsWalkable(mapChipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex)))
			hit = true;
	}
	if (hit) {
		info.isMovement.y = 0.0f;
		info.isHitTop = true;
	}
}

void Enemy::MapChipDown(CollisionMapInfo& info) {
	if (info.isMovement.y >= 0)
		return;
	std::array<Vector3, knumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i)
		positionsNew[i] = CarnerPosition(worldTransformEnemy_.translation_ + info.isMovement, static_cast<Corner>(i));

	bool hit = false;
	MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	if (IsWalkable(mapChipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex)))
		hit = true;
	if (!hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
		if (IsWalkable(mapChipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex)))
			hit = true;
	}
	if (hit) {
		info.isMovement.y = 0.0f;
		info.isHitBottom = true;
	}
}

void Enemy::MapChipLeft(CollisionMapInfo& info) {
	if (info.isMovement.x >= 0)
		return;
	std::array<Vector3, knumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i)
		positionsNew[i] = CarnerPosition(worldTransformEnemy_.translation_ + info.isMovement, static_cast<Corner>(i));

	bool hit = false;
	MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	if (IsWalkable(mapChipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex)))
		hit = true;
	if (!hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
		if (IsWalkable(mapChipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex)))
			hit = true;
	}
	if (hit) {
		info.isMovement.x = 0.0f;
		info.hitWall = true;
	}
}

void Enemy::MapChipRight(CollisionMapInfo& info) {
	if (info.isMovement.x <= 0)
		return;
	std::array<Vector3, knumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i)
		positionsNew[i] = CarnerPosition(worldTransformEnemy_.translation_ + info.isMovement, static_cast<Corner>(i));

	bool hit = false;
	MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	if (IsWalkable(mapChipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex)))
		hit = true;
	if (!hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
		if (IsWalkable(mapChipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex)))
			hit = true;
	}
	if (hit) {
		info.isMovement.x = 0.0f;
		info.hitWall = true;
	}
}

void Enemy::UpdateOnGround(const CollisionMapInfo& info) {
	if (onGround_) {
		std::array<Vector3, knumCorner> positionsCheck;
		for (uint32_t i = 0; i < positionsCheck.size(); ++i) {
			positionsCheck[i] = CarnerPosition(worldTransformEnemy_.translation_ + Vector3(0, -kGroundSearchHeight, 0), static_cast<Corner>(i));
		}
		bool hitSearch = false;
		MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsCheck[kLeftBottom]);
		if (IsWalkable(mapChipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex)))
			hitSearch = true;
		if (!hitSearch) {
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsCheck[kRightBottom]);
			if (IsWalkable(mapChipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex)))
				hitSearch = true;
		}
		if (!hitSearch)
			onGround_ = false;
	} else {
		if (info.isHitBottom)
			onGround_ = true;
	}
}

void Enemy::UpdateOnWall(const CollisionMapInfo& info) {
	if (info.hitWall)
		velocity_.x *= -1;
}