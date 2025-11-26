#include "PlayerBullet.h"

using namespace KamataEngine;

void PlayerBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity, MapChipField* mapChipField) {
	model_ = model;
	mapChipField_ = mapChipField;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {0.4f, 0.4f, 0.4f}; // 本体より小さく
	velocity_ = velocity;
	isDead_ = false;
	lifeTimer_ = 0.0f;
}

void PlayerBullet::Update() {
	// 移動
	worldTransform_.translation_ += velocity_;


	// 寿命チェック
	lifeTimer_ += 1.0f / 60.0f;
	if (lifeTimer_ >= kLifeTime) {
		isDead_ = true;
	}

	// 壁判定（ブロック貫通防止）
	if (mapChipField_) {
		MapChipField::IndexSet index = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_);
		MapChipType type = mapChipField_->GetMapChipTypeByindex(index.xIndex, index.yIndex);
		if (type != MapChipType::kBlank_ && type != MapChipType::kGoal_ && type != MapChipType::kSpike_) {
			isDead_ = true; // 壁に当たったら消える
		}
	}

	math_->worldTransFormUpdate(worldTransform_);
}

void PlayerBullet::Draw(Camera& camera) {
	if (!isDead_) {
		model_->Draw(worldTransform_, camera);
	}
}

void PlayerBullet::OnCollision() { isDead_ = true; }

Vector3 PlayerBullet::GetWorldPosition() const { return worldTransform_.translation_; }

AABB PlayerBullet::GetAABB() {
	Vector3 pos = worldTransform_.translation_;
	AABB aabb;
	aabb.min = {pos.x - kRadius, pos.y - kRadius, pos.z - kRadius};
	aabb.max = {pos.x + kRadius, pos.y + kRadius, pos.z + kRadius};
	return aabb;
}