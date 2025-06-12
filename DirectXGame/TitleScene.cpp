#include "TitleScene.h"

using namespace KamataEngine;


void TitleScene::Initialize() 
{
	camera_.Initialize();                                // カメラの初期化
	playerModel_ = Model::CreateFromOBJ("player", true); // プレイヤーモデルの読み込み
	camera_.translation_.z = -10.0f;

	// player_ メンバ変数を new でインスタンス化
	player_ = new Player();
	player_->Initialize(playerModel_, &camera_, Vector3{0, -2.0f, -3.0f});
	
	 player_->GetWorldTransform().rotation_.y = std::numbers::pi_v<float>; // 180度回転 (πラジアン)

	 // ★追加: 文字モデルのロードと初期化
	 titleTextModel_ = Model::CreateFromOBJ("title", true);
	 titleTextWorldTransform_.Initialize();

    baseTextPos_ = {-1.0f, 0.0f, -7.0f};

	 floatingTimer_ = 0.0f;      // タイマーをリセット
	 floatingAmplitudeY_ = 0.1f; // Y軸方向の揺れの振幅を0.5単位に設定
	 floatingSpeed_ = 0.05f;     // 


	 // 文字の位置、回転、スケールを調整
	 titleTextWorldTransform_.translation_ = baseTextPos_;      // プレイヤーの上あたりに表示

	 titleTextWorldTransform_.rotation_.y = 59.6f;

	worldTransform_.Initialize(); // 例: タイトル画面全体のワールド変換用など
}

void TitleScene::Update() 
{
	camera_.UpdateMatrix();   // カメラの内部状態から行列を計算・更新
	camera_.TransferMatrix(); // 更新された行列をGPUに転送

	
       // ★追加: 文字の上下揺れアニメーション
	floatingTimer_ += floatingSpeed_; // タイマーを速さ分進める

	// std::sin は -1.0 から 1.0 の間で周期的に変化します。
	// これを振幅と基準位置に掛け合わせてY座標を計算します。
	titleTextWorldTransform_.translation_.y = baseTextPos_.y + std::sin(floatingTimer_) * floatingAmplitudeY_;

	// ワールド行列更新
	math->worldTransFormUpdate(worldTransform_); 
	math-> worldTransFormUpdate(player_->GetWorldTransform());
	math->worldTransFormUpdate(titleTextWorldTransform_);
	if (Input::GetInstance()->PushKey(DIK_SPACE)) {
		finished_ = true;
	}
}

void TitleScene::Draw() 
{
	DirectXCommon* dxcommon = DirectXCommon::GetInstance(); // DirectXCommonのインスタンスを取得
	Model::PreDraw(dxcommon->GetCommandList());             // モデル描画の前処理（コマンドリストの設定など）

	player_->Draw();
	titleTextModel_->Draw(titleTextWorldTransform_, camera_);
	Model::PostDraw(); // モデル描画の後処理


}
