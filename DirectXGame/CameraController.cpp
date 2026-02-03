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
void CameraController::StartShake() { shakeTimer_ = kShakeDuration; }

void CameraController::Update() {

	// =========================================================
	// 1. モードごとの基本動作（追従 or ズーム）
	// =========================================================
	switch (mode_) {

	case Mode::kFollowPlayer: { // --- 通常の追従処理 ---

		// ターゲットがいない場合は処理しない
		if (!target_)
			break;

		// 追従対象の情報を取得
		const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
		const Vector3& targetVelocity = target_->GetVelocity();

		// 目標地点の計算（プレイヤー位置 ＋ 固定オフセット ＋ プレイヤーの移動先を予知）
		Vector3 idealDestination = targetWorldTransform.translation_ + targettooffset + targetVelocity;

		// ---------------------------------------------------------
		// 移動範囲の制限 (Clamp)
		// ---------------------------------------------------------

		// X軸の制限
		idealDestination.x = std::clamp(idealDestination.x, movebleArea_.left, movebleArea_.right);

		// Y軸の制限
		idealDestination.y = std::clamp(idealDestination.y, movebleArea_.bottom, movebleArea_.top);

		// ---------------------------------------------------------
		// 補間移動 (Lerp)
		// ---------------------------------------------------------
		// 現在地から目標地点へ、ゆっくり近づける
		// ※ kInterpolationRate はメンバ変数として持っている前提（例: 0.1f）
		float rate = 0.1f; // もし変数がなければここで定義、あればメンバ変数を使ってください
		camera_->translation_ = math_->Lerp(camera_->translation_, idealDestination, rate);

	} break;

	case Mode::kVictoryZoom: { // --- ズーム演出の処理 ---
		UpdateVictoryZoom();
	} break;
	}

	// =========================================================
	// 2. カメラシェイク処理（モードに関係なく揺らす）
	// =========================================================
	if (shakeTimer_ > 0.0f) {
		// タイマーを減らす
		shakeTimer_ -= 1.0f / 60.0f;

		// ランダムなズレを作成
		// rand() % 100 で 0～99 の乱数を作る
		// -50 して -50～49 にする
		// 50.0f で割って -1.0～1.0 の小数にする
		float offsetX = (rand() % 100 - 50) / 50.0f * kShakePower;
		float offsetY = (rand() % 100 - 50) / 50.0f * kShakePower;

		// 時間経過で揺れを弱くする（フェードアウト効果）
		// 残り時間が減るほど揺れ幅も小さくなる
		float decay = shakeTimer_ / kShakeDuration;

		// 最終的なズレを現在のカメラ座標に加算
		camera_->translation_.x += offsetX * decay;
		camera_->translation_.y += offsetY * decay;
	}

	// =========================================================
	// 3. 行列の更新（最後に必ず呼ぶ）
	// =========================================================
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



// ズーム演出を開始する
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