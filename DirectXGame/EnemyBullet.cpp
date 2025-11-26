#include "EnemyBullet.h"

using namespace KamataEngine;

void EnemyBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity, MapChipField* mapChipField) {
	model_ = model;
	mapChipField_ = mapChipField;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {0.5f, 0.5f, 0.5f}; // 少し小さくする
	velocity_ = velocity;
	isDead_ = false;
	lifeTimer_ = 0.0f;
}

void EnemyBullet::Update() {
	// 移動
	worldTransform_.translation_ += velocity_;

	// 行列更新
	math_->worldTransFormUpdate(worldTransform_);

	// 寿命チェック
	lifeTimer_ += 1.0f / 60.0f;
	if (lifeTimer_ >= kLifeTime) {
		isDead_ = true;
	}

	if (mapChipField_) {
		// 弾の現在位置のマップチップを取得
		MapChipField::IndexSet index = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_);
		MapChipType type = mapChipField_->GetMapChipTypeByindex(index.xIndex, index.yIndex);

		// 空白以外なら「壁」とみなして消滅
		if (type != MapChipType::kBlank_) {
			OnCollision(); 
		}
	}


}

void EnemyBullet::Draw(Camera& camera) {
	if (!isDead_) {
		model_->Draw(worldTransform_, camera);
	}
}

void EnemyBullet::OnCollision() { isDead_ = true; }

Vector3 EnemyBullet::GetWorldPosition() const { return worldTransform_.translation_; }

AABB EnemyBullet::GetAABB() {
	Vector3 pos = worldTransform_.translation_;
	AABB aabb;
	aabb.min = {pos.x - kRadius, pos.y - kRadius, pos.z - kRadius};
	aabb.max = {pos.x + kRadius, pos.y + kRadius, pos.z + kRadius};
	return aabb;
}