#include "GameScene1_2.h"
#include "GameStateManager.h"
#include <iostream>
#include <algorithm> // std::remove_if に必要
using namespace KamataEngine; // KamataEngine名前空間を使用

// GameSceneの初期化処理
void GameScene1_2::Initialize() {
	//========================
	// 📦 リソースの読み込み
	//========================
	textureHandel_ = TextureManager::Load("sample.png");
	dirtModel_ = Model::CreateFromOBJ("tutiBlock", true);
	grassModel_ = Model::CreateFromOBJ("tutiBlockSibahu", true);
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	playerModel_ = Model::CreateFromOBJ("player", true);
	deatparticlesModel_ = Model::CreateFromOBJ("deathParticle", true);
	hitEffectModel_ = Model::CreateFromOBJ("hit", true);
	goalModel_ = Model::CreateFromOBJ("gorl", true);
	GameClearTextModel_ = Model::CreateFromOBJ("GameClear", true);
	togeKabeModel_ = Model::CreateFromOBJ("kabeToge", true);
	togeModel_ = Model::CreateFromOBJ("toge", true);
	textureHandle = TextureManager::Load("1-2.png");
	textureHandlePauseText_ = TextureManager::Load("phoseText.png"); // ポーズテキスト用テクスチャの読み込み
	yamaModel = Model::CreateFromOBJ("yama", true);                  // 山モデルの読み込み
	textureHandleGameClearText_ = TextureManager::Load("TextSpriteGameClear.png"); // ゲームクリアテキスト用テクスチャの読み込み
	textureHandleCountdown3_ = TextureManager::Load("3.png");                      // 3の画像
	textureHandleCountdown2_ = TextureManager::Load("2.png");                      // 2の画像
	textureHandleCountdown1_ = TextureManager::Load("1.png");                      // 1の画像
	textureHandleCountdownGo_ = TextureManager::Load("go.png");                    // GOの画像
	enemy_model_ = Model::CreateFromOBJ("enemy", true);                            // 敵モデルの読み込み



	textureHandlePhose_ = TextureManager::Load("Phose.png");
	TextureHandleYazirusi_ = TextureManager::Load("yazirusi.png");
	textureHandleEnter_ = TextureManager::Load("enter.png");  // エンターキー用テクスチャの読み込み
	CloudPlatformModel_ = Model::CreateFromOBJ("kumo", true); // 雲プラットフォームのモデル読み込み
	breakableBlockModel_ = Model::CreateFromOBJ("Breakable", true); // 破壊可能ブロックのモデル読み込み
	treeModel_ = Model::CreateFromOBJ("tree", true);                // 木モデルの読み込み

	//========================
	// 🎥 カメラの設定
	//========================
	camera_.farZ = 1280.0f;
	camera_.Initialize();
	debaucamera_ = new DebugCamera(100, 50);
	debaucamera_->SetFarZ(1280.0f);

	//========================
	// 🗺️ マップ読み込み
	//========================
	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv("Resources/1-2.csv");

	//========================
	// 🧍 プレイヤーの初期化
	//========================
	player_ = new Player();
	Vector3 playerPosition = mapChipField_->GetChipPositionIndex(3, 16);
	player_->Initialize(playerModel_, &camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);
	player_->SetisMove(false);

	//========================
	// 💥 ヒットエフェクト設定
	//========================
	HitEffect::SetModel(hitEffectModel_);
	HitEffect::SetCamera(&camera_);

	//========================
	// 👾 敵の初期化
	//========================
	enemys_.push_back(new Enemy());
	enemys_.back()->Initialize(enemy_model_, &camera_, mapChipField_->GetChipPositionIndex(15, 14));
	enemys_.back()->SetMapChipField(mapChipField_);
	enemys_.back()->SetGameScene1_2(this);
	enemys_.push_back(new Enemy());
	enemys_.back()->Initialize(enemy_model_, &camera_, mapChipField_->GetChipPositionIndex(66, 10));
	enemys_.back()->SetMapChipField(mapChipField_);
	enemys_.back()->SetGameScene1_2(this);
	enemys_.push_back(new Enemy());
	enemys_.back()->Initialize(enemy_model_, &camera_, mapChipField_->GetChipPositionIndex(70, 10));
	enemys_.back()->SetMapChipField(mapChipField_);
	enemys_.back()->SetGameScene1_2(this);

	//========================
	// 🌪️ パーティクルの生成
	//========================
	deatparticles_ = new DeathParticles();
	deatparticles_->Initialize(deatparticlesModel_, &camera_, player_, playerPosition);

	//========================
	// 🌌 スカイドーム初期化
	//========================
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);

	//========================
	// 🎮 カメラコントローラー
	//========================
	CController_ = new CameraController();
	CController_->Initialize(&camera_);
	CController_->SetTarget(player_);
	CController_->Reset();
	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	CController_->SetMovableSrea(cameraArea);

	//========================
	// 🧱 ブロック生成
	//========================
	GenerrateBlock();

	//========================
	// 🌗 フェード処理の初期化
	//========================
	fade_ = new Fade();
	fade_->Initalize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);
	finishedTimer = 0;

	//========================
	// 🎉 ゲームクリアテキスト
	//========================
	GameClearTextWorldTransform_.Initialize();
	GameClearTextWorldTransform_.scale_ = {5.0f, 1.0f, 1.0f};
	GameClearTextWorldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	GameClearTextWorldTransform_.translation_ = mapChipField_->GetChipPositionIndex(20, 18);

	//========================
	//  　棘の壁の初期化
	//========================
	togeKabe_ = new KabeToge();
	togeKabe_->Initialize(togeKabeModel_, &camera_, mapChipField_->GetChipPositionIndex(0, 18));
	togeKabe_->SetIsmove(false);

	CloudPlatform_.push_back(new CloudPlatform());
	CloudPlatform_.back()->Initialize(CloudPlatformModel_, &camera_, mapChipField_->GetChipPositionIndex(15, 16));
	CloudPlatform_.back()->SetMovement({20.0f, 2.0f, 0.0f}, {25.0f, 2.0f, 0.0f}, 0.01f);

	yama_.push_back(new Yama());
	yama_.back()->Initialize(yamaModel, &camera_, mapChipField_->GetChipPositionIndex(20, 19));

	yama_.push_back(new Yama());
	yama_.back()->Initialize(yamaModel, &camera_, mapChipField_->GetChipPositionIndex(70, 19));

	tree_.push_back(new Tree());
	tree_.back()->Initialize(treeModel_, &camera_, mapChipField_->GetChipPositionIndex(5, 19));
	tree_.push_back(new Tree());
	tree_.back()->Initialize(treeModel_, &camera_, mapChipField_->GetChipPositionIndex(90, 19));

	spriteCountdown_ = Sprite::Create(textureHandleCountdown3_, {0, 0}); // 初期スプライト（3）

	Audio::GetInstance()->Initialize("Resources/BGM/");

	// BGMの読み込み（.wavファイル）
	clearbgmHandle_ = KamataEngine::Audio::GetInstance()->LoadWave("Clear1.wav");

	Textmodel1_2 = Sprite::Create(textureHandle, {100.50});
	poseSprite = Sprite::Create(textureHandlePhose_, {0.0});
	yazirusiSprite = Sprite::Create(TextureHandleYazirusi_, {180, 190});
	enterSprite_ = Sprite::Create(textureHandleEnter_, {0.0f, 0.0f}); // エンターキー用スプライトの作成
	pauseTextSprite_ = Sprite::Create(textureHandlePauseText_, {0.0f, 0.0f}); // ポーズメニュー用スプライトの作成
	GameClearTextSprite_ = Sprite::Create(textureHandleGameClearText_, {0.0f, 0.0f}); // ゲームクリアテキスト用スプライトの作成


		// ゲームクリアテキストスプライトの作成
	GameClearTextSprite_ = Sprite::Create(textureHandleGameClearText_, {0.0f, 0.0f}); // ゲームクリアテキスト用スプライトの作成
	spriteCountdown_ = Sprite::Create(textureHandleCountdown3_, {0, 0});              // 初期スプライト（3）
	// HPアイコンのテクスチャを読み込む
	textureHandleHpIconNormal_ = TextureManager::Load("playerAikon.png");
	textureHandleHpIconDamage_ = TextureManager::Load("playerAikonDamage.png");

	// スプライトを生成
	Vector2 hpIconPos = {20.0f, 650.0f}; // 表示したい座標
	spriteHpIconNormal_ = Sprite::Create(textureHandleHpIconNormal_, hpIconPos);
	spriteHpIconDamage_ = Sprite::Create(textureHandleHpIconDamage_, hpIconPos);

	textureHandleHeart_ = TextureManager::Load("playerHp.png");

	Vector2 heartBasePos = {100.0f, 650.0f}; // 1個目のハートの座標
	float heartSpacing = 64.0f;              // ハートとハートの間隔（画像の横幅+隙間）

	// HPの最大値(3回)ループして、ハートのスプライトを生成
	for (int i = 0; i < kMaxPlayerHp; i++) {
		// 表示座標を計算 (例: 20.0, 70.0, 120.0)
		Vector2 pos = {heartBasePos.x + (i * heartSpacing), heartBasePos.y};

		// 満タンハートのスプライトをvectorに追加
		spriteHearts_.push_back(Sprite::Create(textureHandleHeart_, pos));
	}

		Audio::GetInstance()->Initialize("Resources/BGM/");

	// BGMの読み込み
	clearbgmHandle_ = KamataEngine::Audio::GetInstance()->LoadWave("Clear1.wav");
	bgmHandle = KamataEngine::Audio::GetInstance()->LoadWave("gameBGM1.wav");

	// Se読み込み
	Audio::GetInstance()->Initialize("Resources/SE/");
	consorlSelectHandle = KamataEngine::Audio::GetInstance()->LoadWave("cursorMove.wav");

	GameStateManager::GetInstance()->SetCurrentStageID(2); // ステージ2
	GameClearTextSprite_ = Sprite::Create(textureHandleGameClearText_, {0.0f, 0.0f}); // ゲームクリアテキスト用スプライトの作成
	isSprite = true; // スプライト表示
	firstFrame = true;
}

