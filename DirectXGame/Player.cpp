#define NOMINMAX
#include "Player.h"
#include "CloudPlatform.h"
#include "GameScene1_2.h"
#include "MapChipField.h" // MapChipField クラスの定義が必要
#include "cassert"
#include <algorithm> // std::clamp, std::max, std::min のために必要
#include <numbers>   // std::numbers::pi_v のために必要

using namespace KamataEngine;

void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	camera_ = camera;
	model_ = model;

	worldTransformPlayer_.Initialize();
	worldTransformPlayer_.translation_ = position;
	worldTransformPlayer_.rotation_.y = std::numbers::pi_v<float> / 2.0f; // 初期向きを右に設定


	// 攻撃用モデルの設定
	worldTransformAttack_.Initialize();
	worldTransformAttack_.translation_ = worldTransformPlayer_.translation_;
	worldTransformAttack_.rotation_ = worldTransformPlayer_.rotation_;

	titleGroundY_ = worldTransformPlayer_.translation_.y;

	// Mathクラスの初期化（注意：シングルトンや依存性注入を推奨）
	if (!math) {
		math = new Math();
	}
	onGround_ = false; // 初期状態は空中または地面にいるか不明とする
	isDead_ = false;   // 初期状態は生存
	isOnIce_ = false;
	hp_ = 3; // 初期HP
	isInvincible_ = false;
	invincibleTimer_ = 0.0f;
}

void Player::SetStageNodes(const std::vector<Vector3>& nodes) {
	nodes_ = nodes;
	currentNode_ = 0;
	SetWorldPosition(nodes_[0]); // 初期位置をノードに合わせる
}

Vector3 Player::GetWorldPosition() const {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得
	worldPos.x = worldTransformPlayer_.matWorld_.m[3][0]; // 行列の4行1列目
	worldPos.y = worldTransformPlayer_.matWorld_.m[3][1]; // 行列の4行2列目
	worldPos.z = worldTransformPlayer_.matWorld_.m[3][2]; // 行列の4行3列目
	return worldPos;
}

void Player::BehaviorRootInitialize() {}

void Player::UpdateTitleAnimation() {
	//カメラジャンプ中は通らないように制限
	if (isCameraJumping_) {
		return; 
	}
	// --- 1. マスタータイマーを進める ---
	animationTimer_ += (1.0f / 60.0f); 

	// --- 2. 「見回す」動作をランダムに起動 ---
	if (fmod(animationTimer_, 4.0f) < (1.0f / 60.0f) && turnTimer_ <= 0.0f && !isTitleJumping_) {
		lrDirection_ = (lrDirection_ == LRDirection::kRight) ? LRDirection::kLeft : LRDirection::kRight;
		turnFirstRottationY_ = worldTransformPlayer_.rotation_.y;
		turnTimer_ = kTimeTurn;
	}

	// --- 3. 「ジャンプ」動作をランダムに起動 ---
	if (fmod(animationTimer_, 6.0f) < (1.0f / 60.0f) && turnTimer_ <= 0.0f && !isTitleJumping_) {
		isTitleJumping_ = true;
		titleJumpTimer_ = 0.0f;
		isSpinning_ = true; // ジャンプと同時に回転も開始
		spinTimer_ = 0.0f;
	}

	// --- 4. 実行中のアニメーションを更新 ---

	// 「旋回」の更新
	if (turnTimer_ > 0.0f) {
		turnTimer_ = std::max(turnTimer_ - (1.0f / 60.0f), 0.0f);
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		float t = 1.0f - (turnTimer_ / kTimeTurn);
		worldTransformPlayer_.rotation_.y = math->EaseInOutSine(t, turnFirstRottationY_, destinationRotationY);
	}

	// 「スピン（Z回転）」の更新
	if (isSpinning_) {
		spinTimer_ += 1.0f / 60.0f;
		float spinProgress = spinTimer_ / kSpinDuration;
		float spinAngle = spinProgress * 2.0f * std::numbers::pi_v<float>;
		if (lrDirection_ == LRDirection::kLeft) {
			spinAngle *= -1.0f;
		}
		worldTransformPlayer_.rotation_.z = spinAngle;

		if (spinTimer_ >= kSpinDuration) {
			isSpinning_ = false;
			worldTransformPlayer_.rotation_.z = 0.0f;
		}
	}

	// 「ジャンプ（Y座標）」の更新
	if (isTitleJumping_) {
		titleJumpTimer_ += (1.0f / 60.0f);
		float t = titleJumpTimer_ / kTitleJumpDuration;

		if (t >= 1.0f) {
			// ジャンプ終了
			isTitleJumping_ = false;
			worldTransformPlayer_.translation_.y = titleGroundY_; // 地面に戻す
			isSquashing_ = true;                                  // 着地したので「ぽよん」を開始
			squashTimer_ = 0.0f;
		} else {
			float yOffset = std::sin(t * std::numbers::pi_v<float>) * kTitleJumpHeight;
			worldTransformPlayer_.translation_.y = titleGroundY_ + yOffset;
		}
	} else {
		// ジャンプ中でも着地ぽよん中でもない時だけ、アイドリングの「ぽよん」
		if (!isSquashing_) {
			float idleBounce = std::sin(animationTimer_ * 5.0f); // 通常のぽよん
			worldTransformPlayer_.scale_.y = originalScaleY_ + (0.1f * idleBounce);
			worldTransformPlayer_.scale_.x = originalScaleY_ - (0.05f * idleBounce); // 縦と逆に
			worldTransformPlayer_.translation_.y = titleGroundY_;                    // 地面に固定
		}
	}


}


