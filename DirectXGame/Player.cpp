#define NOMINMAX
#include "Player.h"
#include "MapChipField.h" // MapChipField クラスの定義が必要
#include "cassert"
#include <algorithm>   // std::clamp, std::max, std::min のために必要
#include <numbers>     // std::numbers::pi_v のために必要

using namespace KamataEngine;

void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	camera_ = camera;
	model_ = model;

	worldTransformPlayer_.Initialize();
	worldTransformPlayer_.translation_ = position;
	worldTransformPlayer_.rotation_.y = std::numbers::pi_v<float> / 2.0f; // 初期向きを右に設定

	// Mathクラスの初期化（注意：シングルトンや依存性注入を推奨）
	if (!math) {
		math = new Math();
	}
	onGround_ = false; // 初期状態は空中または地面にいるか不明とする
}

Vector3 Player::GetWorldPosition() 
{
	//ワールド座標を入れる変数
	Vector3 worldPos;
	//ワールド行列の平行移動成分を取得
	worldPos.x = worldTransformPlayer_.matWorld_.m[3][0]; // 行列の4行1列目
	worldPos.y = worldTransformPlayer_.matWorld_.m[3][1]; // 行列の4行2列目
	worldPos.z = worldTransformPlayer_.matWorld_.m[3][2]; // 行列の4行3列目
	    return worldPos;

}

// 移動処理（先生のInputMoveロジックを統合）
void Player::Move() {
	if (onGround_) { // 接地状態の場合
		// 左右移動操作
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				if (velosity_.x < 0.0f) {
					velosity_.x *= (1.0f - kAtteunuation); // 旋回最初は減衰
				}
				acceleration.x += kAcceleration / 60.0f; // フレームレートで割る
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRottationY_ = worldTransformPlayer_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				if (velosity_.x > 0.0f) {
					velosity_.x *= (1.0f - kAtteunuation); // 旋回最初は減衰
				}
				acceleration.x -= kAcceleration / 60.0f; // フレームレートで割る
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRottationY_ = worldTransformPlayer_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			}
			velosity_ += acceleration;
			velosity_.x = std::clamp(velosity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			// 非入力時は移動減衰
			velosity_.x *= (1.0f - kAtteunuation);
		}

		// ほぼ0の場合に0にする
		if (std::abs(velosity_.x) <= 0.0001f) {
			velosity_.x = 0.0f;
		}

		// ジャンプ
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			velosity_.y += kJumpAccleration / 60.0f; // ジャンプ初期速度 (フレームレートで割る)
		}
	} else { // 空中状態の場合
		// 重力適用
		velosity_.y += -kGgravityAcceleration / 60.0f;         // 1フレームあたりの重力加速度
		velosity_.y = std::max(velosity_.y, -kLimitFallSpeed); // 最大落下速度制限
		// 空中での横方向の減衰（必要であれば追加）
		// velosity_.x *= (1.0f - kAirAtteunuation);
	}
}

void Player::Update() {
	// 移動入力と速度計算
	Move();

	// 衝突情報を初期化
	CollisionMapInfo collisionInfo = {};
	collisionInfo.isMovement = velosity_; // 現在の速度を予測移動量としてセット

	// 各方向のマップチップとの当たり判定（衝突によってcollisionInfo.isMovementが調整される）
	
	MapChipUp(collisionInfo);    // 上方向
	MapChipDown(collisionInfo);  // 下方向（地面）
	MapChipLeft(collisionInfo);  // 左方向
	MapChipRight(collisionInfo); // 右方向

	// 調整された移動量をプレイヤーの位置に適用
	worldTransformPlayer_.translation_ += collisionInfo.isMovement;

	// 天井接触によるY速度のリセット
	if (collisionInfo.isHitTop) {
		velosity_.y = 0;
	}

	// 壁接触時の横方向減衰
	UpdateOnWall(collisionInfo);

	// 接地状態の更新
	UpdateOnGround(collisionInfo);

	// 旋回制御
	if (turnTimer_ > 0.0f) {
		turnTimer_ = std::max(turnTimer_ - (1.0f / 60.0f), 0.0f); // 1フレームあたりの時間を減らす

		float destinationRotationYTable[] = {
		    std::numbers::pi_v<float> / 2.0f,       // kRight
		    std::numbers::pi_v<float> * 3.0f / 2.0f // kLeft
		};
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		float t = 1.0f - (turnTimer_ / kTimeTurn); // 0.0fから1.0fへ変化する補間係数
		worldTransformPlayer_.rotation_.y = math->EaseInOutSine(t, turnFirstRottationY_, destinationRotationY);
	}

	// ワールド行列更新
	math->worldTransFormUpdate(worldTransformPlayer_); // Math::worldTransFormUpdateがMathクラスのメソッドの場合
}

void Player::Draw() {
	assert(model_);
	assert(camera_);
	model_->Draw(worldTransformPlayer_, *camera_);
}