// ==============================
// 更新処理
// ==============================
void GameScene1_2::Update() {

	    // 前フレームのプレイヤー位置を保存
	playerPreviousPosition_ = player_->GetWorldPosition();
	// 足場フラグリセット
	isOnPlatform_ = false;

	// フェードの更新 & フェーズ管理
	fade_->Update();
	LimitPlayerPosition();
	ChangePhase();
	for (Yama* yama : yama_) {
		yama->Update();
	}
	for (Tree* tree : tree_) {
		tree->Update();
	}
	// ポーズ状態の切り替え
	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_P) && !player_->IsDead()&&!isGameClear_) {
	/*	consorlVoiceSelectHandle = KamataEngine::Audio::GetInstance()->PlayWave(consorlSelectHandle, 0.5f, false);*/
		isPaused_ = !isPaused_;
	}

	if (currentSelect_ == PauseSelect::kContinue) {
		yazirusiSprite->SetPosition({180, 190});
	} else if (currentSelect_ == PauseSelect::kStageSelect) {
		yazirusiSprite->SetPosition({190, 320});
	} else if (currentSelect_ == PauseSelect::kTitle) {
		yazirusiSprite->SetPosition({190, 460});
	}

	// ポーズ中の処理
	if (isPaused_) {
		// Wキーで上に移動
		if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_W)) {
			consorlVoiceSelectHandle = KamataEngine::Audio::GetInstance()->PlayWave(consorlSelectHandle, false, 0.5f);
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
			consorlVoiceSelectHandle = KamataEngine::Audio::GetInstance()->PlayWave(consorlSelectHandle, false, 0.5f);
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
				isPaused_ = false; // ポーズを解除
			} else if (currentSelect_ == PauseSelect::kStageSelect) {
				// ★ 修正: ステージセレクト画面の1-1看板のマップチップ座標を指定
				Vector3 signboardPosition = mapChipField_->GetChipPositionIndex(20, 17);

				// 次のプレイヤー初期位置をGameStateManagerに保存
				GameStateManager::GetInstance()->SetPlayerStartPosition(signboardPosition);
				finished_ = true; // シーン遷移
			} else if (currentSelect_ == PauseSelect::kTitle) {
				currentSelectIndex_ = 1;
				finished_ = true; // シーン遷移
			}
		}

		// ポーズ中のゲーム更新はスキップ
		return;
	}
	
		for (auto& floor : breakableFloors_) {
		floor->Update();
	}

	

	// ==============================
	// フェーズごとの処理
	// ==============================
	switch (phase_) {

	// ------------------------------
	// フェードイン中の処理
	// ------------------------------
	case Phase::kFadeIn:
		player_->Update(); // プレイヤーの更新
		// 敵の更新
		for (Enemy* enemy : enemys_) {
			enemy->Update();
		}
		togeKabe_->Update();	


		// カメラコントローラーの更新
		CController_->Update();

		// ブロックの行列更新 & 転送
		for (std::vector<WorldTransform*>& blockLine : worldTransformBlocks_) {
			for (WorldTransform* block : blockLine) {
				if (!block)
					continue;
				block->matWorld_ = math->MakeAffineMatrix(block->scale_, block->rotation_, block->translation_);
				block->TransferMatrix();
			}
		}

		// デバッグカメラ更新
		debaucamera_->Update();

#ifdef _DEBUG
		// デバッグカメラ切り替え
		if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_M)) {
			isDebugCameraActive_ = !isDebugCameraActive_;
		}
