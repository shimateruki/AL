#include "keyModel.h"

void keyModel::Initialize(Model* model, Camera* camera, const Vector3& position) 
{
	model_ = model;                          // キーモデルを設定
	worldTransform_.translation_ = position; // キーの位置を設定
	worldTransform_.Initialize();            // ワールド変換行列の初期化
	camera_ = camera;                        // カメラへのポインタを設定
	isActive_ = false;                        // 初期状態ではアクティブに設定

	objectColor_.Initialize();
	objectColor_.SetColor(Vector4{1.0f, 1.0f, 1.0f, 1.0f}); // 初期色を白に設定
}
void keyModel::Update() {
	math->worldTransFormUpdate(worldTransform_); // ワールド変換行列の更新
}
void keyModel::Draw() {
	if (isActive_) {                             // キーがアクティブな場合のみ描画
		model_->Draw(worldTransform_, *camera_,&objectColor_); // モデルの描画
	}
}

// ★ 追加: 色を設定する関数の実装
void keyModel::SetColor(const Vector4& color) { objectColor_.SetColor(color); }