// 移動処理
void Player::Move() {

		// --- 現在の摩擦係数を決定 ---
	float currentAttenuation = kAtteunuation; // デフォルトは通常の摩擦
	// 接地していて、かつ氷の上に乗っている場合
	if (onGround_ &&  isOnIce_) {
		// 摩擦を氷用のもの（すごく小さい値）に変更
		currentAttenuation = kIceAttenuation;
	}

	// --- 左右移動（地上・空中で処理共通） ---
	if (Input::GetInstance()->PushKey(DIK_D) || Input::GetInstance()->PushKey(DIK_A)) {
		Vector3 acceleration = {};
		float airControl = onGround_ ? 1.0f : 0.4f;

		if (Input::GetInstance()->PushKey(DIK_D)) {
			if (velosity_.x < 0.0f) {
				// ★ここが書き換えた摩擦係数で処理される
				velosity_.x *= (1.0f - currentAttenuation);
			}
			acceleration.x += (kAcceleration / 60.0f) * airControl;
			if (lrDirection_ != LRDirection::kRight) {
				lrDirection_ = LRDirection::kRight;
				turnFirstRottationY_ = worldTransformPlayer_.rotation_.y;
				turnTimer_ = kTimeTurn;
			}
		} else if (Input::GetInstance()->PushKey(DIK_A)) {
			if (velosity_.x > 0.0f) {
				// ★ここも書き換えた摩擦係数で処理される
				velosity_.x *= (1.0f - currentAttenuation);
			}
			acceleration.x -= (kAcceleration / 60.0f) * airControl;
			if (lrDirection_ != LRDirection::kLeft) {
				lrDirection_ = LRDirection::kLeft;
				turnFirstRottationY_ = worldTransformPlayer_.rotation_.y;
				turnTimer_ = kTimeTurn;
			}
		}

		velosity_ += acceleration;
		velosity_.x = std::clamp(velosity_.x, -kLimitRunSpeed, kLimitRunSpeed);
	} else {
		// ★入力がない時も、書き換えた摩擦係数で減速する
		velosity_.x *= (1.0f - currentAttenuation);
	}

// ジャンプ入力のチェック
	// ※PushKeyだと押しっぱなしで連続ジャンプしてしまうため、
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) { 

		// ジャンプ回数が2回未満ならジャンプできる
		if (jumpCount_ < 2) {
			velosity_.y = kJumpAccleration / 60.0f; // ジャンプの初速を与える
			onGround_ = false;                      // ジャンプした瞬間に空中状態へ

			if (jumpCount_ == 1) {
				isSpinning_ = true;
				spinTimer_ = 0.0f; // タイマーをリセット
			}
			jumpCount_++;                           // ジャンプ回数を1増やす
		}
	}

	// 重力（接地していない場合に常に適用）
	if (!onGround_) {
		// 重力
		velosity_.y += -kGgravityAcceleration / 60.0f;
		velosity_.y = std::max(velosity_.y, -kLimitFallSpeed);
	}

	// --- 微小速度の丸め ---
	if (std::abs(velosity_.x) <= 0.0001f) {
		velosity_.x = 0.0f;
	}
}