#endif

		// カメラの行列更新
		if (isDebugCameraActive_) {
			camera_.matView = debaucamera_->GetCamera().matView;
			camera_.matProjection = debaucamera_->GetCamera().matProjection;
			camera_.TransferMatrix();
		} else {
			camera_.UpdateMatrix();
		}

		// スカイドーム更新
		skydome_->Update();
		break;

			case Phase::kCountdown:
		// このフェーズでカウントダウンの更新処理を行う
		countdownTimer_ += 1.0f / 60.0f; // タイマーを減らす

		// カウントダウンのスプライト切り替え
		if (countdownTimer_ >= 0.0f && countdownTimer_ < 1.0f) {

			spriteCountdown_ = Sprite::Create(textureHandleCountdown3_, {-250, -100}); // 初期スプライト（3）
		} else if (countdownTimer_ >= 1.0f && countdownTimer_ < 2.0f) {
			spriteCountdown_ = Sprite::Create(textureHandleCountdown2_, {-250, -100}); // 初期スプライト（3）
		} else if (countdownTimer_ >= 2.0f && countdownTimer_ < 3.0f) {
			spriteCountdown_ = Sprite::Create(textureHandleCountdown1_, {-250, -100}); // 初期スプライト（3）
		} else {
			spriteCountdown_ = Sprite::Create(textureHandleCountdownGo_, {-250, -100}); // 初期スプライト（3）
			if (countdownTimer_ >= 3.5f) {                                              // GO!表示後
				countdownState_ = CountdownState::kFinished;
				phase_ = Phase::kPlay; // プレイフェーズへ移行
				player_->SetisMove(true); // プレイヤーの移動を許可
				togeKabe_->SetIsmove(true);
				                       // カウントダウン完了
			}
		}
		// カウントダウン中はゲームの進行を停止するため、ここではプレイヤー等の更新処理は書かない
		break;

	// ------------------------------
	// プレイ中の処理
	// ------------------------------
	case Phase::kPlay: {

		player_->Update();
		togeKabe_->Update();
		for (Enemy* enemy : enemys_) {
			enemy->Update();
		}

        player_->CheckAndResolveTogeKabeCollision(togeKabe_);
		CheekAllcollision();
		CController_->Update();

		// ブロックの行列更新 & 転送
		for (std::vector<WorldTransform*>& blockLine : worldTransformBlocks_) {
			for (WorldTransform* block : blockLine) {
				if (!block)
					continue;
				block->matWorld_ = math->MakeAffineMatrix(block->scale_, block->rotation_, block->translation_);
				block->TransferMatrix();
			}
		}

		// Player が壊れる床を踏んだかチェック
	
	if (auto stepped = player_->CheckSteppedBreakable(mapChipField_)) {
			for (auto& floor : breakableFloors_) {
			if (floor->GetIndexX() == stepped->xIndex && floor->GetIndexY() == stepped->yIndex) {
					floor->OnStepped(); // ← 踏んだその1枚だけ開始
					break;              // 見つかったら終わり
				}
			}
		}
		

	

		debaucamera_->Update();

#ifdef _DEBUG
		if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_M)) {
			isDebugCameraActive_ = !isDebugCameraActive_;
		}
