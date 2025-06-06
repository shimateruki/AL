#include "DeathParticles.h"


void DeathParticles::Initialize(Model* model, Camera* camera, const Player* player, const Vector3& position) 
{
	camera_ = camera;
	model_ = model;
	player;
	for (auto& worldTransform : worldTransform_) 
	{
		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}
}

void DeathParticles::Update() {

	//移動処理
	for (uint32_t i = 0; i < kNumParticles; i++) 
	{
		//基本となる速度ベクトル
		Vector3 velosity = {kSpeed, 0, 0};
		//回転速度を計算する
		float angle = kAngleUnit * i;
		//z軸周りの回転行列
		Matrix4x4 matrixRotation = math->MakeRotateZMatrix(angle);
		//基本ベクトルを回転させて速度ベクトルを得る
		velosity = math->Transform(velosity, matrixRotation);
		//移動処理
		worldTransform_[i].translation_ += velosity;
	}
	//カウンターを1フレーム分の秒数を進める
	counter_ += 1.0f / 60.0f;
	//存在時間の上限に達したら
	if (counter_ >= kDuration) 
	{
		counter_ = kDuration;
		//終了扱いにする
		isFinished_ = true;
	}
	if (isFinished_) 
	{
		return;
	}


	for (auto& worldTransform : worldTransform_) 
	{
		math->worldTransFormUpdate(worldTransform);
	}
}

void DeathParticles::Draw() 
{
	if (isFinished_) {
		return;
	}
	for (auto& worldTransform : worldTransform_) {
		model_->Draw(worldTransform, *camera_);
	}
	
}
