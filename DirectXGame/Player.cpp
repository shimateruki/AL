#include "Player.h"
#include"cassert"
using namespace KamataEngine;
	
void Player::Initialize(Model* model, uint32_t textureHandle,Camera* camera) 
{
	assert(model); 
	camera_ = camera;
model_ = model;
	textureHandle_ = textureHandle;
	worldTransformPlayer_.Initialize();

	
}

void Player::Update() 
{
	math->worldTransFormUpdate(worldTransformPlayer_); 
}

void Player::Draw() 
{
	model_->Draw(worldTransformPlayer_, *camera_); 
}