#endif

		if (isDebugCameraActive_) {
			camera_.matView = debaucamera_->GetCamera().matView;
			camera_.matProjection = debaucamera_->GetCamera().matProjection;
			camera_.TransferMatrix();
		} else {
			camera_.UpdateMatrix();
		}

		skydome_->Update();
	} break;

	// ------------------------------
	// 死亡中の処理（プレイヤー死亡）
	// ------------------------------
	case Phase::kDeath: {

		for (Enemy* enemy : enemys_) {
			enemy->Update();
		}

		// ブロックの行列更新 & 転送
		for (std::vector<WorldTransform*>& blockLine : worldTransformBlocks_) {
			for (WorldTransform* block : blockLine) {
				if (!block)
					continue;
				block->matWorld_ = math->MakeAffineMatrix(block->scale_, block->rotation_, block->translation_);
				block->TransferMatrix();
			}
		}

		debaucamera_->Update();

#ifdef _DEBUG
		if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_M)) {
			isDebugCameraActive_ = !isDebugCameraActive_;
		}
#endif

		if (isDebugCameraActive_) {
			camera_.matView = debaucamera_->GetCamera().matView;
			camera_.matProjection = debaucamera_->GetCamera().matProjection;
			camera_.TransferMatrix();
		} else {
			camera_.UpdateMatrix();
		}

		skydome_->Update();

		for (auto it = breakableFloors_.begin(); it != breakableFloors_.end();) {
			(*it)->Update();
			if ((*it)->IsBroken()) {
				it = breakableFloors_.erase(it); // 消滅したらリストから消す
			} else {
				++it;
			}
		}

	} break;

	// ------------------------------
	// ゲームクリア処理
	// ------------------------------
	case Phase::GameClear: {

		math->worldTransFormUpdate(GameClearTextWorldTransform_);
	} break;
	}

	// ==============================
	// 共通処理
	// ==============================

	// ヒットエフェクト更新
	for (HitEffect* hitEffect : hitEffects_) {
		hitEffect->Update();
	}

	// 死亡したヒットエフェクトの削除
	hitEffects_.remove_if([](HitEffect* effect) {
		if (effect->IsDead()) {
			delete effect;
			return true;
		}
		return false;
	});


}
void GameScene1_2::Draw() {
	DirectXCommon* dxcommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxcommon->GetCommandList());

	// 👾 敵の描画
	for (Enemy* enemy : enemys_) {
		if (enemy->isDead())
			continue; // 死んでる敵はスキップ
		enemy->Draw();
	}

	// 🧱 ブロック描画（草・土でモデル切替）
	for (uint32_t y = 0; y < worldTransformBlocks_.size(); ++y) {
		for (uint32_t x = 0; x < worldTransformBlocks_[y].size(); ++x) {
			WorldTransform* block = worldTransformBlocks_[y][x];
			if (block) {
				// タイプを取得
				MapChipType type = mapChipField_->GetMapChipTypeByindex(x, y);

				// デバッグ用出力
				if (type == MapChipType::kGrass_) {
					std::cout << "Grass at (" << x << ", " << y << ") is being processed.\n";
				}

				// 種類ごとに描画モデルを分岐
				switch (type) {
				case MapChipType::kDirt_:
					dirtModel_->Draw(*block, camera_);
					break;
				case MapChipType::kGrass_:
					grassModel_->Draw(*block, camera_);
					break;
				case MapChipType::kGoal_:
					goalModel_->Draw(*block, camera_);
					break;
				case MapChipType::kSpike_:
					// トゲの壁は特別な処理を行う
					togeModel_->Draw(*block, camera_);
					break;
				}
			}
		}
	}



	for (auto& floor : breakableFloors_) {
		floor->Draw(breakableBlockModel_, &camera_); // 破壊可能な床の描画
	}

	togeKabe_->Draw(); // トゲ壁の描画

	// 🌌 スカイドーム描画
	skydome_->Draw();

	// 🧍 プレイヤー描画
	if (!player_->IsDead())
		player_->Draw();

	// 💥 ヒットエフェクト描画
	for (HitEffect* hitEffect : hitEffects_)
		hitEffect->Draw();
	for (Yama* yama : yama_) {
		yama->Draw();
	}
	for (Tree* tree : tree_) {
		tree->Draw();
	}
	// 🌪️ パーティクル描画
	deatparticles_->Draw();

	// 🎉 ゲームクリア表示
	if (isGameClear_) {
		Model::PreDraw(dxcommon->GetCommandList());
		GameClearTextModel_->Draw(GameClearTextWorldTransform_, camera_);
		Model::PostDraw();
	}

	// 🌗 フェード描画
	fade_->Draw(dxcommon->GetCommandList());
	Model::PostDraw();
	Sprite::PreDraw(dxcommon->GetCommandList());
	if (countdownState_ == CountdownState::kCounting) {
		spriteCountdown_->Draw();
	}
	if (isSprite&&!isGameClear_&&!isPaused_) {
		Textmodel1_2->Draw();
		pauseTextSprite_->Draw(); // ポーズスプライトの描画
	}
	if (isPaused_) {
		poseSprite->Draw();
		yazirusiSprite->Draw();
		enterSprite_->Draw(); // エンターキー用スプライトの描画
	}
	if (isGameClear_ && isSprite) {
		GameClearTextSprite_->Draw();
		enterSprite_->Draw(); // エンターキー用スプライトの描画
	}
	if (!player_->IsDead()) {
		// もしプレイヤーが「無敵時間中」なら
		if (player_->GetIsInvincible()) {
			spriteHpIconDamage_->Draw(); // ダメージ時のアイコン
		} else {
			spriteHpIconNormal_->Draw(); // 通常時のアイコン
		}
	}

	if (!player_->IsDead()) {
		// プレイヤーの現在のHPを取得
		int currentHp = player_->GetHp();
		for (int i = 0; i < currentHp; i++) {

			// ハートを描画する
			spriteHearts_[i]->Draw();
		}
	}

	Sprite::PostDraw();
}

