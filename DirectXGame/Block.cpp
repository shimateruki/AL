#include "Block.h"

using namespace KamataEngine;

void Block::Initialize(Model* model, const Vector3& position, Camera* camera) {
	model_ = model;
	camera_ = camera;
	worldTransformPlayer_.Initialize();
	worldTransformPlayer_.translation_ = position;
}

void Block::Update() { 
	math->worldTransFormUpdate(worldTransformPlayer_); 
}

void Block::Draw() {
	model_->Draw(worldTransformPlayer_, *camera_); 
}