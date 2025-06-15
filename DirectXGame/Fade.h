#pragma once
#include "KamataEngine.h"
class Fade 
{
public:
	enum class Status {

		None,
		FadeIn,
		FadeOut,
	};
	void Initalize();
	void Draw(ID3D12GraphicsCommandList* commandList);
	void Update();
	void Start(Status status, float duration);
	void stop();
	//フェード終了処理
	bool isFinished()const;
	

private:
	KamataEngine::Sprite* sprite_ = nullptr;
	Status status_ = Status::None;
	//フェード時間
	float duration_ = 0.0f;
	//経過時間カウンター
	float counter_ = 0.0f;
};
