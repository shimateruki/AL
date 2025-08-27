#include "CloudPlatform.h"

void CloudPlatform::Initialize(Model* model, Camera* camera, const Vector3& position) {
	model_ = model;
	camera_ = camera;
	worldTransform_.translation_ = position;
	worldTransform_.translation_.z = 0.5f;
	worldTransform_.Initialize();
	previousPosition_ = worldTransform_.translation_; // 初期位置を保存
	color_.Initialize();
	color_.SetColor({1.0f, 1.0f, 1.0f, 1.0f}); // 白色で初期化
}

void CloudPlatform::SetMovement(const Vector3& start, const Vector3& end, float speed) {
	startPosition_ = start;
	endPosition_ = end;
	speed_ = speed;
	t_ = 0.0f;          // ← 補間開始
	directionSign_ = 1; // ← +1でend方向へ
	worldTransform_.translation_ = startPosition_;
	worldTransform_.Initialize();
}

// CloudPlatform.cpp
void CloudPlatform::Update() {
	previousPosition_ = worldTransform_.translation_;

	// t を進める
	t_ += directionSign_ * speed_;
	if (t_ > 1.0f) {
		t_ = 1.0f;
		directionSign_ = -1;
	}
	if (t_ < 0.0f) {
		t_ = 0.0f;
		directionSign_ = 1;
	}

	// 新しい位置を計算
	Vector3 newPos = math->Lerp(startPosition_, endPosition_, t_);

	// 移動量を保存
	delta_ = newPos - previousPosition_;

	worldTransform_.translation_ = newPos;
	math->worldTransFormUpdate(worldTransform_);
}
void CloudPlatform::Draw() { model_->Draw(worldTransform_, *camera_,&color_); }

Vector3 CloudPlatform::GetWorldPosition() const { return worldTransform_.translation_; }

AABB CloudPlatform::GetAABB() const {
	Vector3 worldPos = worldTransform_.translation_;
	AABB aabb;
	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};
	return aabb;
}

	Vector3 CloudPlatform::GetDelta() const { return delta_; }