#pragma once
#include "KamataEngine.h"
class ViewProjection; // 前方宣言
class Player 
{
public:
	// 初期化
	void Initialize(KamataEngine:: Model* model, uint32_t textureHandle);
	// 更新
	void Update();
	// 描画
	void Draw( ViewProjection& viewProjection);

private:
	uint32_t textureHandle_ = 0;                  // テクスチャハンドル
	KamataEngine::WorldTransform worldTransform_; // ワールド変換（おそら
	KamataEngine::Model* model_ = nullptr;

};
