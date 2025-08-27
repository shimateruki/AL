#include "Signboard.h"

void Signboard::Initialize(Model* model, Camera* camera, const Vector3& position, int stageID) {
	model_ = model;
	camera_ = camera;
	worldTransform_.translation_ = position;
	worldTransform_.translation_.z = 0.5f;
	worldTransform_.Initialize();
	stageID_ = stageID; // ステージIDを設定
	                    // ここからスプライトの初期化を追加



}


void Signboard::Update() 
{


	math->worldTransFormUpdate(worldTransform_);

;
}

void Signboard::Draw() 
{
	
	model_->Draw(worldTransform_, *camera_);
	


}


Vector3 Signboard::GetWorldPosition() const 
{
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得
	worldPos.x = worldTransform_.matWorld_.m[3][0]; // 行列の4行1列目
	worldPos.y = worldTransform_.matWorld_.m[3][1]; // 行列の4行2列目
	worldPos.z = worldTransform_.matWorld_.m[3][2]; // 行列の4行3列目
	return worldPos;
}

AABB Signboard::GetAABB() 
{
	Vector3 worldPos = GetWorldPosition(); // ワールド座標を取得
	AABB aabb;
	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};
	return aabb;
}
