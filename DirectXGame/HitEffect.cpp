#include "HitEffect.h"
#include <random>
#include <numbers>

KamataEngine::Model* HitEffect::model_ = nullptr; // モデルへのポインタ
KamataEngine::Camera* HitEffect::camera_ = nullptr; // カメラへのポインタ

using namespace KamataEngine;

void HitEffect::Initialize(Vector3 postion) 
{
	std::random_device seedGenerator;
	std::mt19937_64 randomEngine;
	randomEngine.seed(seedGenerator());
	std::uniform_real_distribution<float> rotationDistribution(-std::numbers::pi_v<float>, std::numbers::pi_v<float>);

	// 楕円エフェクト
	for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
		// 楕円エフェクトの位置をランダムに設定
		worldTransform.Initialize();
		worldTransform.rotation_ = {0.0f, 0.0f, rotationDistribution(randomEngine)};
		worldTransform.translation_ = postion + Vector3{0.0f, 1.5f, 0.0f}; // 手動で補正

	
	}

	// 円形エフェクト

	circleWorldTransform_.Initialize();
	circleWorldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	circleWorldTransform_.translation_ = postion;


	objectColor_.Initialize();

	objectColor_.SetColor(Vector4{1.0f, 1.0f, 1.0f, 1.0f});




	

}

void HitEffect::Update()
{
	if (IsDead()) {

		return;
	}
	  switch (state_) {
	  case State::kSpread: {
		  ++counter_;
		  float scale = 0.5f + static_cast<float>(counter_) / kSpreadTime * 0.5f;
		  const float slashScale = 1.5f;
		  for (auto& slashWorldTransform : ellipseWorldTransforms_) {
			  slashWorldTransform.scale_ = {0.1f, scale * slashScale, 1.0f};
		  }
		  const float circleScale = 1.0;
		  circleWorldTransform_.scale_ = {scale * circleScale, scale * circleScale, 1.0f};
		  if (counter_ >= kSpreadTime) {
			  state_ = State::kFade;
			  counter_ = 0; // カウンターをリセット
		  }
		  break;
	  }
	  case State::kFade: {
		  ++counter_;
		  objectColor_.SetColor(Vector4{1.0f, 1.0f, 1.0f, 1.0f - static_cast<float>(counter_) / kFadeTime});
		  if (counter_ >= kFadeTime) {
			  state_ = State::kDead;
		  }
		  break;
	  }
	  default:
		  break;
	  }

	for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
		math->worldTransFormUpdate(worldTransform);
	}
	math->worldTransFormUpdate(circleWorldTransform_);

	
}

void HitEffect::Draw() 
{ 
		KamataEngine::DirectXCommon* dxcommon = KamataEngine::DirectXCommon::GetInstance(); // DirectXCommonのインスタンスを取得
	// Fadeの描画
	    KamataEngine::Model::PreDraw(dxcommon->GetCommandList()); // モデル描画の前処理（コマンドリストの設定など）
	   if (IsDead()) { 
		   return;
	}

	for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
		model_->Draw(worldTransform, *camera_, &objectColor_); 
	}
	
	model_->Draw(circleWorldTransform_, *camera_, &objectColor_); 
	 KamataEngine::Model::PostDraw(); 
	                   // モデルの描画
	
}
	




HitEffect* HitEffect::create(KamataEngine::Vector3 position) 
{
	HitEffect* hitEffect = new HitEffect(); // HitEffectのインスタンスを生成
	if (hitEffect) {
		// 初期化時に正しい型のKamataEngine::Vector3を渡す
		hitEffect->Initialize(position);
	}
	return hitEffect; // 初期化されたHitEffectを返す
}

HitEffect::~HitEffect() {

}
