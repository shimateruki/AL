#include "TitleScene.h"
#include "Fade.h"
using namespace KamataEngine;
void TitleScene::Initialize() {
	camera_.Initialize();
	playerModel_ = Model::CreateFromOBJ("player", true);
	camera_.translation_.z = -10.0f; // カメラのZ位置を調整

	// player_ メンバ変数を new でインスタンス化
	player_ = new Player();
	player_->Initialize(playerModel_, &camera_, Vector3{0, -2.0f, -3.0f});
	player_->GetWorldTransform().rotation_.y = std::numbers::pi_v<float>; // 180度回転 (πラジアン)

	// 文字モデルのロードと初期化
	titleTextModel_ = Model::CreateFromOBJ("title", true); // "title.obj" を指定
	titleTextWorldTransform_.Initialize();

	baseTextPos_ = {-1.0f, 0.0f, -7.0f}; // 文字の基準位置

	floatingTimer_ = 0.0f;      // タイマーをリセット
	floatingAmplitudeY_ = 0.1f; // Y軸方向の揺れの振幅を0.1単位に設定
	floatingSpeed_ = 0.05f;     // 揺れの速さ

	// 文字の位置、回転、スケールを調整 (初期は基準位置に設定)
	titleTextWorldTransform_.translation_ = baseTextPos_;
	// rotation_.y をラジアン値で設定し直す
	titleTextWorldTransform_.rotation_.y = std::numbers::pi_v<float>; // カメラの方を向くように180度回転

	worldTransform_.Initialize(); // 例: タイトル画面全体のワールド変換用など

	// ★フェードとフェーズ管理の初期化
	fade_ = new Fade();
	fade_->Initalize();
	// FadeクラスのStartメソッドがフェードの方向と時間を引数にとると仮定
	fade_->Start(Fade::Status::FadeIn, 1.0f); // フェードイン開始 

	phase_ = Phase::kFadeIn; // フェーズをフェードインに設定
	finished_ = false;       // シーンはまだ終了していない
}

void TitleScene::Update() {
	// フェードは常に更新
	fade_->Update();

	// カメラの更新と転送
	camera_.UpdateMatrix();
	camera_.TransferMatrix();

	// 文字の上下揺れアニメーション
	floatingTimer_ += floatingSpeed_;
	titleTextWorldTransform_.translation_.y = baseTextPos_.y + std::sin(floatingTimer_) * floatingAmplitudeY_;

	
	// MathUtility::GetInstance()->worldTransFormUpdate(worldTransform_);
	math->worldTransFormUpdate(player_->GetWorldTransform());
	math->worldTransFormUpdate(titleTextWorldTransform_);

	// ★フェーズごとの処理
	switch (phase_) {
	case Phase::kFadeIn:
		if (fade_->isFinished()) { 
			phase_ = Phase::kMain;       // フェードイン完了 -> メインフェーズへ
		}
		break;

	case Phase::kMain:

		// メインフェーズ中の入力待ち
		if (Input::GetInstance()->PushKey(DIK_SPACE)) { // スペースキーが押されたら
			phase_ = Phase::kfadeOut;                   // フェードアウトへ移行
			fade_->Start(Fade::Status::FadeOut, 1.0f); // フェードアウト開始 
		}
		break;

	case Phase::kfadeOut:
		if (fade_->isFinished()) {
			finished_ = true;             // フェードアウト完了 -> シーン終了
		}
		break;
	}
}

void TitleScene::Draw() {
	DirectXCommon* dxcommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList* commandList = dxcommon->GetCommandList();

	// 3Dモデルの描画
	Model::PreDraw(commandList);
	player_->Draw();
	titleTextModel_->Draw(titleTextWorldTransform_, camera_);
	Model::PostDraw();

	// Fadeの描画
	fade_->Draw(commandList); // commandList を引数に渡す
}

TitleScene::~TitleScene() {
	
	delete player_;
	delete playerModel_;
	delete titleTextModel_;
	delete fade_;
	
}