void GameScene1_2::GenerrateBlock() {
	const uint32_t kNumBlockVirtal = mapChipField_->GetNumBlockVirtcal();
	const uint32_t kNumBlockHorizontal = mapChipField_->GetNumBlockHorizonal();

	worldTransformBlocks_.resize(kNumBlockVirtal);
	for (uint32_t i = 0; i < kNumBlockVirtal; ++i) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal, nullptr); // nullptrで初期化
	}

	for (uint32_t y = 0; y < kNumBlockVirtal; ++y) {
		for (uint32_t x = 0; x < kNumBlockHorizontal; ++x) {
			MapChipType type = mapChipField_->GetMapChipTypeByindex(x, y);

			if (type == MapChipType::kBlank_) {
				continue; // 何もない
			}

			if (type == MapChipType::kBreakable_) {
				// ★ BreakableFloor は専用クラスで扱う
				auto floor = std::make_unique<BreakableFloor>();
				floor->Initialize(mapChipField_->GetChipPositionIndex(x, y), x, y, mapChipField_);
				breakableFloors_.push_back(std::move(floor));

				worldTransformBlocks_[y][x] = nullptr; // ←必須じゃないけど明示
			} else {
				// 通常ブロックはここで生成
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransform->translation_ = mapChipField_->GetChipPositionIndex(x, y);
				worldTransformBlocks_[y][x] = worldTransform;
			}
		}
	}
}

