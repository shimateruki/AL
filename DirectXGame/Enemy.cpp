#define NOMINMAX
#include "Enemy.h"
#include "GameScene.h"
#include "GameScene1_2.h"
#include "GameScene1_3.h"
#include"GameScene2_1.h"
#include "Player.h"
#include <cassert>
#include <numbers>
#include <algorithm> // std::clamp, std::max, std::min




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
void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	model_ = model;
	camera_ = camera;
	worldTransformEnemy_.Initialize();
	worldTransformEnemy_.translation_ = position;
	worldTransformEnemy_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
	velocity_ = {-kWalkSpeed, 0, 0};
	walkTimer = 0.0f;
	gameScene_ = nullptr;
}

void Enemy::Update() {
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

	switch (behavior_) {
	case Enemy::Behavior::kUnKnow:
		// 特に処理なし
		break;
	case Enemy::Behavior::kRoot: {

		// ▼▼▼ ここから新しいマップチップ当たり判定のロジック ▼▼▼

		// 重力適用
		if (!onGround_) {
			velocity_.y += -kGgravityAcceleration / 60.0f;
			velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
		} else {
			velocity_.y = 0.0f;
		}

		// 崖の端で引き返すロジック
		// プレイヤーのUpdateOnGround()の落下判定を応用する
		if (onGround_) {
			// 足元を探索する座標を計算
			Vector3 pos = GetWorldPosition();
			Vector3 positionsCheck = pos + Vector3(0, -kGroundSearchHeight, 0);
			positionsCheck.x += (velocity_.x > 0 ? kWidth / 2.0f : -kWidth / 2.0f); // 進む方向の足元をチェック

			MapChipType mapchipType = MapChipType::kBlank_; // 初期化
			MapChipField::IndexSet indexSet;
			if (mapChipField_) {
				indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsCheck);
				mapchipType = mapChipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
			}

			// 足元に歩けるマップチップがない場合
			if (!IsWalkable(mapchipType)) {
				if (flipCooldownTimer <= 0) {
					velocity_.x *= -1;
					flipCooldownTimer = kFlipCooldown; // タイマーをリセット
				}
			}
		}

		// 衝突情報を初期化し、現在の速度を移動量として設定
		CollisionMapInfo collisionInfo = {};
		collisionInfo.isMovement = velocity_;

		// 各方向のマップチップとの当たり判定
		MapChipUp(collisionInfo);
		MapChipDown(collisionInfo);
		MapChipLeft(collisionInfo);
		MapChipRight(collisionInfo);

		// 調整された移動量を敵の位置に適用
		worldTransformEnemy_.translation_ += collisionInfo.isMovement;

		// 天井にぶつかったらY速度を0にする
		if (collisionInfo.isHitTop) {
			velocity_.y = 0;
		}

		// 壁にぶつかったらX速度を反転する
		if (collisionInfo.hitWall) {
			velocity_.x *= -1;
		}

		// 接地状態の更新
		UpdateOnGround(collisionInfo);

		// ▲▲▲ 新しいロジックここまで ▲▲▲

		// アニメーション用の回転処理
		walkTimer += 1.0f / 60.0f;
		worldTransformEnemy_.rotation_.y = (velocity_.x > 0) ? std::numbers::pi_v<float> / 2.0f : std::numbers::pi_v<float> * 3.0f / 2.0f;
		worldTransformEnemy_.rotation_.x = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer / 1.0f); // 1.0fはモーションの時間
	}
		break;
	case Enemy::Behavior::kisDead:
		walkTimer += 1.0f / 60.0f;
		worldTransformEnemy_.rotation_.y = math->EaseInOutSine(walkTimer / 1.0f, 0.0f, -std::numbers::pi_v<float> / 2.0f);
		worldTransformEnemy_.rotation_.x = math->EaseInOutSine(walkTimer / 1.0f, 0.0f, -std::numbers::pi_v<float> / 2.0f);
		if (walkTimer >= 1.0f) {
			isDead_ = true;
		}
		break;
	}

	math->worldTransFormUpdate(worldTransformEnemy_);
}

void Enemy::Draw() { model_->Draw(worldTransformEnemy_, *camera_); }

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
	} else if (gameScene1_2_) {
		gameScene1_2_->CreateHitEffect(effectPos);
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
	} else if (gameScene1_2_) {
		gameScene1_2_->CreateHitEffect(effectPos);
	} else if (gameScene1_3_) {
		gameScene1_3_->CreateHitEffect(effectPos);
	} else if (gameScene2_1_) {
		gameScene2_1_->CreateHitEffect(effectPos);
	}
}
//-----------------------------------------------------------------------------
// ユーティリティ
//-----------------------------------------------------------------------------
bool Enemy::IsWalkable(MapChipType type) { return (type == MapChipType::kDirt_ || type == MapChipType::kGrass_ || type == MapChipType::kJumpPad_||type==MapChipType::kBreakable_); }

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

// プレイヤーのUpdateOnGroundと同様の処理
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
