#define NOMINMAX
#include "Player.h"
#include "cassert"
#include <algorithm>
#include <numbers>

using namespace KamataEngine;

void Player::Initialize(Model* model, Camera* camera, Vector3& position) {
	assert(model);
	camera_ = camera;
	model_ = model;

	worldTransformPlayer_.Initialize();
	worldTransformPlayer_.translation_.z =0.0f;
	worldTransformPlayer_.translation_ = position;
	worldTransformPlayer_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

void Player::Update() {
	bool landing = false;
	if (onGround_) {

		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				// 左移動中の右入力
				if (velosity_.x < 0.0f) {
					// 速度と逆入力に入力中は急ブレーキ
					velosity_.x *= (1.0f - kAtteunuation);
				}
				acceleration.x += kAcceleration;

				// 左右状態の切り替え
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;

							// 旋回開始時の角度を記録する
					turnFirstRottationY_ = worldTransformPlayer_.rotation_.y;
					// 旋回タイマーに時間を設定する
					turnTimer_ = kTimeTurn;
				}
		

			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {

				// 左移動中の右入力
				if (velosity_.x > 0.0f) {
					// 速度と逆入力に入力中は急ブレーキ
					velosity_.x *= (1.0f - kAtteunuation);
				}

				acceleration.x -= kAcceleration;

				// 左右状態の切り替え
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;

						// 旋回開始時の角度を記録する
					turnFirstRottationY_ = worldTransformPlayer_.rotation_.y;
					// 旋回タイマーに時間を設定する
					turnTimer_ = kTimeTurn;
				}
			
			}

			// 加速　減速
			velosity_.x += acceleration.x;
			velosity_.y += acceleration.y;

			// 最大速度制限
			velosity_.x = std::clamp(velosity_.x, -kLimitRunSpeed, kLimitRunSpeed);

		} else {
			velosity_.x *= (1.0f - kAtteunuation);
		}
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			velosity_.y += kJumpAccleration;
			// ジャンプ開始
			if (velosity_.y > 0.0f) {
				// 空中状態に移行
				onGround_ = false;
			}
		}

		//空中
	} else {
		velosity_.y += -kGgravityAcceleration/60.0f;
		velosity_.y = std::max(velosity_.y, -kLimitFallSpeed);

		// 地面との当たり判定
		if (velosity_.y < 0) {
			// y座標が地面以下のなったら着地
			if (worldTransformPlayer_.translation_.y <= 1.0f) {
				landing = true;
			}
		}
		//着地
		if (landing) {
			// めり込み
			worldTransformPlayer_.translation_.y = 1.0f;
			// 摩擦で横方向速度に減衰する
			velosity_.x *= (1.0f - kAtteunuation);
			// 下方向に速度をリセット
			velosity_.y = 0.0f;
			// 着地状態に移行
			onGround_ = true;
		}
	}
	// 加速　減速
	// 移動
	worldTransformPlayer_.translation_.x += velosity_.x;
	worldTransformPlayer_.translation_.y += velosity_.y;
	worldTransformPlayer_.translation_.z += velosity_.z;
	// 旋回制御
	if (turnTimer_ > 0) {

		turnTimer_ -= 1.0f / 60.0f;
		float t = 1.0f - (turnTimer_ / kTimeTurn);

		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};

		// 状態に応じた角度を取得する
		float destinationRottaionY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		// 自キャラの角度を設定する
		worldTransformPlayer_.rotation_.y = math->EaseInOutSine(t, turnFirstRottationY_, destinationRottaionY);
	}

	

	// 行列更新
	math->worldTransFormUpdate(worldTransformPlayer_);
}

void Player::Draw() { model_->Draw(worldTransformPlayer_, *camera_); }
