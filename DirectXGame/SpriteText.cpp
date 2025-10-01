#include "SpriteText.h"

void SpriteText::Initialize(Model* model, Camera* camera, const Vector3& position) 
{
	model_ = model;
	camera_ = camera;
	worldTransform_.translation_ = position;
	worldTransform_.Initialize();

	objectColor_.Initialize();
	objectColor_.SetColor(Vector4{1.0f, 1.0f, 1.0f, 1.0f}); // 初期色を白に設定
	

}

void SpriteText::Update() 
{ math->worldTransFormUpdate(worldTransform_); }

void SpriteText::Draw() 
{
	model_->Draw(worldTransform_, *camera_,&objectColor_);
}
// ★ 追加: 色を設定する関数の実装
void SpriteText::SetColor(const Vector4& color) { objectColor_.SetColor(color); }