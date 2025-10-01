#pragma once
#include "KamataEngine.h"
#include "math.h"
using namespace KamataEngine;

class SpriteText 
{
public:
	// 初期化関数
	void Initialize(Model* model, Camera* camera, const Vector3& position);
	// 更新関数
	void Update();
	// 描画関数
	void Draw();
	void SetColor(const Vector4& color);
	// ワールド座標取得
	Vector3 GetWorldPosition() const;
	// AABB取得
	AABB GetAABB();

private:
	Model* model_ = nullptr;        // モデル
	Camera* camera_ = nullptr;      // カメラ
	WorldTransform worldTransform_; // ワールド変換
	Math* math = nullptr;           // 数学ユーティリティクラスのインスタンス
	std::wstring text_;             // 表示するテキスト
	float size_ = 1.0f;             // テキストサイズ
	const float kWidth = 1.0f;      // AABBの幅
	const float kHeight = 0.8f;     // AABBの高さ
	KamataEngine::ObjectColor objectColor_; // オブジェクトの色設定

};
