#define NOMINMAX
#include "CameraController.h"
#include "Player.h"



using namespace KamataEngine;

void CameraController::Initialize(KamataEngine::Camera* camera) 
{ camera_ = camera; }

void CameraController::Update() 
{
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	// 追従対象の速度を取得
	const Vector3& targetVelocity = target_->GetVelocity();

	// 追従対象とオフセットからカメラの理想的な目標地点を計算
	Vector3 idealDestination = targetWorldTransform.translation_ + targettooffset + targetVelocity;

	// 目的地の移動範囲を制限（補間を行う**前**にクランプする）
	idealDestination.x = std::max(idealDestination.x, movebleArea_.left);
	idealDestination.x = std::min(idealDestination.x, movebleArea_.right);
	idealDestination.y = std::min(idealDestination.y, movebleArea_.bottom);
	idealDestination.y = std::max(idealDestination.y, movebleArea_.top);

	// 補間により、カメラの座標を目標地点へゆっくり移動させる
	camera_->translation_ = math_->Lerp(camera_->translation_, idealDestination, kInterpolationRate);

	// カメラの行列を更新
	camera_->UpdateMatrix();
}

void CameraController::Reset() 
{

		const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	Vector3 idealDestination = targetWorldTransform.translation_ + targettooffset;

	// その地点をクランプしてからカメラに直接代入する
	idealDestination.x = std::max(idealDestination.x, movebleArea_.left);
	idealDestination.x = std::min(idealDestination.x, movebleArea_.right);
	idealDestination.y = std::min(idealDestination.y, movebleArea_.bottom);
	idealDestination.y = std::max(idealDestination.y, movebleArea_.top);

	camera_->translation_ = idealDestination;
}
