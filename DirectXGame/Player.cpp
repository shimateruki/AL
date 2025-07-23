#include "Player.h"
using namespace KamataEngine ;



void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandle) 
{

	model_ = model;                                    // モデルの設定
	textureHandle_ = textureHandle;                    // テクスチャハンドルの設定
	worldTransform_.Initialize();                      // ワールド変換の初期化

}

void Player::Update() 
{ worldTransform_.TransferMatrix(); }

void Player::Draw(ViewProjection& viewProjection) 
{
	model_->Draw(worldTransform_, *viewProjection,textureHandle_); 
}

