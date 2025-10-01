#include "yama.h"

void Yama::Initialize(Model* model, Camera* camera, const Vector3& position) 
{
	model_ = model;
	camera_ = camera;
	worldTransform_.translation_ = position;
	worldTransform_.translation_.z = 18.0f;
	worldTransform_.Initialize();
	color_.Initialize();
	color_.SetColor({0.0f, 0.5f, 0.0f, 1.0f}); // 緑色で初期化
}
void Yama::Update() { math->worldTransFormUpdate(worldTransform_); }
void Yama::Draw() { model_->Draw(worldTransform_, *camera_, &color_); }