void GameScene1_2::CheekAllcollision() {

	// プレイヤーAABB
	AABB aabb1 = player_->GetAABB();

for (Enemy* enemy : enemys_) {
		// 死亡しているか、当たり判定が無効化されている敵はスキップ
		if (enemy->isDead() || enemy->isCollisonDisabled()) {
			continue;
		}

		AABB aabb2 = enemy->GetAABB();
		if (math->IsCollision(aabb1, aabb2)) {
			// ここでは、踏んだかどうかの判定のみに集中する
			if (aabb1.min.y >= aabb2.max.y - 4.0f && !player_->GetOnGround()) {
				enemy->OnStomped(player_);
				player_->SetVelocityY(0.3f);
			} else {
				player_->TakeDamage(1);
			}
		}
	}

	// ==== マップチップとの当たり判定 ====
	const uint32_t kNumBlockVertical = mapChipField_->GetNumBlockVirtcal();
	const uint32_t kNumBlockHorizontal = mapChipField_->GetNumBlockHorizonal();

	for (uint32_t y = 0; y < kNumBlockVertical; ++y) {
		for (uint32_t x = 0; x < kNumBlockHorizontal; ++x) {
			MapChipType type = mapChipField_->GetMapChipTypeByindex(x, y);

			// ゴール or 棘 以外は無視
			if (type != MapChipType::kGoal_ && type != MapChipType::kSpike_) {
				continue;
			}

			// マップチップの矩形AABBを作成
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(x, y);
			AABB chipAABB;
			chipAABB.min = {rect.left, rect.bottom, 0.0f};
			chipAABB.max = {rect.right, rect.top, 0.0f};

			// 当たり判定
			if (math->IsCollision(aabb1, chipAABB)) {
				if (type == MapChipType::kGoal_) {
					// ゴール処理
					bgmVoiceHandle_ = KamataEngine::Audio::GetInstance()->PlayWave(clearbgmHandle_, false, 0.5f);
					player_->StartVictoryPose(); // 勝利ポーズ開始
					CController_->StartVictoryZoom(player_);
					isGameClear_ = true;
				} else if (type == MapChipType::kSpike_) {
					// 棘のダメージ処理
					player_->TakeDamage(1);
				}
			}
		}
	}

	  




}