// 各方向のマップチップとの当たり判定 (先生のロジックをベースに)
void Player::MapChipUp(CollisionMapInfo& info) {
	// 上昇なし？
	if (info.isMovement.y <= 0) {
		return;
	}

	std::array<Vector3, knumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CarnerPosition(worldTransformPlayer_.translation_ + info.isMovement, static_cast<Corner>(i));
	}

	MapChipType mapchipType;
	bool hit = false;
	MapChipField::IndexSet indexSet; // 衝突したマップチップのインデックスを保持するため

	// 左上点の判定
	indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[ kLeftTop]);
	mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
	if (mapchipType == MapChipType::kBlock_) {
		hit = true;
	}

	// 右上点の判定
	if (!hit) { // 左上がヒットしていない場合のみチェック
		indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
		mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
		if (mapchipType == MapChipType::kBlock_) {
			hit = true;
		}
	}

	if (hit) {
		indexSet = mapchipField_->GetMapChipIndexSetByPosition(worldTransformPlayer_.translation_ + info.isMovement + Vector3(0, +kHeight / 2.0f, 0));
		// 現在座標が壁の外か判定（すでにブロック内にめり込んでいる場合）
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapchipField_->GetMapChipIndexSetByPosition(worldTransformPlayer_.translation_ + Vector3(0, +kHeight / 2.0f, 0));

		// 衝突したマップチップのYインデックスと現在のYインデックスが異なる場合（つまり、新しいマップチップに当たろうとしている）
		if (indexSetNow.yIndex != indexSet.yIndex) {
		
			MapChipField::Rect rect = mapchipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// めり込みを排除する方向に移動量を設定する (天井の底面に合わせる)
			info.isMovement.y = std::max(0.0f, rect.bottom - (worldTransformPlayer_.translation_.y + kHeight / 2.0f) - kBlank);
			info.isHitTop = true;
		} else {
			// すでに同じブロック内にいる場合は、めり込み量を0に
			info.isMovement.y = 0.0f;
			info.isHitTop = true;
		}
	}
}

void Player::MapChipDown(CollisionMapInfo& info) {
	// 下降なし？
	if (info.isMovement.y >= 0) {
		return;
	}

	std::array<Vector3, knumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CarnerPosition(worldTransformPlayer_.translation_ + info.isMovement, static_cast<Corner>(i));
	}

	MapChipType mapchipType;
	bool hit = false;
	MapChipField::IndexSet indexSet; // 衝突したマップチップのインデックスを保持するため

	// 左下点の判定
	indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
	if (mapchipType == MapChipType::kBlock_) {
		hit = true;
	}

	// 右下点の判定
	if (!hit) { // 左下がヒットしていない場合のみチェック
		indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
		mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
		if (mapchipType == MapChipType::kBlock_) {
			hit = true;
		}
	}

	if (hit) {
		indexSet = mapchipField_->GetMapChipIndexSetByPosition(worldTransformPlayer_.translation_ + info.isMovement + Vector3(0, -kHeight / 2.0f, 0));
		// 現在座標が壁の外か判定（すでにブロック内にめり込んでいる場合）
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapchipField_->GetMapChipIndexSetByPosition(worldTransformPlayer_.translation_ + Vector3(0, -kHeight / 2.0f, 0));

		// 衝突したマップチップのYインデックスと現在のYインデックスが異なる場合
		if (indexSetNow.yIndex != indexSet.yIndex) {
			
			MapChipField::Rect rect = mapchipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// めり込みを排除する方向に移動量を設定する (地面の上面に合わせる)
			info.isMovement.y = std::min(0.0f, rect.top - (worldTransformPlayer_.translation_.y - kHeight / 2.0f) + kBlank);
			info.isHitBottom = true;
		} else {
			// すでに同じブロック内にいる場合は、めり込み量を0に
			info.isMovement.y = 0.0f;
			info.isHitBottom = true;
		}
	}
}

void Player::MapChipLeft(CollisionMapInfo& info) {
	// 左移動なし？
	if (info.isMovement.x >= 0) {
		return;
	}

	std::array<Vector3, knumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CarnerPosition(worldTransformPlayer_.translation_ + info.isMovement, static_cast<Corner>(i));
	}

	MapChipType mapchipType;
	bool hit = false;
	MapChipField::IndexSet indexSet; // 衝突したマップチップのインデックスを保持するため

	// 左上点の判定
	indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
	if (mapchipType == MapChipType::kBlock_) {
		hit = true;
	}

	// 左下点の判定
	if (!hit) { // 左上がヒットしていない場合のみチェック
		indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
		mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
		if (mapchipType == MapChipType::kBlock_) {
			hit = true;
		}
	}

	if (hit) {
		indexSet = mapchipField_->GetMapChipIndexSetByPosition(worldTransformPlayer_.translation_ + info.isMovement + Vector3(-kWidth / 2.0f, 0, 0));
		// 現在座標が壁の外か判定（すでにブロック内にめり込んでいる場合）
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapchipField_->GetMapChipIndexSetByPosition(worldTransformPlayer_.translation_ + Vector3(-kWidth / 2.0f, 0, 0));

		// 衝突したマップチップのXインデックスと現在のXインデックスが異なる場合
		if (indexSetNow.xIndex != indexSet.xIndex) {
		
			MapChipField::Rect rect = mapchipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// めり込みを排除する方向に移動量を設定する (壁の右面に合わせる)
			info.isMovement.x = std::max(0.0f, rect.right - (worldTransformPlayer_.translation_.x - kWidth / 2.0f) + kBlank);
			info.hitWall = true;
		} else {
			// すでに同じブロック内にいる場合は、めり込み量を0に
			info.isMovement.x = 0.0f;
			info.hitWall = true;
		}
	}
}

