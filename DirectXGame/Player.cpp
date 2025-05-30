#define NOMINMAX
#include "Player.h"
#include "cassert"
#include <algorithm>
#include <numbers>
#include"MapChipField.h"

using namespace KamataEngine;

void Player::Initialize(Model* model, Camera* camera, Vector3& position) {
	assert(model);
	camera_ = camera;
	model_ = model;

	worldTransformPlayer_.Initialize();
	worldTransformPlayer_.translation_.z =0.0f;
	worldTransformPlayer_.translation_ = position;
	worldTransformPlayer_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	
}

void Player::Move() 
{
	bool landing = false;
	if (onGround_) {

		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				// 左移動中の右入力
				if (velosity_.x < 0.0f) {
					// 速度と逆入力に入力中は急ブレーキ
					velosity_.x *= (1.0f - kAtteunuation);
				}
				acceleration.x += kAcceleration;

				// 左右状態の切り替え
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;

					// 旋回開始時の角度を記録する
					turnFirstRottationY_ = worldTransformPlayer_.rotation_.y;
					// 旋回タイマーに時間を設定する
					turnTimer_ = kTimeTurn;
				}

			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {

				// 左移動中の右入力
				if (velosity_.x > 0.0f) {
					// 速度と逆入力に入力中は急ブレーキ
					velosity_.x *= (1.0f - kAtteunuation);
				}

				acceleration.x -= kAcceleration;

				// 左右状態の切り替え
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;

					// 旋回開始時の角度を記録する
					turnFirstRottationY_ = worldTransformPlayer_.rotation_.y;
					// 旋回タイマーに時間を設定する
					turnTimer_ = kTimeTurn;
				}
			}

			// 加速　減速
			velosity_.x += acceleration.x;
			velosity_.y += acceleration.y;

			// 最大速度制限
			velosity_.x = std::clamp(velosity_.x, -kLimitRunSpeed, kLimitRunSpeed);

		} else {
			velosity_.x *= (1.0f - kAtteunuation);
		}
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			velosity_.y += kJumpAccleration;
			// ジャンプ開始
			if (velosity_.y > 0.0f) {
				// 空中状態に移行
				onGround_ = false;
			}
		}

		// 空中
	} else {
		velosity_.y += -kGgravityAcceleration / 60.0f;
		velosity_.y = std::max(velosity_.y, -kLimitFallSpeed);

		// 地面との当たり判定
		if (velosity_.y < 0) {
			// y座標が地面以下のなったら着地
			if (worldTransformPlayer_.translation_.y <= 1.0f) {
				landing = true;
			}
		}
		// 着地
		if (landing) {
			// めり込み
			worldTransformPlayer_.translation_.y = 1.0f;
			// 摩擦で横方向速度に減衰する
			velosity_.x *= (1.0f - kAtteunuation);
			// 下方向に速度をリセット
			velosity_.y = 0.0f;
			// 着地状態に移行
			onGround_ = true;
		}
	}

	

	

	// 旋回制御
	if (turnTimer_ > 0) {

		turnTimer_ -= 1.0f / 60.0f;
		float t = 1.0f - (turnTimer_ / kTimeTurn);

		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};

		// 状態に応じた角度を取得する
		float destinationRottaionY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		// 自キャラの角度を設定する
		worldTransformPlayer_.rotation_.y = math->EaseInOutSine(t, turnFirstRottationY_, destinationRottaionY);
	}


}

void Player::Update() {
	
	//移動処理
	Move();

	// 衝突判定情報構造体
	CollisionMapInfo collisionInfo;
	collisionInfo.isMovement = velosity_;

	// 各方向の当たり判定処理
	MapChipUp(collisionInfo);    // 上方向
	MapChipDown(collisionInfo);  // 下方向（地面）
	MapChipLeft(collisionInfo);  // 左方向
	MapChipRight(collisionInfo); // 右方向

	ResolveCollision();

	if (collisionInfo.isHitTop) {
		DebugText::GetInstance()->ConsolePrintf("hit celing\n");
		velosity_.y = 0;
	}

	// 行列更新
	math->worldTransFormUpdate(worldTransformPlayer_);
}

void Player::Draw() { model_->Draw(worldTransformPlayer_, *camera_); }

void Player::ResolveCollision() 
{
	worldTransformPlayer_.translation_ += velosity_;
	
}

