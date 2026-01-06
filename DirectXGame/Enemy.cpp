#define NOMINMAX
#include "Enemy.h"
#include "GameScene.h"
#include "Player.h"
#include <algorithm> // std::clamp, std::max, std::min
#include <cassert>
#include <numbers>

using namespace KamataEngine;

//-----------------------------------------------------------------------------
// 定数
//-----------------------------------------------------------------------------
const float kWalkSpeed = 0.1f;
const float kLimitFallSpeed = 0.5f;
const float kGgravityAcceleration = 0.8f;

//-----------------------------------------------------------------------------
// 初期化・更新・描画
//-----------------------------------------------------------------------------
void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position, Type type) {
	assert(model);
	model_ = model;
	camera_ = camera;
	worldTransformEnemy_.Initialize();
	worldTransformEnemy_.translation_ = position;
	worldTransformEnemy_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
	velocity_ = {-kWalkSpeed, 0, 0};
	walkTimer = 0.0f;
	gameScene_ = nullptr;
	type_ = type;
	shotTimer_ = 0.0f;
	startPosition_ = position; // 初期位置を記憶
	flightTimer_ = 0.0f;
	if (type_ == Type::kSplit) {
		isSplit_ = true;
	} else {
		isSplit_ = false;
	}
	if (type_ == Type::kWalk) {
		hp_ = 2;
	} else if (type_ == Type::kShooter) {
		hp_ = 2;
	} else if (type_ == Type::kFlying) {
		hp_ = 3;
	} else if (type_ == Type::kHoming) {
		hp_ = 2;
	} else if (type_ == Type::kSlime) {
		hp_ = 3; // スライムはちょっとタフ？
		jumpWaitTimer_ = 0.0f;
		velocity_ = {0, 0, 0}; // 最初は止まっている
	}
	if (type_ == Type::kSplit) {
		worldTransformEnemy_.scale_ = {2.0f, 2.0f, 2.0f};
		hp_ = 5;
	} else {
		worldTransformEnemy_.scale_ = {1.0f, 1.0f, 1.0f};
	}
}

