#include "tree.h"

void Tree::Initialize(Model* model, Camera* camera, const Vector3& position) 
{
	model_ = model;
	camera_ = camera;
	worldTransform_.translation_ = position;
	worldTransform_.translation_.z = 5.5f;
	worldTransform_.Initialize();
	math = new Math();
}
void Tree::Update() { math->worldTransFormUpdate(worldTransform_); }
void Tree::Draw() { model_->Draw(worldTransform_, *camera_); }