void Player::MapChipUp(CollisionMapInfo& info) 
{
	std::array<Vector3, knumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CarnerPosition(worldTransformPlayer_.translation_ + info.isMovement, static_cast<Corner>(i));
	}
	//上昇あり?
	if (info.isMovement.y <= 0) {
		return;
	}
	MapChipType mapchipType;
	//真上の当たり判定
	bool hit = false;
	//左上の判定
	IndexSet indexSet;
	indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
	if (mapchipType == MapChipType::kBlock_) {
		hit = true;
	}
	if (hit) {
		indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);

		info.isMovement.y = std::max(0.0f, velosity_.y);
		// 天井に当たったことを記録する
		info.isHitTop = true;
	}
	

	//右上の判定

	indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
	if (mapchipType == MapChipType::kBlock_) {
		hit = true;
	}

		if (hit) {
		indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
		mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);

		info.isMovement.y = std::max(0.0f, velosity_.y);
		// 天井に当たったことを記録する
		info.isHitTop = true;
	}


}

void Player::MapChipDown(CollisionMapInfo& info) {
	std::array<Vector3, knumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CarnerPosition(worldTransformPlayer_.translation_ + info.isMovement, static_cast<Corner>(i));
	}

	// 下降あり?
	if (info.isMovement.y >= 0) {
		return;
	}

	MapChipType mapchipType;
	bool hit = false;
	IndexSet indexSet;

	// 左下
	indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
	if (mapchipType == MapChipType::kBlock_) {
		hit = true;
	}

	if (hit) {
		indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
		mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
		info.isMovement.y = std::min(0.0f, velosity_.y);
		info.isHitBottom = true;
	}
	

	// 右下
	indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
	if (mapchipType == MapChipType::kBlock_) {
		hit = true;
	}

	if (hit) {
		indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
		mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
		info.isMovement.y = std::min(0.0f, velosity_.y);
		info.isHitBottom = true;
	}
}


void Player::MapChipLeft(CollisionMapInfo& info) {
	std::array<Vector3, knumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CarnerPosition(worldTransformPlayer_.translation_ + info.isMovement, static_cast<Corner>(i));
	}

	// 左移動あり?
	if (info.isMovement.x >= 0) {
		return;
	}

	MapChipType mapchipType;
	bool hit = false;
	IndexSet indexSet;

	// 左上
	indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
	if (mapchipType == MapChipType::kBlock_) {
		hit = true;
	}

	// 左下
	indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
	if (mapchipType == MapChipType::kBlock_) {
		hit = true;
	}

	if (hit) {
		info.isMovement.x = std::min(0.0f, velosity_.x);
		info.isHitLeft = true;
	}
}

void Player::MapChipRight(CollisionMapInfo& info) {
	std::array<Vector3, knumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CarnerPosition(worldTransformPlayer_.translation_ + info.isMovement, static_cast<Corner>(i));
	}

	// 右移動あり?
	if (info.isMovement.x <= 0) {
		return;
	}

	MapChipType mapchipType;
	bool hit = false;
	IndexSet indexSet;

	// 右上
	indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
	if (mapchipType == MapChipType::kBlock_) {
		hit = true;
	}

	// 右下
	indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
	if (mapchipType == MapChipType::kBlock_) {
		hit = true;
	}

	if (hit) {
		info.isMovement.x = std::max(0.0f, velosity_.x);
		info.isHitRight = true;
	}
}




Vector3 Player::CarnerPosition(const Vector3& center, Corner cornter) {
	Vector3 offSetTable[knumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0},
        {-kWidth / 2.0f, -kHeight / 2.0f, 0},
        {+kWidth / 2.0f, +kHeight / 2.0f, 0},
        {-kWidth / 2.0f, +kHeight / 2.0f, 0}
	};

	return center + offSetTable[static_cast<uint32_t>(cornter)];

}

void Player::ground(const CollisionMapInfo& info) 
{
	if (info.isHitBottom) {
		//着地状態
		onGround_ = true;
		//着地時にX速度を減衰
		velosity_.x += (1.0f - kAtteunuation);
		//Yの速度をゼロにする
		velosity_.y = 0.0f;
		if (velosity_.y>0.0f) {
			onGround_ = false;
		}

	}
	else 
	{
		MapChipType mapChipType;
		//真下の当たり判定
		bool hit = false;

		//左上点の当たり判定
		kLeftBottom + Vector3(0, -smallnumber, 0);
		kRightBottom + Vector3(0, -smallnumber, 0);

		//落下
		if (!hit) {
			//空中状態に切り替える
			onGround_ = false;
		}

	}
}