void Enemy::Update() {
	// 1. 弾の更新
	for (auto it = bullets_.begin(); it != bullets_.end();) {
		(*it)->Update();
		if ((*it)->IsDead()) {
			delete (*it);
			it = bullets_.erase(it);
		} else {
			++it;
		}
	}

	// 2. 行動状態遷移
	if (flipCooldownTimer > 0) {
		flipCooldownTimer -= 1.0f / 60.0f;
	}
	if (behaviorRequest_ != Behavior::kUnKnow) {
		behavior_ = behaviorRequest_;
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			break;
		case Behavior::kisDead:
			walkTimer = 0.0f;
			break;
		}
		behaviorRequest_ = Behavior::kUnKnow;
	}

	// 3. 行動ごとの更新
	switch (behavior_) {
	case Enemy::Behavior::kUnKnow:
		break;

	case Enemy::Behavior::kRoot: {
		// --- 共通: 重力適用 ---
		if (!onGround_) {
			velocity_.y += -kGgravityAcceleration / 60.0f;
			velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
		} else {
			velocity_.y = 0.0f;
		}

		// ==========================================
		// タイプ別の挙動
		// ==========================================
		if (type_ == Type::kWalk || type_ == Type::kSplit) {
			// ★歩く敵のロジック

			// 崖の端で引き返す
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
				if (!IsWalkable(mapchipType)) {
					if (flipCooldownTimer <= 0) {
						velocity_.x *= -1;
						flipCooldownTimer = kFlipCooldown;
					}
				}
			}
			// アニメーション
			walkTimer += 1.0f / 60.0f;
			worldTransformEnemy_.rotation_.y = (velocity_.x > 0) ? std::numbers::pi_v<float> / 2.0f : std::numbers::pi_v<float> * 3.0f / 2.0f;
			worldTransformEnemy_.rotation_.x = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer / 1.0f);

		} else if (type_ == Type::kShooter) {
			// ★撃つ敵のロジック
			velocity_.x = 0.0f;

			// タイマーを進める
			shotTimer_ += 1.0f / 60.0f;

			// 発射までの残り時間
			float timeToShoot = kShotInterval - shotTimer_;
			const float kTelegraphTime = 0.5f; // 発射0.5秒前から予兆

			// ▼▼▼ 予兆アニメーション処理 ▼▼▼
			if (timeToShoot <= kTelegraphTime && timeToShoot > 0.0f) {
				// 発射直前！
				float t = 1.0f - (timeToShoot / kTelegraphTime);

				// 1. ムギュッと潰れる
				float squashY = 1.0f - (t * 0.4f);
				float stretchXZ = 1.0f + (t * 0.3f);
				worldTransformEnemy_.scale_ = {stretchXZ, squashY, stretchXZ};

				// 2. プルプル震える
				float shakeAmount = t * 0.05f;
				float offsetX = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * shakeAmount;
				worldTransformEnemy_.translation_.x = startPosition_.x + offsetX;

			} else {
				// 通常時
				float breathe = std::sin(shotTimer_ * 5.0f) * 0.05f;
				worldTransformEnemy_.scale_ = {1.0f - breathe, 1.0f + breathe, 1.0f - breathe};
				worldTransformEnemy_.translation_.x = startPosition_.x;
			}
		

			if (player_) {
				// ※ここでの diff 計算などは、震えた後の translation_ を使って問題ありません
				Vector3 myPos = worldTransformEnemy_.translation_;
				Vector3 targetPos = player_->GetWorldPosition();
				Vector3 diff = targetPos - myPos;
				if (diff.x > 0)
					worldTransformEnemy_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
				else
					worldTransformEnemy_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
				worldTransformEnemy_.rotation_.x = 0.0f;

				float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
				const float kAttackRange = 15.0f;
				if (dist <= kAttackRange) {
					// shotTimer_ は上で加算済み
					if (shotTimer_ >= kShotInterval) {
						shotTimer_ = 0.0f;
						if (dist > 0.0f) {
							EnemyBullet* newBullet = new EnemyBullet();
							Vector3 velocity = diff;
							velocity.x /= dist;
							velocity.y /= dist;
							velocity.z /= dist;
							velocity *= 0.2f;

							// 弾の発射位置
							newBullet->Initialize(model_, myPos, velocity, mapChipField_);
							bullets_.push_back(newBullet);

							// ★【修正点3】反動で戻すときもX軸だけ戻すか、何もしない
							worldTransformEnemy_.scale_ = {1.0f, 1.0f, 1.0f};
							// worldTransformEnemy_.translation_ = startPosition_; // ←これ消すか、Xだけにする
							worldTransformEnemy_.translation_.x = startPosition_.x;
						}
					}
				} else {
					shotTimer_ += 1.0f / 60.0f;
				}
			
			}
		} else if (type_ == Type::kFlying) {
			// ★飛行する敵
			// (こいつは自分で座標を更新して終わるタイプなので、ここでリターンしてOK)

			velocity_.x = 0.0f;
			velocity_.y = 0.0f;
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
				if (moveDir > 0)
					worldTransformEnemy_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
				else
					worldTransformEnemy_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
			}
			// ここで return することで、下の「共通移動処理」が二重にかかるのを防ぐ
			math->worldTransFormUpdate(worldTransformEnemy_);
			return;

		} else if (type_ == Type::kHoming) {
			// ★自爆する敵
			// (こいつは自分でマップ判定を持っているので、ここでリターンしてOK)

			if (!onGround_) {
				velocity_.y += -kGgravityAcceleration / 60.0f;
				velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
			} else {
				velocity_.y = 0.0f;
			}

			if (player_) {
				// ... (プレイヤー追尾、振動演出など) ...
				Vector3 myPos = worldTransformEnemy_.translation_;
				Vector3 targetPos = player_->GetWorldPosition();
				Vector3 diff = targetPos - myPos;
				float distSq = (diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z);

				if (distSq < (kDetectionRange * kDetectionRange)) {
					float runSpeed = 0.08f;
					if (diff.x > 0) {
						velocity_.x = runSpeed;
						worldTransformEnemy_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
					} else {
						velocity_.x = -runSpeed;
						worldTransformEnemy_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
					}

					explosionTimer_ += 1.0f / 60.0f;
					if (explosionTimer_ >= kExplosionTime)
						isReadyToExplode_ = true;

					float ratio = std::min(explosionTimer_ / kExplosionTime, 1.0f);
					float shakeSpeed = 40.0f + (ratio * 60.0f);
					float shakeAmount = 0.1f + (ratio * 0.2f);
					homingTimer_ += 1.0f / 60.0f;
					float shake = std::sin(homingTimer_ * shakeSpeed) * shakeAmount;
					worldTransformEnemy_.scale_ = {1.0f + shake, 1.0f + shake, 1.0f + shake};

				} else {
					velocity_.x = 0.0f;
					homingTimer_ += 1.0f / 60.0f;
					float breathe = std::sin(homingTimer_ * 3.0f) * 0.05f;
					worldTransformEnemy_.scale_ = {1.0f + breathe, 1.0f - breathe, 1.0f + breathe};
				}
			}

			// マップチップ衝突（kHoming専用）
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
				velocity_.y = 0.35f;
			UpdateOnGround(collisionInfo);

			// ここで return することで、下の「共通移動処理」が二重にかかるのを防ぐ
			math->worldTransFormUpdate(worldTransformEnemy_);
			return;

		}
		// ==========================================
		// スライムのロジック (Homingの外に出した)
		// ==========================================
		else if (type_ == Type::kSlime) {
			if (onGround_) {
				// 1. 地面にいるときは摩擦で止まる
				velocity_.x = 0.0f;

				// 2. タイマーを進める
				jumpWaitTimer_ += 1.0f;

				// 3. 一定時間（60フレーム＝約1秒）経ったらジャンプ！
				const float kJumpInterval = 60.0f;
				if (jumpWaitTimer_ >= kJumpInterval) {

					// 上に跳ねる
					velocity_.y = 0.6f; // ジャンプ力
					onGround_ = false;  // 接地フラグを切る

					// プレイヤーの方向に跳ぶ
					float moveSpeed = 0.15f; // 横の移動速度
					if (player_) {
						// プレイヤーが自分より右にいるか左にいるか判定
						if (player_->GetWorldPosition().x > worldTransformEnemy_.translation_.x) {
							velocity_.x = moveSpeed; // 右へ
						} else {
							velocity_.x = -moveSpeed; // 左へ
						}
					} else {
						velocity_.x = -moveSpeed; // プレイヤーがいなければ左へ
					}

					// タイマーリセット (ランダム性を持たせても面白いです)
					jumpWaitTimer_ = 0.0f;
				}
			} else {
				// 空中にいる間は横速度を維持（特に何もしなくてOK）
			}
		} else if (type_ == Type::kFlee) {
			// 1. プレイヤーから逃げる方向を決める
			float runSpeed = 0.15f; // 歩く敵より少し速くすると面白い
			if (player_) {
				if (worldTransformEnemy_.translation_.x > player_->GetWorldPosition().x) {
					// 自分が右にいるなら、右へ逃げる
					velocity_.x = runSpeed;
				} else {
					// 自分が左にいるなら、左へ逃げる
					velocity_.x = -runSpeed;
				}
			}

			// 2. 崖の判定（落ちないように端で止まる）
			if (onGround_) {
				Vector3 pos = GetWorldPosition();
				// 進んでいる方向の足元を確認
				Vector3 checkPos = pos + Vector3(velocity_.x > 0 ? kWidth / 2.0f : -kWidth / 2.0f, -kGroundSearchHeight, 0);

				MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(checkPos);
				MapChipType type = mapChipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);

				// もし足元が「歩けない場所（空白など）」なら
				if (!IsWalkable(type)) {
					velocity_.x = 0.0f; // 震えて止まる（追い詰められた！）
				}
			}

			// アニメーション（進行方向を向く）
			if (velocity_.x != 0.0f) {
				walkTimer += 1.0f / 60.0f;
				worldTransformEnemy_.rotation_.y = (velocity_.x > 0) ? std::numbers::pi_v<float> / 2.0f : std::numbers::pi_v<float> * 3.0f / 2.0f;
				worldTransformEnemy_.rotation_.x = std::sin(std::numbers::pi_v<float> * 4.0f * walkTimer); // 焦って速く動く
			}
		}


		// ==========================================
		// 共通: マップチップ衝突判定 (Walk, Split, Shooter, Slime用)
		// ==========================================

		// 1. 移動情報を準備
		CollisionMapInfo collisionInfo = {};
		collisionInfo.isMovement = velocity_;

		// 2. 各方向の当たり判定
		MapChipUp(collisionInfo);
		MapChipDown(collisionInfo);
		MapChipLeft(collisionInfo);
		MapChipRight(collisionInfo);

		// 3. 移動量を適用（これが無いと動かない！）
		worldTransformEnemy_.translation_ += collisionInfo.isMovement;

		// 4. 反射や停止の処理
		if (collisionInfo.isHitTop) {
			velocity_.y = 0;
		}
		if (collisionInfo.hitWall && (type_ == Type::kWalk || type_ == Type::kSplit)) {
			velocity_.x *= -1; // 歩く敵は壁で反転
		}

		// 5. 接地判定
		UpdateOnGround(collisionInfo);

		break;
	}

	// 死亡演出
	case Enemy::Behavior::kisDead:
		walkTimer += 1.0f / 60.0f;
		worldTransformEnemy_.rotation_.y = math->EaseInOutSine(walkTimer / 1.0f, 0.0f, -std::numbers::pi_v<float> / 2.0f);
		worldTransformEnemy_.rotation_.x = math->EaseInOutSine(walkTimer / 1.0f, 0.0f, -std::numbers::pi_v<float> / 2.0f);
		if (walkTimer >= 1.0f) {
			isDead_ = true;
		}
		break;
	}
	if (damageBlinkTimer_ > 0.0f) {
		damageBlinkTimer_ -= 1.0f / 60.0f;
	}
	// 4. 行列更新
	math->worldTransFormUpdate(worldTransformEnemy_);
}

