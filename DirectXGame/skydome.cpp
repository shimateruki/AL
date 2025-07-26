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
	DirectXCommon* dxcommon = DirectXCommon::GetInstance(); // DirectXCommonのインスタンスを取得
	// Fadeの描画
	Model::PreDraw(dxcommon->GetCommandList()); // モデル描画の前処理（コマンドリストの設定など
	model_->Draw(worldTransformSkydome_, *camera_); 
	Model::PostDraw(); // モデルの描画後処理
}