void Player::Update() {
	// 1. フラグ初期化
	onCloud_ = nullptr;


		BehaviorRootUpdate();


	// 3. 衝突処理（地面＆雲）
	//    → GameScene 側で呼んでると思う
	//    player->CheckCloudPlatformCollision(cloudPlatforms);
	//    player->CheckMapChipCollision(...);

	// 4. 雲に乗っていたらdelta加算
	if (onCloud_) {
		worldTransformPlayer_.translation_ += onCloud_->GetDelta();
		onGround_ = true; // 雲に乗っている場合は接地状態とみなす
	}



	ImGui::Begin("player");
	ImGui::Text("%d", hp_);
}

void Player::Draw() {
	assert(model_);
	assert(camera_);
	//着地時の伸び縮みアニメーション
	UpdateSquashAnimation();

	//アニメーションの補正計算
	Vector3 finalTranslation = worldTransformPlayer_.translation_;
	if (onGround_ || isSquashing_) {
		float yOffset = (originalScaleY_ - worldTransformPlayer_.scale_.y) * (kHeight / 2.0f);
		finalTranslation.y -= yOffset;
	}

	// 5. 行列更新
	math->worldTransFormUpdate(worldTransformPlayer_);
	if (isInvincible_) {
		float blink = fmod(invincibleTimer_ * 20.0f, 2.0f);
		if (blink < 1.0f) {
			return;
		}
		
	}
	
	model_->Draw(worldTransformPlayer_, *camera_);

}

