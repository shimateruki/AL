#define NOMINMAX
#include "CameraController.h"
#include "Player.h"

using namespace KamataEngine;

void CameraController::Initialize(KamataEngine::Camera* camera) { camera_ = camera; }

void CameraController::Update() {
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransformPlayer();

	// 02_06 スライド29枚目で追加
	const Vector3& targetVelocity = target_->GetVelosit();

	// 追従対象とオフセットからカメラの座標を計算
	destination_.x = targetWorldTransform.translation_.x + targettooffset.x + targetVelocity.x * kVelocityBias;
	destination_.y = targetWorldTransform.translation_.y + targettooffset.y + targetVelocity.y * kVelocityBias;
	destination_.z = targetWorldTransform.translation_.z + targettooffset.z + targetVelocity.z * kVelocityBias;
	// 座標補間によりゆったり追従(数学関数追加)
	camera_->translation_ = math_->Lerp(camera_->translation_, destination_, kInterpolationRate);

	// 追従対象が画面外に出ないように補正
	camera_->translation_.x = std::max(camera_->translation_.x, destination_.x + targetMargin.left);
	camera_->translation_.x = std::min(camera_->translation_.x, destination_.x + targetMargin.right);
	camera_->translation_.y = std::max(camera_->translation_.y, destination_.y + targetMargin.bottom);
	camera_->translation_.y = std::min(camera_->translation_.y, destination_.y + targetMargin.top);

	// 移動範囲制限
	camera_->translation_.x = std::max(camera_->translation_.x, movebleArea_.left);
	camera_->translation_.x = std::min(camera_->translation_.x, movebleArea_.right);
	camera_->translation_.y = std::min(camera_->translation_.y, movebleArea_.bottom);
	camera_->translation_.y = std::max(camera_->translation_.y, movebleArea_.top);

	camera_->UpdateMatrix();
}

void CameraController::Reset() {

	// 追従対象のワールドトランスフォーム
	const WorldTransform& targetWorldTransform = target_->GetWorldTransformPlayer();

	destination_ = targetWorldTransform.translation_ + targettooffset;
}
