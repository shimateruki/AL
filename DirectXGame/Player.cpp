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

	// Mathクラスの初期化（注意：シングルトンや依存性注入を推奨）
	if (!math) {
		math = new Math();
	}
	onGround_ = false; // 初期状態は空中または地面にいるか不明とする
	isDead_ = false;   // 初期状態は生存
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


// 移動処理
void Player::Move() {
	// --- 左右移動（地上・空中で処理共通） ---
	if (Input::GetInstance()->PushKey(DIK_D) || Input::GetInstance()->PushKey(DIK_A)) {
		Vector3 acceleration = {};
		float airControl = onGround_ ? 1.0f : 0.4f; // 空中は操作が40%だけ効く（調整OK）

		if (Input::GetInstance()->PushKey(DIK_D)) {
			if (velosity_.x < 0.0f) {
				velosity_.x *= (1.0f - kAtteunuation); // 逆方向なら減衰
			}
			acceleration.x += (kAcceleration / 60.0f) * airControl;
			if (lrDirection_ != LRDirection::kRight) {
				lrDirection_ = LRDirection::kRight;
				turnFirstRottationY_ = worldTransformPlayer_.rotation_.y;
				turnTimer_ = kTimeTurn;
			}
		} else if (Input::GetInstance()->PushKey(DIK_A)) {
			if (velosity_.x > 0.0f) {
				velosity_.x *= (1.0f - kAtteunuation);
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
		velosity_.x *= (1.0f - kAtteunuation); // 入力なし → 減速
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

	// 5. 行列更新
	math->worldTransFormUpdate(worldTransformPlayer_);
}

void Player::Draw() {
	assert(model_);
	assert(camera_);

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
	if (mapchipType == MapChipType::kDirt_ || mapchipType == MapChipType::kGrass_ || mapchipType == MapChipType::kBreakable_ || mapchipType == MapChipType::kJumpPad_) {
		hit = true;
	}

	// 右上点の判定
	if (!hit) { // 左上がヒットしていない場合のみチェック
		indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
		mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
		if (mapchipType == MapChipType::kDirt_ || mapchipType == MapChipType::kGrass_ || mapchipType == MapChipType::kBreakable_ || mapchipType == MapChipType::kJumpPad_) {

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
	if (mapchipType == MapChipType::kDirt_ || mapchipType == MapChipType::kGrass_ || mapchipType == MapChipType::kBreakable_ || mapchipType == MapChipType::kJumpPad_) {
		hit = true;
	}

	// 右下点の判定
	if (!hit) { // 左下がヒットしていない場合のみチェック
		indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
		mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
		if (mapchipType == MapChipType::kDirt_ || mapchipType == MapChipType::kGrass_ || mapchipType == MapChipType::kBreakable_ || mapchipType == MapChipType::kJumpPad_) {
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
		if (mapchipType == MapChipType::kBreakable_) {
			isbreak = true;
		}
		if (mapchipType == MapChipType::kJumpPad_) {
			// キノコジャンプ発動！
			velosity_.y += (kJumpAccleration / 60.0f) * 2.0f;
			onGround_ = false;
			info.isMovement.y = 0.0f; // 地面に吸着しない
			info.isHitBottom = false; // 接地フラグもオフ
			return;                   // ← 地面処理スキップ
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
	if (mapchipType == MapChipType::kDirt_ || mapchipType == MapChipType::kGrass_ || mapchipType == MapChipType::kBreakable_ || mapchipType == MapChipType::kJumpPad_) {
		hit = true;
	}

	// 左下点の判定
	if (!hit) { // 左上がヒットしていない場合のみチェック
		indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
		mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
		if (mapchipType == MapChipType::kDirt_ || mapchipType == MapChipType::kGrass_ || mapchipType == MapChipType::kBreakable_ || mapchipType == MapChipType::kJumpPad_) {
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
	if (mapchipType == MapChipType::kDirt_ || mapchipType == MapChipType::kGrass_ || mapchipType == MapChipType::kBreakable_ || mapchipType == MapChipType::kJumpPad_) {
		hit = true;
	}

	// 右下点の判定
	if (!hit) { // 右上がヒットしていない場合のみチェック
		indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
		mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
		if (mapchipType == MapChipType::kDirt_ || mapchipType == MapChipType::kGrass_ || mapchipType == MapChipType::kBreakable_ || mapchipType == MapChipType::kJumpPad_) {
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
			if (mapchipType == MapChipType::kDirt_ || mapchipType == MapChipType::kGrass_ || mapchipType == MapChipType::kBreakable_ || mapchipType == MapChipType::kJumpPad_) {
				hitSearch = true;
			}

			if (!hitSearch) { // 左下がヒットしていない場合のみチェック
				indexSet = mapchipField_->GetMapChipIndexSetByPosition(positionsCheck[kRightBottom]);
				mapchipType = mapchipField_->GetMapChipTypeByindex(indexSet.xIndex, indexSet.yIndex);
				if (mapchipType == MapChipType::kDirt_ || mapchipType == MapChipType::kGrass_ || mapchipType == MapChipType::kBreakable_ || mapchipType == MapChipType::kJumpPad_) {
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
			jumpCount_ = 0;                              // 接地したのでジャンプ回数をリセット
			velosity_.x *= (1.0f - kAttenuationLanding); // 着地時の横方向減衰
			velosity_.y = 0.0f;                          // Y速度をゼロに
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
	} // ★★★ isSpinning_ の if文はここで閉じる ★★★

	// 旋回制御 (isSpinning_ の外にあるのが正しい状態)
	if (turnTimer_ > 0.0f) {
		turnTimer_ = std::max(turnTimer_ - (1.0f / 60.0f), 0.0f);

		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		float t = 1.0f - (turnTimer_ / kTimeTurn);
		worldTransformPlayer_.rotation_.y = math->EaseInOutSine(t, turnFirstRottationY_, destinationRotationY);
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
	isDead_ = true;
}

void Player::OnCollision(const KabeToge* togeKabe_) {
	(void)togeKabe_;
	isDead_ = true; // 壁に当たったら死亡
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