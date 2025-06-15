#include "Fade.h"
#include<cmath>
#include<algorithm>
using namespace KamataEngine;
void Fade::Initalize() 
{
	// スプライト用テクスチャハンドル（単色テクスチャでもOK）
	uint32_t textureHandle = TextureManager::Load("white1x1.png"); // 白1x1の画像（透明度操作がしやすい）

	// スプライト生成
	sprite_ = Sprite::Create(textureHandle, {0.0f, 0.0f});
	sprite_->SetSize(Vector2(1280.0f, 720.0f));
	sprite_->SetColor(Vector4(0, 0, 0, 1));
}

void Fade::Draw(ID3D12GraphicsCommandList* commandList) {
	if (status_ == Status::None) {
		return;
	} 
	Sprite::PreDraw(commandList); // 引数から受け取る
	sprite_->Draw();   
	Sprite::PostDraw();
}

void Fade::Update() 
{
	switch (status_) {
	case Fade::Status::None:
		break;
	case Fade::Status::FadeIn:

				// フェードイン（黒 → 透明）
		counter_ += 1.0f / 60.0f;
		if (counter_ >= duration_) {
			counter_ = duration_;
		}
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(1.0f - (counter_ / duration_), 0.0f, 1.0f)));
		break;
		
	case Fade::Status::FadeOut:
	 counter_ += 1.0f / 60.0f;
		if (counter_ >= duration_) {
			counter_ = duration_;
		}
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(counter_ / duration_, 0.0f, 1.0f)));
		break;

	}
}

void Fade::Start(Status status, float duration) 
{
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
}

void Fade::stop() 
{ status_ = Status::None; }

bool Fade::isFinished() const {
	switch (status_) {
	case Status::FadeOut:
	case Status::FadeIn:
		return counter_ >= duration_;
	default:
		return false;
	}
}