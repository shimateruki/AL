#include "BreakableFloor.h"

using namespace KamataEngine;

void BreakableFloor::Initialize(const Vector3& pos, uint32_t x, uint32_t y, MapChipField* field) {
	worldTransform_.translation_ = pos;
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.Initialize();

	color_.Initialize();
	color_.SetColor({0.0f, 0.0f, 1.0f, 1.0f}); // 青色で初期化

	xIndex_ = x;
	yIndex_ = y;
	mapChipField_ = field;
}

void BreakableFloor::OnStepped() {
	if (!isBreaking_ && !isBroken_) {
		isBreaking_ = true;
		timer_ = 0;
	}
}

void BreakableFloor::Update() {
	// 壊れる処理中ならタイマー進める
	if (isBreaking_ && !isBroken_) {
		timer_++;
		if (timer_==30) {
			color_.SetColor({1.0f, 1.0f, 0.0f, 1.0f}); // 黄色で初期化
		}
		if (timer_==60) {
			color_.SetColor({1.0f, 0.0f, 0.0f, 1.0f}); // 赤色で初期化
		}
		if (timer_ >= kBreakTime) {
			// 完全に壊れた
			isBroken_ = true;
			if (mapChipField_) {
				// マップの当該チップを空白に差し替え
				// （ResetMapChipDataだと全部消えちゃうので）
				mapChipField_->SetMapChipType(xIndex_, yIndex_, MapChipType::kBlank_);
			}
		}
	}

	if (!isBroken_) {
		math->worldTransFormUpdate(worldTransform_);
	}
}

void BreakableFloor::Draw(Model* model, Camera* camera) {
	if (!isBroken_ && model && camera) {
		model->Draw(worldTransform_, *camera,&color_);
	}
}

AABB BreakableFloor::GetAABB() const {
	Vector3 worldPos = {worldTransform_.matWorld_.m[3][0], worldTransform_.matWorld_.m[3][1], worldTransform_.matWorld_.m[3][2]};
	AABB aabb;
	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};
	return aabb;
}