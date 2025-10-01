#include "skydome.h"
using namespace KamataEngine;


void Skydome::Initialize(Model* model,Camera* camera) 
{ 
  #ifdef _DEBUG
		assert(model);
#endif
	model_ = model;

	worldTransformSkydome_.Initialize();
	worldTransformSkydome_.translation_ = {0.0f, 70.0f, 0.0f};
	worldTransformSkydome_.scale_ = {1.0f, 0.3f, 1.0f}; // スカイドームのスケール
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
