
#include "Enemy.h" // Enemyクラスのヘッダーファイルをインクルード
#include <cassert> // assertマクロを使用するためにインクルード
#include <numbers> // std::numbers::pi_v を使用するためにインクルード 
#include"Player.h"
#include "GameScene.h"
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
	gameScene_ = nullptr; // ゲームシーンへのポインタを初期化 (nullptrに設定)

    }

// 更新処理 
void Enemy::Update() {
	if (behaviorRequest_ != Behavior::kUnKnow) {
		// 振るまいを変更する
		behavior_ = behaviorRequest_;
		// 各振るまいごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			// 通常行動の初期化は特に必要ない場合
			break;
		case Behavior::kisDead:
			// 死亡状態の初期化処理 (必要に応じて追加)
			walkTimer = 0.0f; // 死亡アニメーションのタイマーをリセット
			break;
		}
		// 振るまいリクエストをリセット
		behaviorRequest_ = Behavior::kUnKnow;
	}

	switch (behavior_) {
	case Enemy::Behavior::kUnKnow:
		// 敵のワールド行列を更新

		break;
	case Enemy::Behavior::kRoot:
		// 敵の現在位置に速度を加算して移動
		worldTransformEnemy_.translation_ += velocity_;

		// 歩行タイマーを更新 (1秒間に60フレームを想定)
		walkTimer += 1.0f / 60.0f;

		// 歩行モーションのためのX軸周りの回転アニメーション
		//
		// sin波を使って腕や足の動きを表現するようなアニメーション
		worldTransformEnemy_.rotation_.x = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer / kWalkMotionTime);

		// 敵のワールド行列を更新
		math->worldTransFormUpdate(worldTransformEnemy_);
		break;
	case Enemy::Behavior::kisDead:
		//アニメーションタイマーを加速する
		walkTimer += 1.0f / 60.0f;
		// 死亡アニメーションのためのX軸周りの回転アニメーション
		worldTransformEnemy_.rotation_.y = math->EaseInOutSine(walkTimer / kWalkMotionTime, kWalkMotionAngleStart, kWalkMotionAngleEnd);
		worldTransformEnemy_.rotation_.x = math->EaseInOutSine(walkTimer / kWalkMotionTime, 0.0f, -kWalkMotionAngleEnd);
		// 死亡アニメーションの時間が経過したら、敵を非表示にする
		if (walkTimer >= 1.0f) {
			isDead_ = true; // 敵が死亡したときの処理
		}
		break;
	default:
		break;
	}
	math->worldTransFormUpdate(worldTransformEnemy_);
}

// 描画処理 
void Enemy::Draw() {
	// 敵のモデルを描画 
	model_->Draw(worldTransformEnemy_, *camera_);
}

Vector3 Enemy::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得
	worldPos.x =worldTransformEnemy_.matWorld_.m[3][0]; // 行列の4行1列目
	worldPos.y =worldTransformEnemy_.matWorld_.m[3][1]; // 行列の4行2列目
	worldPos.z =worldTransformEnemy_.matWorld_.m[3][2]; // 行列の4行3列目
	return worldPos;
}

AABB Enemy::GetAABB() {
	Vector3 worldPos = GetWorldPosition();
	AABB aabb;
	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};
	return aabb;
}

// プレイヤーとの衝突処理 (プレイヤーとの衝突
void Enemy::onCollision(const Player* player) 
{
	if (behavior_ == Behavior::kisDead)
	{
		return;
	}
	if (player->GetIsAttack()) {
		isCollisDisabled_ = true;             // 衝突無効化フラグを立てる
		behaviorRequest_ = Behavior::kisDead; // プレイヤーが攻撃中なら敵の行動を死亡状態に変
        // プレイヤーのワールド変換の平行移動成分を取得して加算する 
	
			Vector3 effectPos = (GetWorldPosition() + player->GetWorldPosition())/2.0f;
			gameScene_->CreateHitEffect(effectPos); // ヒットエフェクトを生成
	}


}
