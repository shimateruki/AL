#include "skydome.h"
using namespace KamataEngine;


void Skydome::Initialize(Model* model,Camera* camera) 
{ 
		assert(model); 
	model_ = model;

	worldTransformSkydome_.Initialize();
	camera_ = camera;

}

void Skydome::Update() {
	worldTransformSkydome_.matWorld_ = math_->MakeAffineMatrix(worldTransformSkydome_.scale_,
		worldTransformSkydome_.rotation_, worldTransformSkydome_.translation_);
	worldTransformSkydome_.TransferMatrix(); 
}

void Skydome::Draw() {
	model_->Draw(worldTransformSkydome_, *camera_); 
}
