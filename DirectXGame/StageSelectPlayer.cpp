#include "StageSelectPlayer.h"
#include"numbers"

void StageSelectPlayer::Initialize(Model* model, Camera* camera, const Vector3& position) {
	model_ = model;
	camera_ = camera;
	worldTransformPlayer_.translation_ = position;
	worldTransformPlayer_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransformPlayer_.rotation_ = {0.0f, 0.0f, 0.0f};
	worldTransformPlayer_.Initialize();
	prevPosition_ = position; // 初期位置を保存
}

void StageSelectPlayer::HandleInput() {
	if (Input::GetInstance()->TriggerKey(DIK_D)) {
		if (currentNodeIndex_ < nodes_.size() - 1) {
			currentNodeIndex_++;
		}
	}
	if (Input::GetInstance()->TriggerKey(DIK_A)) {
		if (currentNodeIndex_ > 0) {
			currentNodeIndex_--;
		}
	}
}

void StageSelectPlayer::Update() {
	HandleInput();

	Vector3 currentPos = worldTransformPlayer_.translation_;
	Vector3 targetPos = nodes_[currentNodeIndex_];
	Vector3 toTarget = targetPos - currentPos;
	float distance = math->Length(toTarget);

	if (distance > 0.05f) {
		// 目標に向かって移動
		Vector3 dir = math->Normalize(toTarget);
		worldTransformPlayer_.translation_ += dir * moveSpeed_;

		// 移動方向で向きを決定
		if (dir.x > 0) {
			targetRotationY_ = std::numbers::pi_v<float> / 2.0f; // 右
		} else if (dir.x < 0) {
			targetRotationY_ = std::numbers::pi_v<float> * 3.0f / 2.0f; // 左
		}
	} else {
		// ノード到達 → 看板正面を向く
		worldTransformPlayer_.translation_ = targetPos;
		targetRotationY_ = 0.0f;
	}

	// 回転をスムーズに補間
	worldTransformPlayer_.rotation_.y = math->Lerp(worldTransformPlayer_.rotation_.y, targetRotationY_, 0.1f);

	math->worldTransFormUpdate(worldTransformPlayer_);
}

void StageSelectPlayer::Draw() {
	if (model_ && camera_) {
		model_->Draw(worldTransformPlayer_, *camera_);
	}
}

void StageSelectPlayer::AddNode(const Vector3& nodePos) { nodes_.push_back(nodePos); }

Vector3 StageSelectPlayer::GetWorldPosition() const { // ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得
	worldPos.x = worldTransformPlayer_.matWorld_.m[3][0]; // 行列の4行1列目
	worldPos.y = worldTransformPlayer_.matWorld_.m[3][1]; // 行列の4行2列目
	worldPos.z = worldTransformPlayer_.matWorld_.m[3][2]; // 行列の4行3列目
	return worldPos;
}

AABB StageSelectPlayer::GetAABB() {

	Vector3 worldPos = GetWorldPosition(); // ワールド座標を取得
	AABB aabb;
	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};
	return aabb;
}