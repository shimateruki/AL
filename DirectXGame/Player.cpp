#include "Player.h"
#include"cassert"
using namespace KamataEngine;
	
void Player::Initialize(Model* model, uint32_t textureHandle,Camera* camera) 
{
	assert(model); 
	camera_ = camera;
	model_ = model;
	textureHandle_ = textureHandle;
	worldTransform_.Initialize();

	
}

void Player::Update() 
{
	worldTransform_.TransferMatrix(); 
}

void Player::Draw() 
{
	model_->Draw(worldTransform_,*camera_, textureHandle_); 
}
