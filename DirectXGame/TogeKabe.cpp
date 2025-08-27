#include "TogeKabe.h"

void TogeKabe::Initialize(Model* model, Camera* camera, const Vector3& position) 
{
	model_ = model;                                        // モデルの設定
	camera_ = camera;                                      // カメラの設定
	worldTransform_.Initialize();                          // ワールド変換の初期化
	worldTransform_.scale_ = Vector3(1.0f, 1.0f, 1.0f);    // スケールの初期化（単位スケール）
	worldTransform_.rotation_ = Vector3(0.0f, 0.0f, 0.0f); // 回転の初期化（単位回転）
	worldTransform_.translation_ = position;               // 初期位置の設定
	math = new Math;
}

void TogeKabe::Update() 
{
	math->worldTransFormUpdate(worldTransform_); // インスタンスを使用してメソッドを呼び出す
}

void TogeKabe::Draw() 
{
	model_->Draw(worldTransform_, *camera_); // モデルの描画
}

Vector3 TogeKabe::GetWorldPosition() const 
{
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得
	worldPos.x = worldTransform_.matWorld_.m[3][0]; // 行列の4行1列目
	worldPos.y = worldTransform_.matWorld_.m[3][1]; // 行列の4行2列目
	worldPos.z = worldTransform_.matWorld_.m[3][2]; // 行列の4行3列目
	return worldPos;
}

AABB TogeKabe::GetAABB() 
{
	Vector3 worldPos = GetWorldPosition(); // ワールド座標を取得
	AABB aabb;
	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};
	return aabb;
}
