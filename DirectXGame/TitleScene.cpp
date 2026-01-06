#include "TitleScene.h"
#include "Fade.h"
using namespace KamataEngine;
void TitleScene::Initialize() {
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	camera_.Initialize();
	playerModel_ = Model::CreateFromOBJ("player", true);
	textureHandleStart_ = TextureManager::Load("gameStartText.png"); // スタートボタン用テクスチャの読み込み
	textureHandleEnter_ = TextureManager::Load("spaceText.png"); // エンターキー用テクスチャの読み込み
	textureHandleArrow_ = TextureManager::Load("yazirusi.png");
	textureHandleTutorial_ = TextureManager::Load("tutorialText.png");


	camera_.translation_.z = -10.0f; // カメラのZ位置を調整

	// player_ メンバ変数を new でインスタンス化
	player_ = new Player();
	player_->Initialize(playerModel_, &camera_, Vector3{0, -2.0f, -3.0f});
	player_->GetWorldTransform().rotation_.y = std::numbers::pi_v<float>; // 180度回転 (πラジアン)


	// 文字モデルのロードと初期化
	titleTextModel_ = Model::CreateFromOBJ("title", true); 
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

	startSprite_ = Sprite::Create(textureHandleStart_, {450.0f, 500.0f}); // スタートボタン用スプライトの作成
	spaceSprite_ = Sprite::Create(textureHandleEnter_, {600.0f, 450.0f}); // エンターキー用スプライトの作成
	tutorialSprite_ = Sprite::Create(textureHandleTutorial_, {450.0f, 600.0f});
	arrowSprite_ = Sprite::Create(textureHandleArrow_, {350.0f, 500.0f});

	isMusic = false;

		// タイマー初期化
	blinkTimer_ = 0.0f;

	currentSelection_ = 0;
	nextScene_ = NextScene::kGame;
}

void TitleScene::Update() {
	// 常に更新する処理
	fade_->Update();
	skydome_->Update();
	camera_.UpdateMatrix();
	camera_.TransferMatrix();

	// BGM関連（元のコードにあったコメントアウト部分）
	// if (!isMusic) {
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

	case Phase::kMain: {
		// --- メイン（入力待機）中 ---

		// 文字の上下揺れ
		floatingTimer_ += floatingSpeed_;
		titleTextWorldTransform_.translation_.y = baseTextPos_.y + std::sin(floatingTimer_) * floatingAmplitudeY_;
		math->worldTransFormUpdate(titleTextWorldTransform_);

		// スライムのアイドリングアニメを更新
		player_->UpdateTitleAnimation();

		// ---------------------------------------------------------
		// ★ 選択操作 (W/S または 矢印キー上下)
		// ---------------------------------------------------------
		if (Input::GetInstance()->TriggerKey(DIK_W) || Input::GetInstance()->TriggerKey(DIK_UP)) {
			currentSelection_--;
		}
		if (Input::GetInstance()->TriggerKey(DIK_S) || Input::GetInstance()->TriggerKey(DIK_DOWN)) {
			currentSelection_++;
		}

		// 選択範囲のループ処理 (0: Start, 1: Tutorial)
		if (currentSelection_ < 0)
			currentSelection_ = 1;
		if (currentSelection_ > 1)
			currentSelection_ = 0;

		// ---------------------------------------------------------
		// 矢印の位置更新と点滅処理
		// ---------------------------------------------------------

		blinkTimer_ += 0.05f;                          // 点滅スピード
		float alpha = std::abs(std::sin(blinkTimer_)); // 0.0 ～ 1.0

		if (currentSelection_ == 0) {
			// --- 「START」選択中 ---

			// 矢印をStartの横に移動 
			if (arrowSprite_)
				arrowSprite_->SetPosition({350.0f, 510.0f});

			// Startを点滅
			if (startSprite_)
				startSprite_->SetColor({1.0f, 1.0f, 1.0f, alpha});
			// Tutorialは通常表示
			if (tutorialSprite_)
				tutorialSprite_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
		} else {
			// --- 「TUTORIAL」選択中 ---

			// 矢印をTutorialの横に移動
			if (arrowSprite_)
				arrowSprite_->SetPosition({350.0f, 610.0f});

			// Tutorialを点滅
			if (tutorialSprite_)
				tutorialSprite_->SetColor({1.0f, 1.0f, 1.0f, alpha});
			// Startは通常表示
			if (startSprite_)
				startSprite_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
		}

		// ---------------------------------------------------------
		// 決定処理 (スペースキー)
		// ---------------------------------------------------------
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			phase_ = Phase::kfadeOut;

			// 決定時は点滅をやめて不透明に戻す
			if (startSprite_)
				startSprite_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
			if (tutorialSprite_)
				tutorialSprite_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});

			// 選択に応じて次のシーンをセット
			if (currentSelection_ == 0) {
				nextScene_ = NextScene::kGame; // 通常ゲームへ
			} else {
				nextScene_ = NextScene::kTutorial; // チュートリアルへ
			}

			// スライムにカメラジャンプ開始を命令
			player_->StartCameraJump();
			// ここで決定SE再生などを入れるとGood

			fade_->Start(Fade::Status::FadeOut, 1.8f);
		}
	} break;

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
	spaceSprite_->Draw(); // エンターキー用スプライトの描画
	if (tutorialSprite_)
		tutorialSprite_->Draw();
	if (arrowSprite_)
		arrowSprite_->Draw();
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
	delete tutorialSprite_; 
	delete arrowSprite_;
	
}