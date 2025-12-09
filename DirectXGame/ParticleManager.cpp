#include "ParticleManager.h"
#include <iostream>

void ParticleManager::Initialize(Model* model, Camera* camera) {
	model_ = model;
	camera_ = camera;
	math = new Math();
	objectColor_.Initialize();
	
}
void ParticleManager::Update() {
	// イテレータを使ってリストを走査
	for (auto it = particles_.begin(); it != particles_.end();) {

		// 1. 移動処理
		it->velocity += it->acceleration;
		it->worldTransform.translation_ += it->velocity;


		if (camera_) {
			it->worldTransform.rotation_ = camera_->rotation_;
		}


		// 2. 寿命処理
		it->lifeTimer += 1.0f / 60.0f;

		// 3. スケール（大きさ）の線形補間
		float t = it->lifeTimer / it->maxLife; 
		float currentScale = math->Lerp(it->startScale, it->endScale, t);
		it->worldTransform.scale_ = {currentScale, currentScale, currentScale};

		// 4. 色の更新（startColor と endColor の間で変化させる）
		// ※ここも少し修正：wだけでなくRGBも変化できるようにしました
		it->color.x = math->Lerp(it->startColor.x, it->endColor.x, t);
		it->color.y = math->Lerp(it->startColor.y, it->endColor.y, t);
		it->color.z = math->Lerp(it->startColor.z, it->endColor.z, t);
		it->color.w = math->Lerp(it->startColor.w, it->endColor.w, t);

		// 完全に透明になったら見えないのでクランプなどは不要

		// 5. 行列更新
		math->worldTransFormUpdate(it->worldTransform);

		// 6. 寿命が尽きたら削除
		if (it->lifeTimer >= it->maxLife) {
			it = particles_.erase(it);
		} else {
			++it;
		}
	}
}

void ParticleManager::Draw(ID3D12GraphicsCommandList* commandList) {

	Model::PreDraw(commandList);
	objectColor_.SetColor({1.0f, 0.0f, 0.0f, 1.0f});
	// 2. 描画ループ
	for (auto& particle : particles_) {
		model_->Draw(particle.worldTransform, *camera_, &objectColor_);
	}

	// 3. 3D描画終了の合図
	Model::PostDraw();
}

void ParticleManager::Emit(const Vector3& pos, const Vector3& vel, const Vector3& accel, float life, float startScale, float endScale, const Vector4& startColor, const Vector4& endColor) {

	// 1. リストの末尾に「空のパーティクル」を直接生成する（コピーが発生しない）
	particles_.emplace_back();

	// 2. 今作ったばかりのパーティクルへの参照を取得する
	Particle& newParticle = particles_.back();

	// 3. 参照を使って直接データを書き込む
	newParticle.worldTransform.Initialize();
	newParticle.worldTransform.translation_ = pos;
	newParticle.worldTransform.rotation_ = {0, 0, 0}; // 必要ならカメラの回転を入れると良いですが、一旦0でOK

	newParticle.velocity = vel;
	newParticle.acceleration = accel;
	newParticle.lifeTimer = 0.0f;
	newParticle.maxLife = life;
	newParticle.startScale = startScale;
	newParticle.endScale = endScale;
	newParticle.startColor = startColor;
	newParticle.endColor = endColor;
	newParticle.color = startColor; // 最初は startColor で始める
}
void ParticleManager::Clear() { particles_.clear(); }