void GameScene1_2::ChangePhase() {
	switch (phase_) {
	case Phase::kFadeIn:
		if (fade_->isFinished()) {
			countdownState_ = CountdownState::kCounting;
			phase_ = Phase::kCountdown; // フェードインが完了したらカウントダウンフェーズへ
		}

		break;
	case Phase::kPlay:
		if (player_->IsDead()) {
			// 死亡演出フェーズに切り替え
			phase_ = Phase::kDeath;
			isSprite = false; // スプライト非表示
			const Vector3& deatParticlesPosition = player_->GetWorldPosition();
			deatparticles_->Initialize(deatparticlesModel_, &camera_, player_, deatParticlesPosition);

			fade_->Start(Fade::Status::FadeOut, 3.0f); // フェードアウト開始
			nextScene_ = NextScene::kGameOver;
		}
		if (isGameClear_) {
			phase_ = Phase::GameClear; // ゲームクリアフェーズに切り替え
		}

		break;
	case Phase::kDeath:
		finishedTimer++;
		deatparticles_->Update();
		// ★ 修正: ステージセレクト画面の1-1看板のマップチップ座標を指定
		Vector3 signboardPosition = mapChipField_->GetChipPositionIndex(20, 17);

		// 次のプレイヤー初期位置をGameStateManagerに保存
		GameStateManager::GetInstance()->SetPlayerStartPosition(signboardPosition);
		if (finishedTimer >= 180) {
			finished_ = true;
		}
		break;
	case Phase::GameClear:

		// GameStateManagerの唯一のインスタンスを取得して、フラグを設定
		GameStateManager::GetInstance()->SetStageClear(2, true);


		// ステージセレクト画面の1-1看板のマップチップ座標を指定
		signboardPosition = mapChipField_->GetChipPositionIndex(20, 17);

		// 次のプレイヤー初期位置をGameStateManagerに保存
		GameStateManager::GetInstance()->SetPlayerStartPosition(signboardPosition);
		player_->UpdateVictoryAnimation(); // 勝利ポーズのアニメを更新
		CController_->Update();
		if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
			isTimerFinished_ = true;                   // スペースキーが押されたらタイマー終了フラグを立てる
			fade_->Start(Fade::Status::FadeOut, 3.0f); // フェードアウト開始
			isSprite = false;                          // スプライト非表示
		}
		if (isTimerFinished_) {
			finishedTimer++;
		}
		if (finishedTimer >= 180) {
			finished_ = true;
		}
		break;
	}
}