void Enemy::Draw() {
	// 弾の描画
	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw(*camera_);
	}
	if (damageBlinkTimer_ > 0.0f) {
		// 0.05秒周期で点滅 
		if (std::fmod(damageBlinkTimer_, 0.1f) < 0.05f) {

			return;
		}
	}
	// 敵本体の描画
	model_->Draw(worldTransformEnemy_, *camera_);

}

//-----------------------------------------------------------------------------
// Getter / Setter
//-----------------------------------------------------------------------------
Vector3 Enemy::GetWorldPosition() {
	Vector3 worldPos;
	worldPos.x = worldTransformEnemy_.matWorld_.m[3][0];
	worldPos.y = worldTransformEnemy_.matWorld_.m[3][1];
	worldPos.z = worldTransformEnemy_.matWorld_.m[3][2];
	return worldPos;
}

AABB Enemy::GetAABB() {
	Vector3 worldPos = GetWorldPosition();
	AABB aabb;
	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};
	return aabb;
}

//-----------------------------------------------------------------------------
// 衝突
//-----------------------------------------------------------------------------
void Enemy::onCollision(const Player* player) {
	if (behavior_ == Behavior::kisDead) {
		return;
	}
	isCollisDisabled_ = true;
	behaviorRequest_ = Behavior::kisDead;
	Vector3 effectPos = (GetWorldPosition() + player->GetWorldPosition()) / 2.0f;
	if (gameScene_) {
		gameScene_->CreateHitEffect(effectPos);
	}
}