// 各方向のマップチップとの当たり判定
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
	indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
	if (mapchipType == MapChipType::kDirt_ || mapchipType == MapChipType::kGrass_ || mapchipType == MapChipType::kBreakable_ || mapchipType == MapChipType::kJumpPad_||mapchipType == MapChipType::kIceFloor_) {
		hit = true;
	}

	// 右上点の判定
	if (!hit) { // 左上がヒットしていない場合のみチェック
		indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
		mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
		if (mapchipType == MapChipType::kDirt_ || mapchipType == MapChipType::kGrass_ || mapchipType == MapChipType::kBreakable_ || mapchipType == MapChipType::kJumpPad_ ||
		    mapchipType == MapChipType::kIceFloor_) {

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
	MapChipField::IndexSet indexSet;

	// 左下点の判定
	indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
	if (mapchipType == MapChipType::kDirt_ || mapchipType == MapChipType::kGrass_ || mapchipType == MapChipType::kBreakable_ || mapchipType == MapChipType::kJumpPad_ ||
	    mapchipType == MapChipType::kIceFloor_) {
		hit = true;
	}

	// 右下点の判定
	if (!hit) {
		indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
		mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
		if (mapchipType == MapChipType::kDirt_ || mapchipType == MapChipType::kGrass_ || mapchipType == MapChipType::kBreakable_ || mapchipType == MapChipType::kJumpPad_ ||
		    mapchipType == MapChipType::kIceFloor_) {
			hit = true;
		}
	}

	if (hit) {

		// もし接触したブロックが氷なら、「報告書」にフラグを立てる
		if (mapchipType == MapChipType::kIceFloor_) {
			info.onIce = true;
		}

		// ---- この後の着地処理は、氷ブロックでも通常通り実行される ----
		indexSet = mapchipField_->GetMapChipIndexSetByPosition(worldTransformPlayer_.translation_ + info.isMovement + Vector3(0, -kHeight / 2.0f, 0));
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapchipField_->GetMapChipIndexSetByPosition(worldTransformPlayer_.translation_ + Vector3(0, -kHeight / 2.0f, 0));

		if (indexSetNow.yIndex != indexSet.yIndex) {
			MapChipField::Rect rect = mapchipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.isMovement.y = std::min(0.0f, rect.top - (worldTransformPlayer_.translation_.y - kHeight / 2.0f) + kBlank);
			info.isHitBottom = true;
		} else {
			info.isMovement.y = 0.0f;
			info.isHitBottom = true;
		}

		if (mapchipType == MapChipType::kBreakable_) {
			isbreak = true;
		}
		if (mapchipType == MapChipType::kJumpPad_) {
			velosity_.y += (kJumpAccleration / 60.0f) * 2.0f;
			onGround_ = false;
			info.isMovement.y = 0.0f;
			info.isHitBottom = false;
			return; // ジャンプパッドだけは特別にここで処理を終える
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
	if (mapchipType == MapChipType::kDirt_ || mapchipType == MapChipType::kGrass_ || mapchipType == MapChipType::kBreakable_ || mapchipType == MapChipType::kJumpPad_ ||
	    mapchipType == MapChipType::kIceFloor_) {
		hit = true;
	}

	// 左下点の判定
	if (!hit) { // 左上がヒットしていない場合のみチェック
		indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
		mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
		if (mapchipType == MapChipType::kDirt_ || mapchipType == MapChipType::kGrass_ || mapchipType == MapChipType::kBreakable_ || mapchipType == MapChipType::kJumpPad_ ||
		    mapchipType == MapChipType::kIceFloor_) {
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
	if (mapchipType == MapChipType::kDirt_ || mapchipType == MapChipType::kGrass_ || mapchipType == MapChipType::kBreakable_ || mapchipType == MapChipType::kJumpPad_ ||
	    mapchipType == MapChipType::kIceFloor_) {
		hit = true;
	}

	// 右下点の判定
	if (!hit) { // 右上がヒットしていない場合のみチェック
		indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
		mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
		if (mapchipType == MapChipType::kDirt_ || mapchipType == MapChipType::kGrass_ || mapchipType == MapChipType::kBreakable_ || mapchipType == MapChipType::kJumpPad_ ||
		    mapchipType == MapChipType::kIceFloor_) {
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

// 接地状態の切り替え処理 


void Player::UpdateOnGround(const CollisionMapInfo& info) {
	if (onGround_) {
		// --- 空中に移行する瞬間の処理 ---
		if (velosity_.y > 0.0f) { // ジャンプした
			onGround_ = false;
			isOnIce_ = false;
			return; // 空中に行ったので、以降の地面の処理は不要
		}

		// --- 地面にいる間の継続的な処理 ---

		// 1. 毎フレーム、足元の床の種類をチェックする
		MapChipType currentFloor = GetFloorChipType();

		// 2. 床の種類に応じて isOnIce_ フラグを更新する
		if (currentFloor == MapChipType::kIceFloor_) {
			isOnIce_ = true;
		} else {
			isOnIce_ = false;
		}

		// 3. 落下判定（足元にブロックが無くなったら）
		if (currentFloor == MapChipType::kBlank_ || currentFloor == MapChipType::kSpike_) {
			// もし足元が空白かトゲなら、落下する
			onGround_ = false;
			isOnIce_ = false;
		}

	

	} else {
		// --- 着地した瞬間の処理 ---
		if (info.isHitBottom) {
			onGround_ = true;
			isOnIce_ = info.onIce; // 着地した場所が氷だったかを受け取る
			isSquashing_ = true;   // ぽよんアニメ開始
			squashTimer_ = 0.0f;   // タイマーリセット
			jumpCount_ = 0;
			velosity_.x *= (1.0f - kAttenuationLanding);
			velosity_.y = 0.0f;
		}
	}
}
// 壁接触時の処理
void Player::UpdateOnWall(const CollisionMapInfo& info) {
	if (info.hitWall) {
		// 変更後：速度を反転させて、少しだけ跳ね返らせる
		velosity_.x *= -0.3f; 
	}
}



void Player::BehaviorRootUpdate() {
	// 1. 入力や重力で、このフレームでどれだけ動きたいか（速度）を計算
	if (isMove_) {
		Move();
	}

	// 衝突情報を初期化
	CollisionMapInfo collisionInfo = {};

	// 2. X軸（横）の移動と衝突判定を「個別」に行う
	collisionInfo.isMovement.x = velosity_.x;
	collisionInfo.isMovement.y = 0.0f;
	MapChipLeft(collisionInfo);
	MapChipRight(collisionInfo);
	worldTransformPlayer_.translation_.x += collisionInfo.isMovement.x;

	// 3. Y軸（縦）の移動と衝突判定を「個別」に行う
	collisionInfo.isMovement.x = 0.0f;
	collisionInfo.isMovement.y = velosity_.y;
	MapChipUp(collisionInfo);
	MapChipDown(collisionInfo);
	worldTransformPlayer_.translation_.y += collisionInfo.isMovement.y;

	// 4. 状態と速度の更新
	if (collisionInfo.isHitTop) {
		velosity_.y = 0;
	}
	UpdateOnGround(collisionInfo);
	UpdateOnWall(collisionInfo);

	// 5. アニメーション関連の更新
	// 2段ジャンプ中の回転処理
	if (isSpinning_) {
		spinTimer_ += 1.0f / 60.0f;
		float spinProgress = spinTimer_ / kSpinDuration;
		float spinAngle = spinProgress * 2.0f * std::numbers::pi_v<float>;

		if (lrDirection_ == LRDirection::kLeft) {
			spinAngle *= -1.0f;
		}
		worldTransformPlayer_.rotation_.z = spinAngle;

		if (spinTimer_ >= kSpinDuration) {
			isSpinning_ = false;
			worldTransformPlayer_.rotation_.z = 0.0f;
		}
	} 

	// 旋回制御
	if (turnTimer_ > 0.0f) {
		turnTimer_ = std::max(turnTimer_ - (1.0f / 60.0f), 0.0f);

		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		float t = 1.0f - (turnTimer_ / kTimeTurn);
		worldTransformPlayer_.rotation_.y = math->EaseInOutSine(t, turnFirstRottationY_, destinationRotationY);
	}
	// 無敵時間の更新
	if (isInvincible_) {
		invincibleTimer_ -= (1.0f / 60.0f); // 60FPS想定
		if (invincibleTimer_ <= 0.0f) {
			isInvincible_ = false;
		}
	}
}

void Player::StartMoveToNode(int index) {
	startPos_ = GetWorldPosition();
	targetPos_ = nodes_[index];
	targetNode_ = index;
	t_ = 0.0f;
	isMoving_ = true;
}
void Player::MoveToTarget() {
	t_ += 0.05f; // 移動スピード
	Vector3 newPos;
	newPos.x = startPos_.x + (targetPos_.x - startPos_.x) * t_;
	newPos.y = startPos_.y + (targetPos_.y - startPos_.y) * t_;
	newPos.z = startPos_.z + (targetPos_.z - startPos_.z) * t_;
	SetWorldPosition(newPos);

	if (t_ >= 1.0f) {
		isMoving_ = false;
		currentNode_ = targetNode_;
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

AABB Player::GetAABB() {
	Vector3 worldPos = GetWorldPosition();
	AABB aabb;
	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};
	return aabb;
}

void Player::OnCollision(const Enemy* enemy) {
	(void)enemy;
	TakeDamage(1);
}

void Player::OnCollision(const KabeToge* togeKabe_) {
	(void)togeKabe_;
	TakeDamage(1);
}

// Player.cpp
std::optional<MapChipField::IndexSet> Player::CheckSteppedBreakable(MapChipField* mapChipField) {
	// 足元のちょい下（半分＋ε）で見ると安定
	const float eps = 0.5f;
	Vector3 foot = worldTransformPlayer_.translation_ + Vector3(0, -kHeight / 2.0f - eps, 0);

	auto index = mapChipField->GetMapChipIndexSetByPosition(foot);
	if (mapChipField->GetMapChipTypeByindex(index.xIndex, index.yIndex) == MapChipType::kBreakable_) {
		return index;
	}
	return std::nullopt;
}

void Player::CheckCloudPlatformCollision(const std::vector<CloudPlatform*>& cloudPlatforms) {
	AABB playerAABB = GetAABB();
	onCloud_ = nullptr; // ← 毎フレームリセット

	for (auto* cloud : cloudPlatforms) {
		if (!cloud)
			continue;

		AABB cloudAABB = cloud->GetAABB();
		if (!math->IsCollision(playerAABB, cloudAABB))
			continue;

		// 衝突量を計算
		float dx1 = cloudAABB.max.x - playerAABB.min.x;
		float dx2 = playerAABB.max.x - cloudAABB.min.x;
		float dy1 = cloudAABB.max.y - playerAABB.min.y;
		float dy2 = playerAABB.max.y - cloudAABB.min.y;

		float overlapX = std::min(dx1, dx2);
		float overlapY = std::min(dy1, dy2);

		if (overlapX < overlapY) {
			// --- 横衝突 ---
			// 雲は横からぶつかっても「壁扱い」しない
			// → ここでは何もしない or スルーする
		} else {
			// --- 縦方向の衝突 ---
			if (dy1 < dy2) {
				// 下からぶつかった → 跳ね返すだけ
				worldTransformPlayer_.translation_.y = cloudAABB.max.y + kHeight / 2.0f;
				velosity_.y = 0;
			} else {
				// 上から乗った
				worldTransformPlayer_.translation_.y = cloudAABB.min.y - kHeight / 2.0f;
				velosity_.y = 0;

				onGround_ = true;
				onCloud_ = cloud;
			}
		}
	}

	playerAABB = GetAABB();
}

void Player::ApplyCloudDelta() {
	if (onCloud_) {
		worldTransformPlayer_.translation_ += onCloud_->GetDelta();
	}
}


// プレイヤーの足元にあるマップチップの種類を返す
MapChipType Player::GetFloorChipType() {
	// プレイヤーの足元（中心から少し下）の座標
	Vector3 footPosition = worldTransformPlayer_.translation_;
	footPosition.y -= (kHeight / 2.0f) + 0.1f; // 足の裏あたりの座標

	// 座標からマップチップのインデックスを取得
	MapChipField::IndexSet index = mapchipField_->GetMapChipIndexSetByPosition(footPosition);
	// インデックスからマップチップの種類を返す
	return mapchipField_->GetMapChipTypeByindex(index.xIndex, index.yIndex);
}


void Player::TakeDamage(int damage) {
	// 無敵時間中はダメージを受けない
	if (isInvincible_) {
		return;
	}

	hp_ -= damage;
	if (hp_ <= 0) {
		hp_ = 0;
		SetIsDead(true); // HPが0になったら死亡
	}

	// ダメージを受けたら無敵時間開始
	isInvincible_ = true;
	invincibleTimer_ = kInvincibleDuration;


	velosity_.y = kJumpAccleration / 120.0f;
	onGround_ = false; // 空中状態にする

	// 2. もし2段ジャンプスピン中なら、それを解除する
	if (isSpinning_) {
		isSpinning_ = false;
		worldTransformPlayer_.rotation_.z = 0.0f;
	}


}

void Player::CheckAndResolveTogeKabeCollision(const KabeToge* togeKabe) {
	if (!togeKabe || isDead_) {
		return;
	}

	AABB playerAABB = GetAABB();
	AABB kabeAABB = togeKabe->GetAABB();

	if (!math->IsCollision(playerAABB, kabeAABB)) {
		return; // 衝突していないなら終了
	}


	// 1. ダメージ処理
	TakeDamage(1);

	// 2. 押し戻し処理
	// プレイヤーの左側が、壁の右側にどれだけめり込んでいるか
	float penetration = kabeAABB.max.x - playerAABB.min.x;

	if (penetration > 0) {

		// 2a. 押し出された「後」のプレイヤーの予測座標を計算
		Vector3 newPos = worldTransformPlayer_.translation_;
		newPos.x += penetration;

		// 2b. その予測座標で、プレイヤーの「右上の角」と「右下の角」が安全かチェック
		Vector3 checkPosTopRight = CarnerPosition(newPos, kRightTop);
		Vector3 checkPosBottomRight = CarnerPosition(newPos, kRightBottom);

		MapChipField::IndexSet indexTop = mapchipField_->GetMapChipIndexSetByPosition(checkPosTopRight);
		MapChipField::IndexSet indexBottom = mapchipField_->GetMapChipIndexSetByPosition(checkPosBottomRight);

		MapChipType typeTop = mapchipField_->GetMapChipTypeByindex(indexTop.xIndex, indexTop.yIndex);
		MapChipType typeBottom = mapchipField_->GetMapChipTypeByindex(indexBottom.xIndex, indexBottom.yIndex);

		// 2c. 押し出し先が「固いブロック」かどうかを判定
		bool isTopSolid =
		    (typeTop == MapChipType::kDirt_ || typeTop == MapChipType::kGrass_ || typeTop == MapChipType::kBreakable_ || typeTop == MapChipType::kJumpPad_ || typeTop == MapChipType::kIceFloor_);
		bool isBottomSolid =
		    (typeBottom == MapChipType::kDirt_ || typeBottom == MapChipType::kGrass_ || typeBottom == MapChipType::kBreakable_ || typeBottom == MapChipType::kJumpPad_ ||
		     typeBottom == MapChipType::kIceFloor_);

		if (isTopSolid || isBottomSolid) {
			// 押し出し先が壁！＝「圧死」
			SetIsDead(true);
		} else {
			// 押し出し先は安全（空白）なので、プレイヤーを移動させる
			worldTransformPlayer_.translation_.x += penetration;

			// 押し戻されたので、左向きの速度はゼロにする
			if (velosity_.x < 0) {
				velosity_.x = 0;
			}
		}
	
	}
}

void Player::UpdateSquashAnimation() {
	if (!isSquashing_) {
		// ぽよん中でないなら、徐々に元のスケール(1.0)に戻す
		worldTransformPlayer_.scale_.x += (1.0f - worldTransformPlayer_.scale_.x) * 0.2f;
		worldTransformPlayer_.scale_.y += (1.0f - worldTransformPlayer_.scale_.y) * 0.2f;
		return;
	}

	// タイマーを進める (60FPS想定)
	squashTimer_ += 1.0f / 60.0f;

	if (squashTimer_ >= kSquashDuration) {
		// アニメーション終了
		isSquashing_ = false;
		worldTransformPlayer_.scale_.x = 1.0f;
		worldTransformPlayer_.scale_.y = 1.0f;
	} else {
		// アニメーション中
		// タイマーの進捗 (0.0 -> 1.0)
		float t = squashTimer_ / kSquashDuration;

		// sinの山（0.0 -> 1.0 -> 0.0）の形を使って「ぽよん」を表現
		float bounce = std::sin(t * std::numbers::pi_v<float>);

		// 縦に縮む (1.0 -> 0.7 -> 1.0 のように変化)
		float squashAmount = 0.3f; // 30%縮む
		worldTransformPlayer_.scale_.y = 1.0f - (bounce * squashAmount);
		// 横に伸びる (1.0 -> 1.3 -> 1.0 のように変化)
		worldTransformPlayer_.scale_.x = 1.0f + (bounce * squashAmount);
	}
}

// カメラジャンプの開始命令
void Player::StartCameraJump() {
	isCameraJumping_ = true;
	cameraJumpTimer_ = 0.0f;
	isSpinning_ = true; // 回転も同時に行う
	spinTimer_ = 0.0f;
	isSquashing_ = false; // ぽよんを止める
	onGround_ = false;    // 地面から離れる
}

// カメラジャンプ中のアニメーション更新
void Player::UpdateCameraJump() {
	if (!isCameraJumping_) {
		return;
	}

	// 1. タイマーを進める
	cameraJumpTimer_ += (1.0f / 60.0f);
	float t = cameraJumpTimer_ / kCameraJumpDuration;
	t = std::min(t, 1.0f); // 1.0f で止める

	// 2. イージングを使って滑らかに変化させる（EaseOutQuad: だんだんゆっくりになる）
	float easeT = 1.0f - (1.0f - t) * (1.0f - t); // EaseOutQuad

	// 3. スケールを大きくする (1.0 -> 10.0)
	float scale = originalScaleY_ + (kCameraJumpScale - originalScaleY_) * easeT;
	worldTransformPlayer_.scale_ = {scale, scale, scale};

	// 4. Z座標を手前に動かす (0.0 -> 15.0)
	worldTransformPlayer_.translation_.z = kCameraJumpZoom * easeT;

	// 5. Y座標を放物線で動かす (sin(0) -> sin(π))
	float yOffset = std::sin(t * std::numbers::pi_v<float>) * kTitleJumpHeight;
	worldTransformPlayer_.translation_.y = titleGroundY_ + yOffset;

	// 6. スピン回転の処理 
	if (isSpinning_) {
		spinTimer_ += 1.0f / 60.0f;
		float spinProgress = spinTimer_ / kSpinDuration;
		float spinAngle = spinProgress * 2.0f * std::numbers::pi_v<float>;
		if (lrDirection_ == LRDirection::kLeft) {
			spinAngle *= -1.0f;
		}
		worldTransformPlayer_.rotation_.z = spinAngle;

		if (spinTimer_ >= kSpinDuration) {
			isSpinning_ = false; // 1回転したら終わり
		}
	}
}