void Player::MapChipRight(CollisionMapInfo& info) {
	// 右移動なし？
	if (info.isMovement.x <= 0) {
		return;
	}

	std::array<Vector3, knumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CarnerPosition(worldTransformPlayer_.translation_ + info.isMovement, static_cast<Corner>(i));
	}

	MapChipType mapchipType;
	bool hit = false;
	MapChipField::IndexSet indexSet; // 衝突したマップチップのインデックスを保持するため

	// 右上点の判定
	indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
	if (mapchipType == MapChipType::kBlock_) {
		hit = true;
	}

	// 右下点の判定
	if (!hit) { // 右上がヒットしていない場合のみチェック
		indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
		mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
		if (mapchipType == MapChipType::kBlock_) {
			hit = true;
		}
	}

	if (hit) {

		indexSet = mapchipField_->GetMapChipIndexSetByPosition(worldTransformPlayer_.translation_ + info.isMovement + Vector3(+kWidth / 2.0f, 0, 0));

		// 現在座標が壁の外か判定（すでにブロック内にめり込んでいる場合）
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapchipField_->GetMapChipIndexSetByPosition(worldTransformPlayer_.translation_ + Vector3(+kWidth / 2.0f, 0, 0));

		// 衝突したマップチップのXインデックスと現在のXインデックスが異なる場合
		if (indexSetNow.xIndex != indexSet.xIndex) {
			MapChipField::Rect rect = mapchipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// めり込みを排除する方向に移動量を設定する (壁の左面に合わせる)
			info.isMovement.x = std::min(0.0f, rect.left - (worldTransformPlayer_.translation_.x + kWidth / 2.0f) + kBlank);
			info.hitWall = true;
		} else {
			// すでに同じブロック内にいる場合は、めり込み量を0に
			info.isMovement.x = 0.0f;
			info.hitWall = true;
		}
	}
}

// 接地状態の切り替え処理 (先生のUpdateOnGroundに相当)
void Player::UpdateOnGround(const CollisionMapInfo& info) {
	if (onGround_) {
		// ジャンプ開始時、Y速度が正なら非接地状態に
		if (velosity_.y > 0.0f) {
			onGround_ = false;
		} else {
			// 落下判定: 現在の位置から少し下のマップチップを探索
			std::array<Vector3, knumCorner> positionsCheck;
			for (uint32_t i = 0; i < positionsCheck.size(); ++i) {
				positionsCheck[i] = CarnerPosition(worldTransformPlayer_.translation_ + Vector3(0, -kGroundSearchHeight, 0), static_cast<Corner>(i));
			}

			bool hitSearch = false;
			MapChipType mapchipType;
			MapChipField::IndexSet indexSet;

			// 左下と右下の探索点でブロックをチェック
			indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsCheck[kLeftBottom]);
			mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
			if (mapchipType == MapChipType::kBlock_) {
				hitSearch = true;
			}

			if (!hitSearch) { // 左下がヒットしていない場合のみチェック
				indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsCheck[kRightBottom]);
				mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
				if (mapchipType == MapChipType::kBlock_) {
					hitSearch = true;
				}
			}

			// 足元にブロックがない場合、落下開始（非接地状態へ）
			if (!hitSearch) {
				onGround_ = false;
			}
		}
	} else {
		// 非接地状態の場合、地面に接触したかで接地状態に切り替える
		if (info.isHitBottom) {
			onGround_ = true;
			velosity_.x *= (1.0f - kAttenuationLanding); // 着地時の横方向減衰
			velosity_.y = 0.0f;                          // Y速度をゼロに
		}
	}
}

// 壁接触時の処理 (先生のUpdateOnWallに相当)
void Player::UpdateOnWall(const CollisionMapInfo& info) {
	if (info.hitWall) {
		velosity_.x *= (1.0f - kAttenuationWall); // 壁接触時の横方向減衰
	}
}

// キャラクターの四隅の座標を計算
Vector3 Player::CarnerPosition(const Vector3& center, Corner cornter) {
	Vector3 offSetTable[] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, //  kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, //  kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, //  kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}  //  kLeftTop
	};

	return center + offSetTable[static_cast<uint32_t>(cornter)];
}

AABB Player::GetAABB() 
{
	Vector3 worldPos = GetWorldPosition();
	AABB aabb;
	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};
	return aabb;
}

void Player::OnCollision(const Enemy* enemy) 
{
	(void)enemy;
	velosity_ += Vector3(0.0f, 0.1f, 0.0f);
}


