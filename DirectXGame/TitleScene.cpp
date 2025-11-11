#include "TitleScene.h"
#include "Fade.h"
using namespace KamataEngine;
void TitleScene::Initialize() {
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	camera_.Initialize();
	playerModel_ = Model::CreateFromOBJ("player", true);
	textureHandleStart_ = TextureManager::Load("sutart.png"); // スタートボタン用テクスチャの読み込み
	textureHandleEnter_ = TextureManager::Load("enter.png"); // エンターキー用テクスチャの読み込み
	camera_.translation_.z = -10.0f; // カメラのZ位置を調整

	// player_ メンバ変数を new でインスタンス化
	player_ = new Player();
	player_->Initialize(playerModel_, &camera_, Vector3{0, -2.0f, -3.0f});
	player_->GetWorldTransform().rotation_.y = std::numbers::pi_v<float>; // 180度回転 (πラジアン)


	// 文字モデルのロードと初期化
	titleTextModel_ = Model::CreateFromOBJ("title", true); // "title.obj" を指定
	titleTextWorldTransform_.Initialize();
	color_.Initialize();
	color_.SetColor({0.5f, 1.0f, 1.0f, 1.0f});

	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);


	baseTextPos_ = {-1.5f, 0.5f, -6.0f}; // 文字の基準位置

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

	startSprite_ = Sprite::Create(textureHandleStart_, {0.0f, 0.0f}); // スタートボタン用スプライトの作成
	enterSprite_ = Sprite::Create(textureHandleEnter_, {0.0f, 0.0f}); // エンターキー用スプライトの作成

	//音楽
	bgmHandle_ = bgmAudio->GetInstance()->LoadWave("BGM/titleBGM.wav");

	isMusic = false;

  enterSeHandle = seAudio->GetInstance()->LoadWave("SE/confirm.wav");

}

void TitleScene::Update() {
	// 常に更新する処理
	fade_->Update();
	skydome_->Update();
	camera_.UpdateMatrix();
	camera_.TransferMatrix();
	//if (!isMusic) {
	//	bgmVoiceHandle_ = bgmAudio->GetInstance()->PlayWave(bgmHandle_, true, 0.5f); // trueでループ再生
	//	isMusic = true;
	//}
	
	// ★フェーズごとの処理
	switch (phase_) {
	case Phase::kFadeIn:
		// --- フェードイン中 ---

		// 文字の上下揺れ
		floatingTimer_ += floatingSpeed_;
		titleTextWorldTransform_.translation_.y = baseTextPos_.y + std::sin(floatingTimer_) * floatingAmplitudeY_;
		math->worldTransFormUpdate(titleTextWorldTransform_);

		// スライムのアイドリングアニメを更新
		player_->UpdateTitleAnimation();

		if (fade_->isFinished()) {
			phase_ = Phase::kMain; 
		}
		break;

	case Phase::kMain:
		// --- メイン（入力待機）中 ---

		// 文字の上下揺れ
		floatingTimer_ += floatingSpeed_;
		titleTextWorldTransform_.translation_.y = baseTextPos_.y + std::sin(floatingTimer_) * floatingAmplitudeY_;
		math->worldTransFormUpdate(titleTextWorldTransform_);

		// スライムのアイドリングアニメを更新
		player_->UpdateTitleAnimation();

		// Enterキーが押されたら演出開始
		if (Input::GetInstance()->TriggerKey(DIK_RETURN)) { 
			phase_ = Phase::kfadeOut;                 

			// スライムにカメラジャンプ開始を命令
			player_->StartCameraJump();
			//se再生

			fade_->Start(Fade::Status::FadeOut, 1.8f);
			enterVoice = seAudio->GetInstance()->PlayWave(enterSeHandle, false, 0.5f);
		}
		break;

	case Phase::kfadeOut: 
		// --- 演出中 ---

		// スライムのカメラジャンプアニメを毎フレーム更新
		player_->UpdateCameraJump();

		// タイトル文字は動かさない（最後に更新した位置で固定）
		math->worldTransFormUpdate(titleTextWorldTransform_);

		// フェードが完了したらシーン終了
		if (fade_->isFinished()) {
			finished_ = true;
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
	skydome_->Draw();
	titleTextModel_->Draw(titleTextWorldTransform_, camera_,&color_);
	Model::PostDraw();
	Sprite::PreDraw(commandList);
	// スプライトの描画
	startSprite_->Draw(); // スタートボタン用スプライトの描画
	enterSprite_->Draw(); // エンターキー用スプライトの描画
	Sprite::PostDraw();
	// Fadeの描画
	fade_->Draw(commandList); // commandList を引数に渡す
}

TitleScene::~TitleScene() {
	//Audio::GetInstance()->StopWave(bgmVoiceHandle_);
	//Audio::GetInstance()->StopWave(enterVoice);
	delete player_;
	delete playerModel_;
	delete titleTextModel_;
	delete fade_;
	
}