void Enemy::OnStomped(const Player* player) {
	if (behavior_ == Behavior::kisDead)
		return;

	isCollisDisabled_ = true;
	behaviorRequest_ = Behavior::kisDead;
	Vector3 effectPos = (GetWorldPosition() + player->GetWorldPosition()) / 2.0f;
	effectPos.y -= 1.5f; // 少し下にずらす
	if (gameScene_) {
		gameScene_->CreateHitEffect(effectPos);
	}
}
//-----------------------------------------------------------------------------
// ユーティリティ
//-----------------------------------------------------------------------------
bool Enemy::IsWalkable(MapChipType type) { return (type == MapChipType::kDirt_ || type == MapChipType::kGrass_ || type == MapChipType::kJumpPad_ || type == MapChipType::kBreakable_ ||type ==MapChipType::kCloud_||type==MapChipType::kWallBreak_); }

// プレイヤーのCarnerPositionと同様の処理
Vector3 Enemy::CarnerPosition(const Vector3& center, Corner cornter) {
	Vector3 offSetTable[] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, // kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, // kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, // kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}  // kLeftTop
	};
	return center + offSetTable[static_cast<uint32_t>(cornter)];
}

// プレイヤーのMapChipUpと同様の処理
void Enemy::MapChipUp(CollisionMapInfo& info) {
	if (info.isMovement.y <= 0) {
		return;
	}

	std::array<Vector3, knumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CarnerPosition(worldTransformEnemy_.translation_ + info.isMovement, static_cast<Corner>(i));
	}

	MapChipType mapchipType;
	bool hit = false;
	MapChipField::IndexSet indexSet;

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapchipType = mapChipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
	if (IsWalkable(mapchipType)) {
		hit = true;
	}

	if (!hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
		mapchipType = mapChipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
		if (IsWalkable(mapchipType)) {
			hit = true;
		}
	}

	if (hit) {
		info.isMovement.y = 0.0f;
		info.isHitTop = true;
	}
}

