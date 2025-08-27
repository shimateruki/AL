#pragma once
#include "KamataEngine.h"
#include "math.h"
using namespace KamataEngine;
class keyModel 
{
public:
	void Initialize(Model* model, Camera* camera, const Vector3& position);
	void Update();
	void Draw();
	void SetActive(bool active) { isActive_ = active; } // キーのアクティブ状態を設定する関数
	void SetColor(const Vector4& color);

private:
	Model* model_ = nullptr;        // キーモデル
	WorldTransform worldTransform_; // ワールド変換行列
	Camera* camera_ = nullptr;      // カメラへのポインタ
	bool isActive_ = false;         // キーがアクティブかどうかのフラグ
	Math* math;
	KamataEngine::ObjectColor objectColor_; // オブジェクトの色設定

};