void GameScene1_2::CreateHitEffect(const KamataEngine::Vector3& position) {
	position;
	//HitEffect* newHitEffect = HitEffect::create(position); // 新しいヒットエフェクトを生成
	//hitEffects_.push_back(newHitEffect);                   // ヒットエフェクトをリストに追加)
	//                                                       // ヒットエフェクトの数が最大数を超えた場合、最も古いものを削除
}

void GameScene1_2::LimitPlayerPosition() {
	if (firstFrame) {
		firstFrame = false;
		return;
	}

	Vector3 pos = player_->GetWorldPosition();

	// プレイヤーのサイズを取得（半分の幅）
	float halfWidth = player_->GetWidth() / 2.0f;

	// 制限用の基準値（壁の位置）
	const float leftLimit = 1.4f;
	const float rightLimit = 98.0f;

	// X方向の制限（めり込み防止）
	if (pos.x < leftLimit + halfWidth) {
		pos.x = leftLimit + halfWidth;
	}
	if (pos.x > rightLimit - halfWidth) {
		pos.x = rightLimit - halfWidth;
	}

	player_->SetWorldPosition(pos);
}

// デストラクタ
GameScene1_2::~GameScene1_2() {
	// 生成したインスタンスの解放
	delete dirtModel_;     // ブロックモデルの解放
	delete debaucamera_;   // デバッグカメラの解放
	delete modelSkydome_;  // スカイドームモデルの解放
	delete player_;        // プレイヤーの解放
	delete mapChipField_;  // マップチップフィールドの解放
	delete deatparticles_; // パーティクルの解放
	delete CController_;   // カメラコントローラーの解放
	delete fade_;          // フェードの解放
	// ヒットエフェクトの解放
	for (HitEffect* hitEffect : hitEffects_) {
		delete hitEffect; // 各ヒットエフェクトの解放
	}

	// 生成したブロックのWorldTransformインスタンスを全て解放
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlocks : worldTransformBlockLine) {
			delete worldTransformBlocks;
		}
	}
	for (Enemy* enemy : enemys_) { // C++11以降の範囲ベースforループ
		delete enemy;
	}

	worldTransformBlocks_.clear(); // ベクターをクリア
}