// プレイヤーのMapChipDownと同様の処理
void Enemy::MapChipDown(CollisionMapInfo& info) {
	if (info.isMovement.y >= 0) {
		return;
	}

	std::array<Vector3, knumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CarnerPosition(worldTransformEnemy_.translation_ + info.isMovement, static_cast<Corner>(i));
	}

	MapChipType mapchipType;
	bool hit = false;
	MapChipField::IndexSet indexSet;

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapchipType = mapChipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
	if (IsWalkable(mapchipType)) {
		hit = true;
	}

	if (!hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
		mapchipType = mapChipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
		if (IsWalkable(mapchipType)) {
			hit = true;
		}
	}

	if (hit) {
		info.isMovement.y = 0.0f;
		info.isHitBottom = true;
	}
}

// プレイヤーのMapChipLeftと同様の処理
void Enemy::MapChipLeft(CollisionMapInfo& info) {
	if (info.isMovement.x >= 0) {
		return;
	}

	std::array<Vector3, knumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CarnerPosition(worldTransformEnemy_.translation_ + info.isMovement, static_cast<Corner>(i));
	}

	MapChipType mapchipType;
	bool hit = false;
	MapChipField::IndexSet indexSet;

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapchipType = mapChipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
	if (IsWalkable(mapchipType)) {
		hit = true;
	}

	if (!hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
		mapchipType = mapChipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
		if (IsWalkable(mapchipType)) {
			hit = true;
		}
	}

	if (hit) {
		info.isMovement.x = 0.0f;
		info.hitWall = true;
	}
}

// プレイヤーのMapChipRightと同様の処理
void Enemy::MapChipRight(CollisionMapInfo& info) {
	if (info.isMovement.x <= 0) {
		return;
	}

	std::array<Vector3, knumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CarnerPosition(worldTransformEnemy_.translation_ + info.isMovement, static_cast<Corner>(i));
	}

	MapChipType mapchipType;
	bool hit = false;
	MapChipField::IndexSet indexSet;

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapchipType = mapChipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
	if (IsWalkable(mapchipType)) {
		hit = true;
	}

	if (!hit) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
		mapchipType = mapChipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
		if (IsWalkable(mapchipType)) {
			hit = true;
		}
	}

	if (hit) {
		info.isMovement.x = 0.0f;
		info.hitWall = true;
	}
}

void Enemy::UpdateOnGround(const CollisionMapInfo& info) {
	if (onGround_) {
		// 落下判定: 現在の位置から少し下のマップチップを探索
		std::array<Vector3, knumCorner> positionsCheck;
		for (uint32_t i = 0; i < positionsCheck.size(); ++i) {
			positionsCheck[i] = CarnerPosition(worldTransformEnemy_.translation_ + Vector3(0, -kGroundSearchHeight, 0), static_cast<Corner>(i));
		}

		bool hitSearch = false;
		MapChipType mapchipType;
		MapChipField::IndexSet indexSet;

		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsCheck[kLeftBottom]);
		mapchipType = mapChipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
		if (IsWalkable(mapchipType)) {
			hitSearch = true;
		}
		if (!hitSearch) {
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsCheck[kRightBottom]);
			mapchipType = mapChipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
			if (IsWalkable(mapchipType)) {
				hitSearch = true;
			}
		}
		if (!hitSearch) {
			onGround_ = false;
		}
	} else {
		if (info.isHitBottom) {
			onGround_ = true;
		}
	}
}

// プレイヤーのUpdateOnWallと同様の処理
void Enemy::UpdateOnWall(const CollisionMapInfo& info) {
	if (info.hitWall) {
		// 壁にぶつかったら反転する
		velocity_.x *= -1;
	}
}

void Enemy::TakeDamage(int damage) {
	// すでに死んでいたら何もしない
	if (isDead_ || behavior_ == Behavior::kisDead) {
		return;
	}

	// HPを減らす
	hp_ -= damage;
	damageBlinkTimer_ = 0.2f;
	// HPが0以下になったら死亡
	if (hp_ <= 0) {
		hp_ = 0;

		// 死亡状態へ移行
		isCollisDisabled_ = true;
		behaviorRequest_ = Behavior::kisDead;

		// 死亡エフェクト
		if (gameScene_) {
			gameScene_->CreateHitEffect(GetWorldPosition());
		}
	}
}