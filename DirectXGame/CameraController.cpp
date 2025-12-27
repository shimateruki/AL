#define NOMINMAX
#include "CameraController.h"
#include "Player.h"

using namespace KamataEngine;

void CameraController::Initialize(KamataEngine::Camera* camera) {
	camera_ = camera;
	mode_ = Mode::kFollowPlayer; // モードを初期化


	if (!math_) {
		math_ = new Math();
	}
}

void CameraController::Update() {

	// モードに応じて処理を切り替える
	switch (mode_) {

	case Mode::kFollowPlayer: { // --- 通常の追従処理 ---
		// 追従対象のワールドトランスフォームを参照
		const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
		// 追従対象の速度を取得
		const Vector3& targetVelocity = target_->GetVelocity();

		// 追従対象とオフセットからカメラの理想的な目標地点を計算
		Vector3 idealDestination = targetWorldTransform.translation_ + targettooffset + targetVelocity;

		// ---------------------------------------------------------
		// 移動範囲の制限 
		// ---------------------------------------------------------

		// X軸の制限 (左端 ～ 右端)
		idealDestination.x = std::clamp(idealDestination.x, movebleArea_.left, movebleArea_.right);

		// Y軸の制限 (下限 ～ 上限)
		// movebleArea_.bottom = 地面の高さ（これ以上下がらない）
		// movebleArea_.top    = 空の高さ（これ以上上がらない）
		idealDestination.y = std::clamp(idealDestination.y, movebleArea_.bottom, movebleArea_.top);

		// ---------------------------------------------------------

		// 補間により、カメラの座標を目標地点へゆっくり移動させる

		camera_->translation_ = math_->Lerp(camera_->translation_, idealDestination, kInterpolationRate);

	} break;

	case Mode::kVictoryZoom: { // --- ズーム演出の処理 ---
		UpdateVictoryZoom();
	} break;
	}

	// カメラの行列は常に更新
	camera_->UpdateMatrix();
}

void CameraController::Reset() {

	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	Vector3 idealDestination = targetWorldTransform.translation_ + targettooffset;
	idealDestination.x = std::max(idealDestination.x, movebleArea_.left);
	idealDestination.x = std::min(idealDestination.x, movebleArea_.right);
	idealDestination.y = std::min(idealDestination.y, movebleArea_.bottom);
	idealDestination.y = std::max(idealDestination.y, movebleArea_.top);
	camera_->translation_ = idealDestination;

	// Reset時もモードを戻しておく
	mode_ = Mode::kFollowPlayer;
}



// ズーム演出を開始する（GameSceneから呼ばれる）
void CameraController::StartVictoryZoom(Player* target) {
	mode_ = Mode::kVictoryZoom;
	zoomTimer_ = 0.0f;
	zoomStartPos_ = camera_->translation_; // 現在のカメラ位置を保存

	// 目標地点を計算（
	Vector3 targetPos = target->GetWorldPosition();
	zoomTargetPos_ = {targetPos.x, targetPos.y + 1.0f, -5.0f};


}

// ズーム演出の更新処理（Updateから呼ばれる）
void CameraController::UpdateVictoryZoom() {
	if (zoomTimer_ >= kZoomDuration) {
		// ズームが完了したら、動かない
		camera_->translation_ = zoomTargetPos_;
		return;
	}

	// タイマーを進める
	zoomTimer_ += (1.0f / 60.0f);
	float t = zoomTimer_ / kZoomDuration;

	// EaseInOutSineで滑らかに補間
	float easedT = math_->EaseInOutSine(t, 0.0f, 1.0f);

	// カメラ座標をスタート地点から目標地点へ滑らかに移動させる
	camera_->translation_ = math_->Lerp(zoomStartPos_, zoomTargetPos_, easedT);
}