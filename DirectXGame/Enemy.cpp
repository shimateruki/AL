
#include "Enemy.h" // Enemyクラスのヘッダーファイルをインクルード
#include <cassert> // assertマクロを使用するためにインクルード
#include <numbers> // std::numbers::pi_v を使用するためにインクルード 

    // 初期化処理 (02_09 スライド5枚目)
    void
    Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
	// モデルがNULLでないことを確認 (NULLチェック)
	assert(model);

	// メンバー変数にモデルとカメラへのポインタを格納 
	model_ = model;
	camera_ = camera;

	// 敵のワールド変換を初期化 
	worldTransformEnemy_.Initialize();
	// 敵の初期位置を設定
	worldTransformEnemy_.translation_ = position;
	// 敵の初期回転角度を設定 (Y軸周りに270度、おそらく特定方向を向かせるため) 
	worldTransformEnemy_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;

	// 敵の移動速度を設定 (X軸負の方向に歩行速度で移動) 
	velocity_ = {-kWalkSpeed, 0, 0};
	// 歩行タイマーを初期化 
	walkTimer = 0.0f;
}

// 更新処理 
void Enemy::Update() {
	// 敵の現在位置に速度を加算して移動 
	worldTransformEnemy_.translation_ += velocity_;

	// 歩行タイマーを更新 (1秒間に60フレームを想定) 
	walkTimer += 1.0f / 60.0f;

	// 歩行モーションのためのX軸周りの回転アニメーション 
	// sin波を使って腕や足の動きを表現するようなアニメーション
	worldTransformEnemy_.rotation_.x = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer / kWalkMotionTime);

	// 敵のワールド行列を更新
	math->worldTransFormUpdate(worldTransformEnemy_);
}

// 描画処理 
void Enemy::Draw() {
	// 敵のモデルを描画 
	model_->Draw(worldTransformEnemy_, *camera_);
}