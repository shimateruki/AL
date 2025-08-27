#pragma once
#include "KamataEngine.h"
#include "math.h"

using namespace KamataEngine;
class Signboard 
{
public:

	// 初期化関数
	void Initialize(Model* model, Camera* camera, const Vector3& position, int stageID);
	// 更新関数
	void Update();
	// 描画関数
	void Draw();
	void DrawT();
	Vector3 GetWorldPosition() const;

	AABB GetAABB();
	int GetStageID() const { return stageID_; }

private:
	// ワールド変換
	KamataEngine::WorldTransform worldTransform_;
	// モデル
	KamataEngine::Model* model_ = nullptr;
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;
	Math* math = nullptr; // 数学ユーティリティクラスのインスタンス
	const float kWidth = 1.0f;          // 幅
	const float kHeight = 0.8f;         // 高さ
	int stageID_ = -1;                  // 看板が紐づけるステージ番号

		 	 // テクスチャ
	KamataEngine::Sprite* numberSprite_ = nullptr; // テクスチャ


};
