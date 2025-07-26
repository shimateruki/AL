#define NOMINMAX
#include "Player.h"
#include "ViewProjection.h"
#include <imgui.h>
#include <algorithm> // std::max, std::min
using namespace KamataEngine ;



void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandle) 
{

	model_ = model;                                    // モデルの設定
	textureHandle_ = textureHandle;                    // テクスチャハンドルの設定
	worldTransform_.Initialize();                      // ワールド変換の初期化

	

}

void Player::Update() 
{

	

	Vector3 move = {0, 0, 0}; // 移動ベクトルの初期化
	const float kMoveSpeed = 0.1f; // 移動速度の定数
	// 入力に応じて移動ベクトルを更新

	if (input_->PushKey(DIK_LEFT)) {
		move.x -= kMoveSpeed; // 左移動
	}
	if (input_->PushKey(DIK_RIGHT)) {
		move.x += kMoveSpeed; // 右移動
	}

	//押した方向で移動ベクトルの変更
	if (input_->PushKey(DIK_UP)) {
		move.z -= kMoveSpeed; // 前進
	}
	if (input_->PushKey(DIK_DOWN)) {
		move.z += kMoveSpeed; // 後退
	}
	worldTransform_.translation_ += move; // ワールド変換の位置を更新

	const float kMoveLimitx = 30.0f; // x軸の移動制限
	const float kMoveLimity = 18.0f; // z軸の移動制限
	// 移動制限を適用
	worldTransform_.translation_.x = std::max(worldTransform_.translation_.x, -kMoveLimitx);
	worldTransform_.translation_.x = std::min(worldTransform_.translation_.x, kMoveLimitx);
	worldTransform_.translation_.y = std::max(worldTransform_.translation_.y, -kMoveLimity);
	worldTransform_.translation_.y = std::min(worldTransform_.translation_.y, kMoveLimity);

	Math::worldTransFormUpdate(worldTransform_); // ワールド変換の行列を更新

	ImGui::Begin("Player Transform Debug"); // ←ウィンドウタイトルは任意の文字列でOK
	ImGui::DragFloat3("Player", &worldTransform_.translation_.x);
	ImGui::End();

}

void Player::Draw( ViewProjection& viewProjection) 
{ model_->Draw(worldTransform_, viewProjection.GetCamera(), textureHandle_); }

