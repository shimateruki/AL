#include "GameOver.h"

void GameOver::Initialize() 
{
	textureHandle_ = TextureManager::Load("gameOver.png");
	playerModel = Model::CreateFromOBJ("GameOverPlayer", true);
	backgroundModel_ = Model::CreateFromOBJ("background", true);
	camera_.Initialize();
	camera_.translation_.z = -10.0f; // カメラのZ位置を調整
	player_ = new Player();
	player_->Initialize(playerModel, &camera_, Vector3{0, -5.5f, 7.0f});
	background_.Initialize();
	background_.translation_ = {0, 0.0f, 30};
	bacgroundColor.Initialize();
	bacgroundColor.SetColor({0.0f, 0.0f, 0.0f, 1.0f});
	fade_ = new Fade();
	fade_->Initalize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);
	gameOverSprite_ = Sprite::Create(textureHandle_, {0.0f, 0.0f});
	TextureHandleYazirusi_ = TextureManager::Load("yazirusi.png");
	yazirusiSprite = Sprite::Create(TextureHandleYazirusi_, {180, 190});
}
void GameOver::Update() 
{
	fade_->Update();
	// カメラの更新と転送
	camera_.UpdateMatrix();
	camera_.TransferMatrix();
	
	if (currentSelect_ == PauseSelect::kContinue) {
		yazirusiSprite->SetPosition({180, 280});
	} else if (currentSelect_ == PauseSelect::kStageSelect) {
		yazirusiSprite->SetPosition({190, 380});
	} else if (currentSelect_ == PauseSelect::kTitle) {
		yazirusiSprite->SetPosition({190, 480});
	}

	// ポーズ中の処理
	
		// Wキーで上に移動
		if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_W)) {
			if (currentSelect_ == PauseSelect::kContinue) {
				currentSelect_ = PauseSelect::kTitle;
			} else if (currentSelect_ == PauseSelect::kStageSelect) {
				currentSelect_ = PauseSelect::kContinue;
			} else if (currentSelect_ == PauseSelect::kTitle) {
				currentSelect_ = PauseSelect::kStageSelect;
			}
		}
		// Sキーで下に移動
		if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_S)) {
			if (currentSelect_ == PauseSelect::kContinue) {
				currentSelect_ = PauseSelect::kStageSelect;
			} else if (currentSelect_ == PauseSelect::kStageSelect) {
				currentSelect_ = PauseSelect::kTitle;
			} else if (currentSelect_ == PauseSelect::kTitle) {
				currentSelect_ = PauseSelect::kContinue;
			}
		}
	    // 決定（Enterキー）
	    if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_RETURN)) {
		    if (currentSelect_ == PauseSelect::kContinue) {
			    nextScene_ = NextScene::kRetry; // リトライを選択
			    finished_ = true;
		    } else if (currentSelect_ == PauseSelect::kStageSelect) {
			    nextScene_ = NextScene::kStageSelect; // ステージセレクトを選択
			    finished_ = true;
		    } else if (currentSelect_ == PauseSelect::kTitle) {
			    nextScene_ = NextScene::kTitle; // タイトルを選択
			    finished_ = true;
		    }
	    }
	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_RETURN)) {
		isTimer = true; // エンターキーが押されたらタイマー終了フラグを立てる
		fade_->Start(Fade::Status::FadeOut, 3.0f); // フェードアウト開始
	}
	if (isTimer) {
		displayTimer_++;
	}
	if (displayTimer_>=180) {
		finished_ = true;
	}
	math->worldTransFormUpdate(player_->GetWorldTransform());
	math->worldTransFormUpdate(background_);
}

void GameOver::Draw() 
{
	
	 DirectXCommon* dxcommon = DirectXCommon::GetInstance();
	ID3D12GraphicsCommandList* commandList = dxcommon->GetCommandList();



	// --- モデル描画 ---
	Model::PreDraw(commandList);
	backgroundModel_->Draw(background_, camera_, &bacgroundColor);
	player_->Draw();

	Model::PostDraw();


		  // --- 背景スプライト描画 ---
	Sprite::PreDraw(commandList);
	gameOverSprite_->Draw(); // ← 先に背景として描画
	Sprite::PostDraw();

	// --- UIスプライト描画 ---
	Sprite::PreDraw(commandList);
	yazirusiSprite->Draw();
	fade_->Draw(commandList);
	Sprite::PostDraw();


}

GameOver::~GameOver() {}
