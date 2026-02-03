#include "GameScene.h" 
#include <iostream>
#include "GameStateManager.h"

using namespace KamataEngine; 

// GameSceneの初期化処理
void GameScene::Initialize(int stageID) {
	currentStageID_ = stageID;

	//========================================================================
	// 📦 1. モデル・テクスチャ・リソース読み込み
	//========================================================================

	// --- テクスチャ ---
	textureHandel_ = TextureManager::Load("sample.png");
	textureHandlePhose_ = TextureManager::Load("Phose.png");
	TextureHandleYazirusi_ = TextureManager::Load("yazirusi.png");
	textureHandleEnter_ = TextureManager::Load("enter.png");
	textureHandleGameClearText_ = TextureManager::Load("TextSpriteGameClear.png");
	textureHandlePauseText_ = TextureManager::Load("phoseText.png");

	// UI関連
	textureHandleHpIconNormal_ = TextureManager::Load("playerAikon.png");
	textureHandleHpIconDamage_ = TextureManager::Load("playerAikonDamage.png");
	textureHandleHeart_ = TextureManager::Load("playerHp.png");
	texHandleCoinEmpty_ = TextureManager::Load("noKoinUi.png");
	texHandleCoinGet_ = TextureManager::Load("koinUi.png");

	// カウントダウン画像
	textureHandleCountdown3_ = TextureManager::Load("3.png");
	textureHandleCountdown2_ = TextureManager::Load("2.png");
	textureHandleCountdown1_ = TextureManager::Load("1.png");
	textureHandleCountdownGo_ = TextureManager::Load("go.png");

	// ステージ看板画像
	textureHandle1_1_ = TextureManager::Load("1-1.png");
	textureHandle1_2_ = TextureManager::Load("1-2.png");
	textureHandle1_3_ = TextureManager::Load("1-3.png");

	// ボスの名前画像
	textureHandleBossName_ = TextureManager::Load("boss_name.png");

	// --- モデル ---
	dirtModel_ = Model::CreateFromOBJ("tutiBlock", true);
	grassModel_ = Model::CreateFromOBJ("tutiBlockSibahu", true);
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	playerModel_ = Model::CreateFromOBJ("player", true);
	deatparticlesModel_ = Model::CreateFromOBJ("deathParticle", true);
	breakableBlockModel_ = Model::CreateFromOBJ("Breakable", true);
	hitEffectModel_ = Model::CreateFromOBJ("hit", true);
	goalModel_ = Model::CreateFromOBJ("gorl", true);
	GameClearTextModel_ = Model::CreateFromOBJ("GameClear", true);
	togeKabeModel_ = Model::CreateFromOBJ("kabeToge", true);
	togeModel_ = Model::CreateFromOBJ("toge", true);
	yamaModel = Model::CreateFromOBJ("yama", true);
	kinokoModel_ = Model::CreateFromOBJ("kinoko", true);
	iceBlockModel_ = Model::CreateFromOBJ("iceBlock", true);
	umbrellaModel_ = Model::CreateFromOBJ("parasol", true);
	hasigoModel_ = Model::CreateFromOBJ("hasigo", true);
	kumoModel_ = Model::CreateFromOBJ("kumo", true);
	iwaModel_ = Model::CreateFromOBJ("iwa", true);
	starCoinModel_ = Model::CreateFromOBJ("koin", true);
	treeModel_ = Model::CreateFromOBJ("tree", true);
	particleModel_ = Model::CreateFromOBJ("deathParticle", true);

	// 敵モデル
	enemy_model_Walk = Model::CreateFromOBJ("enemy", true);
	enemy_model_Shooter = Model::CreateFromOBJ("enemyShooter", true);
	enemy_model_Homing = Model::CreateFromOBJ("enemyHoming", true);

	//========================================================================
	// 🎥 2. カメラの設定
	//========================================================================
	camera_.farZ = 1280.0f;
	camera_.Initialize();

	debaucamera_ = new DebugCamera(100, 50);
	debaucamera_->SetFarZ(1280.0f);

	CController_ = new CameraController();
	CController_->Initialize(&camera_);

	//========================================================================
	// 🗺️ 3. マップ & フィールド初期化
	//========================================================================
	std::stringstream ss;
	ss << "Resources/map/1-" << stageID << ".csv";
	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv(ss.str());

	// ゴール隠し処理（ボス戦の場合）
	if (currentStageID_ == 5) {
		for (uint32_t y = 0; y < mapChipField_->GetNumBlockVirtcal(); ++y) {
			for (uint32_t x = 0; x < mapChipField_->GetNumBlockHorizonal(); ++x) {
				if (mapChipField_->GetMapChipTypeByindex(x, y) == MapChipType::kGoal_) {
					mapChipField_->SetMapChipType(x, y, MapChipType::kBlank_);
				}
			}
		}
	}

	//========================================================================
	// 🌪️ 4. パーティクル初期化
	//========================================================================
	particleManager_ = new ParticleManager();
	particleManager_->Initialize(particleModel_, &camera_);

	//========================================================================
	// 🧍 5. プレイヤーの初期化
	//========================================================================
	// デフォルト位置
	Vector3 playerPosition = mapChipField_->GetChipPositionIndex(3, 2);

	// スタート地点(90)を探す
	for (uint32_t y = 0; y < mapChipField_->GetNumBlockVirtcal(); ++y) {
		for (uint32_t x = 0; x < mapChipField_->GetNumBlockHorizonal(); ++x) {
			if (mapChipField_->GetMapChipTypeByindex(x, y) == MapChipType::kPlayerStart_) {
				playerPosition = mapChipField_->GetChipPositionIndex(x, y);
				mapChipField_->SetMapChipType(x, y, MapChipType::kBlank_); // 空白に戻す
			}
		}
	}

	player_ = new Player();
	player_->Initialize(playerModel_, &camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);
	player_->SetisMove(false);
	player_->SetParticleManager(particleManager_);
	player_->SetUmbrellaModel(umbrellaModel_);
	player_->SetCameraController(CController_);

	// カメラコントローラー追従設定
	CController_->SetTarget(player_);
	CController_->Reset();
	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 40.0f};
	CController_->SetMovableSrea(cameraArea);

	// 死亡パーティクル
	deatparticles_ = new DeathParticles();
	deatparticles_->Initialize(deatparticlesModel_, &camera_, player_, playerPosition);

	//========================================================================
	// 👹 6. 敵・オブジェクト・ブロック生成
	//========================================================================

	// 通常ブロック生成
	GenerrateBlock();

	// ボス生成（ステージ5専用）
	entranceTimer_ = 0.0f;
	isBossDefeated_ = false;

	if (currentStageID_ == 5) {
		Enemy* boss = new Enemy();

		Vector3 bossPos = mapChipField_->GetChipPositionIndex(50, 10);
		bossPos.y += 20.0f;

		// 初期化
		boss->Initialize(enemy_model_Walk, &camera_, bossPos, Enemy::Type::kBoss);
		boss->SetPlayer(player_);
		boss->SetMapChipField(mapChipField_);
		boss->SetGameScene(this);
		boss->SetParticleManager(particleManager_);
		// 最初は登場用ステートにしておく
		boss->SetBossState(Enemy::BossState::kEntrance);

		enemys_.push_back(boss);

		// 演出開始まで動かさない
		isBossActive_ = false;
	} else {
		// 他のステージは最初からアクティブ
		isBossActive_ = true;
	}

	// トゲ壁
	togeKabe_ = new KabeToge();
	togeKabe_->Initialize(togeKabeModel_, &camera_, mapChipField_->GetChipPositionIndex(0, 18));
	togeKabe_->SetIsmove(false);

	// 背景オブジェクト
	yama_.push_back(new Yama());
	yama_.back()->Initialize(yamaModel, &camera_, mapChipField_->GetChipPositionIndex(20, 19));
	yama_.push_back(new Yama());
	yama_.back()->Initialize(yamaModel, &camera_, mapChipField_->GetChipPositionIndex(70, 19));

	tree_.push_back(new Tree());
	tree_.back()->Initialize(treeModel_, &camera_, mapChipField_->GetChipPositionIndex(5, 19));
	tree_.push_back(new Tree());
	tree_.back()->Initialize(treeModel_, &camera_, mapChipField_->GetChipPositionIndex(90, 19));

	HitEffect::SetModel(hitEffectModel_);
	HitEffect::SetCamera(&camera_);

	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);

	//========================================================================
	// 🎨 7. スプライト(UI)生成
	//========================================================================
	TextSprite1_1 = Sprite::Create(textureHandle, {100.50});
	poseSprite = Sprite::Create(textureHandlePhose_, {0.0});
	yazirusiSprite = Sprite::Create(TextureHandleYazirusi_, {180, 190});
	enterSprite_ = Sprite::Create(textureHandleEnter_, {0.0f, 0.0f});
	pauseTextSprite_ = Sprite::Create(textureHandlePauseText_, {0.0f, 0.0f});
	GameClearTextSprite_ = Sprite::Create(textureHandleGameClearText_, {0.0f, 0.0f});

	// ボス演出用スプライト
	spriteBossName_ = Sprite::Create(textureHandleBossName_, {-1000.0f, 0.0f}); // 最初は画面外

	// カウントダウン
	spriteCountdown_ = Sprite::Create(textureHandleCountdown3_, {0, 0});

	// ステージ看板
	Vector2 spritePos = {200.0f, 0.0f};
	sprite1_1_ = Sprite::Create(textureHandle1_1_, spritePos);
	sprite1_2_ = Sprite::Create(textureHandle1_2_, spritePos);
	sprite1_3_ = Sprite::Create(textureHandle1_3_, spritePos);

	// HPアイコン
	Vector2 hpIconPos = {20.0f, 650.0f};
	spriteHpIconNormal_ = Sprite::Create(textureHandleHpIconNormal_, hpIconPos);
	spriteHpIconDamage_ = Sprite::Create(textureHandleHpIconDamage_, hpIconPos);

	// HPハート
	Vector2 heartBasePos = {100.0f, 650.0f};
	float heartSpacing = 64.0f;
	for (int i = 0; i < kMaxPlayerHp; i++) {
		Vector2 pos = {heartBasePos.x + (i * heartSpacing), heartBasePos.y};
		spriteHearts_.push_back(Sprite::Create(textureHandleHeart_, pos));
	}

	// スターコインUI
	currentStarCoinCount_ = GameStateManager::GetInstance()->GetStarCoinRecord(currentStageID_);
	if (currentStarCoinCount_ > 3)
		currentStarCoinCount_ = 3;

	for (int i = 0; i < 3; i++) {
		uiStarCoins_[i] = Sprite::Create(texHandleCoinEmpty_, {0, 0});
		uiStarCoins_[i]->SetPosition({30.0f + (i * 50.0f), 30.0f});
		if (i < currentStarCoinCount_) {
			uiStarCoins_[i]->SetTextureHandle(texHandleCoinGet_);
		}
	}

	//========================================================================
	// 🏁 8. その他初期化
	//========================================================================

	// フェード開始
	fade_ = new Fade();
	fade_->Initalize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	finishedTimer = 0;

	// ゲームクリアテキスト座標
	GameClearTextWorldTransform_.Initialize();
	GameClearTextWorldTransform_.scale_ = {5.0f, 1.0f, 1.0f};
	GameClearTextWorldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	GameClearTextWorldTransform_.translation_ = mapChipField_->GetChipPositionIndex(20, 18);

	// ① 白画像の読み込み
	texHandleWhite_ = TextureManager::Load("white.png");

	// ② 背景用スプライト生成（黒く塗る）
	spriteHPBack_ = Sprite::Create(texHandleWhite_, {0, 0});
	spriteHPBack_->SetColor({0.2f, 0.2f, 0.2f, 1.0f}); // ダークグレー
	spriteHPBack_->SetSize({600.0f, 30.0f});           // 横長にする
	spriteHPBack_->SetPosition({340.0f, 150.0f});       // 画面上部中央あたり

	// ③ 前面用スプライト生成（赤く塗る）
	spriteHPGauge_ = Sprite::Create(texHandleWhite_, {0, 0});
	spriteHPGauge_->SetColor({1.0f, 0.2f, 0.2f, 1.0f}); // 赤色
	spriteHPGauge_->SetSize({600.0f, 30.0f});           // 背景と同じサイズで開始
	spriteHPGauge_->SetPosition({340.0f, 150.0f});       // 背景と同じ位置


	// ① ボス名画像の読み込み
	 texHandleBossName_ = TextureManager::Load("boss_name.png");

	// ② スプライト生成
	spriteBossName_ = Sprite::Create(texHandleBossName_, {640.0f, 360.0f});

	// ★重要：画像の中心を基準点にする（中央表示やズーム演出が楽になります）
	spriteBossName_->SetAnchorPoint({0.5f, 0.5f});

	// 最初は画面外に隠しておく
	spriteBossName_->SetPosition({1000.0f, 0.0f});

	// 仮画像の場合、とりあえず色をつけておく
	spriteBossName_->SetColor({1.0f, 1.0f, 1.0f, 1.0f}); 
	spriteBossName_->SetSize({800.0f, 160.0f});  

	// ゲーム状態管理
	GameStateManager::GetInstance()->SetCurrentStageID(currentStageID_);

	isSprite = true;
	firstFrame = true;
	currentPlayCoinCount_ = 0;
	starCoins_.clear();
	currentSelectIndex_ = 0;
}

// ==============================
// 更新処理
// ==============================
void GameScene::Update() {
	// フェードの更新 & フェーズ管理
	fade_->Update();
	LimitPlayerPosition();
	for (Yama* yama : yama_) {
		yama->Update();
	}
	for (Tree* tree : tree_) {
		tree->Update();
	}
	if (gorl_) {
		gorl_->Update();
	}
	ChangePhase();
	particleManager_->Update();

	// ポーズ状態の切り替え
	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_P) && !player_->IsDead() && !isGameClear_) {
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
		if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_W)) {
			if (currentSelect_ == PauseSelect::kContinue) {
				currentSelect_ = PauseSelect::kTitle;
			} else if (currentSelect_ == PauseSelect::kStageSelect) {
				currentSelect_ = PauseSelect::kContinue;
			} else if (currentSelect_ == PauseSelect::kTitle) {
				currentSelect_ = PauseSelect::kStageSelect;
			}
		}
		if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_S)) {
			if (currentSelect_ == PauseSelect::kContinue) {
				currentSelect_ = PauseSelect::kStageSelect;
			} else if (currentSelect_ == PauseSelect::kStageSelect) {
				currentSelect_ = PauseSelect::kTitle;
			} else if (currentSelect_ == PauseSelect::kTitle) {
				currentSelect_ = PauseSelect::kContinue;
			}
		}

		if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			if (currentSelect_ == PauseSelect::kContinue) {
				isPaused_ = false;
			} else if (currentSelect_ == PauseSelect::kStageSelect) {
				Vector3 signboardPosition;
				switch (currentStageID_) {
				case 1:
					signboardPosition = mapChipField_->GetChipPositionIndex(10, 17);
					break;
				case 2:
					signboardPosition = mapChipField_->GetChipPositionIndex(21, 9);
					break;
				case 3:
					signboardPosition = mapChipField_->GetChipPositionIndex(36, 9);
					break;
				case 4:
					signboardPosition = mapChipField_->GetChipPositionIndex(57, 17);
					break;
				case 5:
					signboardPosition = mapChipField_->GetChipPositionIndex(75, 17);
					break;
				default:
					signboardPosition = mapChipField_->GetChipPositionIndex(10, 17);
					break;
				}
				GameStateManager::GetInstance()->SetPlayerStartPosition(signboardPosition);
				finished_ = true;
			} else if (currentSelect_ == PauseSelect::kTitle) {
				currentSelectIndex_ = 1;
				finished_ = true;
			}
		}
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
		player_->Update();
		togeKabe_->Update();
		for (Enemy* enemy : enemys_) {
			enemy->Update();
		}

		CheekAllcollision();
		CController_->Update();

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
		if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_E)) {
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
		break;

	// ------------------------------
	// カウントダウン中の処理
	// ------------------------------
	case Phase::kCountdown:
		countdownTimer_ += 1.0f / 60.0f;

		if (countdownTimer_ >= 0.0f && countdownTimer_ < 1.0f) {
			spriteCountdown_ = Sprite::Create(textureHandleCountdown3_, {-250, -100});
		} else if (countdownTimer_ >= 1.0f && countdownTimer_ < 2.0f) {
			spriteCountdown_ = Sprite::Create(textureHandleCountdown2_, {-250, -100});
		} else if (countdownTimer_ >= 2.0f && countdownTimer_ < 3.0f) {
			spriteCountdown_ = Sprite::Create(textureHandleCountdown1_, {-250, -100});
		} else {
			spriteCountdown_ = Sprite::Create(textureHandleCountdownGo_, {-250, -100});
			if (countdownTimer_ >= 3.5f) {
				countdownState_ = CountdownState::kFinished;
				phase_ = Phase::kPlay;
				player_->SetisMove(true);
				togeKabe_->SetIsmove(true);
			}
		}
		break;

	// ------------------------------
	// プレイ中の処理
	// ------------------------------


	     case Phase::kPlay:
		particleManager_->Update();
		player_->Update();
		togeKabe_->Update();

		// 敵の更新制御
		if (currentStageID_ == 5) {
			if (isBossActive_) {
				for (Enemy* enemy : enemys_) {
					enemy->Update();
				}
			}
			// ボス登場演出への遷移
			if (!isBossDefeated_ && !isBossActive_) {
				if (player_->GetWorldPosition().x > 43.0f) {
					phase_ = Phase::kBossEntrance;
					entranceTimer_ = 0.0f;
					player_->SetIsCutscene(true);
				}
			}
		} else {
			for (Enemy* enemy : enemys_) {
				enemy->Update();
			}
		}

		switch (currentStageID_) {
		case 1:
		case 2:
		case 3:
			player_->CheckAndResolveTogeKabeCollision(togeKabe_);
			break;
		case 4:
			break;
		}

		CheekAllcollision();
		CController_->Update();

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
		if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_E)) {
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

		if (auto stepped = player_->CheckSteppedBreakable(mapChipField_)) {
			for (auto& floor : breakableFloors_) {
				if (floor->GetIndexX() == stepped->xIndex && floor->GetIndexY() == stepped->yIndex) {
					floor->OnStepped();
					break;
				}
			}
		}

	
		if (currentStageID_ == 5) {
			Enemy* boss = nullptr;
			for (Enemy* e : enemys_) {
				if (e->GetType() == Enemy::Type::kBoss) {
					boss = e;
					break;
				}
			}

			if (boss && !boss->isDead()) {
				// HPバー更新
				if (bossMaxHp_ == 0)
					bossMaxHp_ = boss->GetHp();
				float ratio = (float)boss->GetHp() / (float)bossMaxHp_;
				ratio = std::clamp(ratio, 0.0f, 1.0f);
				if (spriteHPGauge_)
					spriteHPGauge_->SetSize({600.0f * ratio, 30.0f});

				// ★ボス名の位置固定（戦闘中は HPバーの上に常駐）
				if (spriteBossName_) {
					spriteBossName_->SetPosition({380.0f, 0.0f}); // HPバーの上
					spriteBossName_->SetSize({1280.0f, 360.0f});     // 戦闘用サイズ
				}
			} else {
				// ボス死亡時は消す
				if (spriteHPGauge_)
					spriteHPGauge_->SetSize({0.0f, 0.0f});
				if (spriteHPBack_)
					spriteHPBack_->SetSize({0.0f, 0.0f});
				if (spriteBossName_)
					spriteBossName_->SetPosition({-1000.0f, 0.0f});
			}
		}
		break;

	// ------------------------------
	// ボス登場演出フェーズ
	// ------------------------------
	case Phase::kBossEntrance:
		entranceTimer_ += 1.0f / 60.0f;

		// ① 落下開始
		if (entranceTimer_ >= 0.5f && entranceTimer_ < 0.6f) {
			isBossActive_ = true;
		}



		// ③ 戦闘開始！
		if (entranceTimer_ >= 4.0f) {
			phase_ = Phase::kPlay;
			player_->SetIsCutscene(false);
			bossHpInitialized_ = true;

		

			// ボスを待機状態へ
			for (Enemy* e : enemys_) {
				if (e->GetType() == Enemy::Type::kBoss) {
					e->SetBossState(Enemy::BossState::kWait);
				}
			}
		}

		// 演出中の更新処理
		particleManager_->Update();
		player_->Update();
		if (isBossActive_) {
			for (Enemy* enemy : enemys_) {
				enemy->Update();
			}
		}
		camera_.UpdateMatrix();
		skydome_->Update();
		for (std::vector<WorldTransform*>& blockLine : worldTransformBlocks_) {
			for (WorldTransform* block : blockLine) {
				if (!block)
					continue;
				block->matWorld_ = math->MakeAffineMatrix(block->scale_, block->rotation_, block->translation_);
				block->TransferMatrix();
			}
		}
		break;

	// ------------------------------
	// 死亡中の処理
	// ------------------------------
	case Phase::kDeath:
		for (Enemy* enemy : enemys_) {
			enemy->Update();
		}

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
		if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_E)) {
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
				it = breakableFloors_.erase(it);
			} else {
				++it;
			}
		}
		break;

	// ------------------------------
	// ゲームクリア処理
	// ------------------------------
	case Phase::GameClear:
		math->worldTransFormUpdate(GameClearTextWorldTransform_);
		break;
	}

	// ==============================
	// 共通処理 (敵死亡・ヒットエフェクト等)
	// ==============================
	for (HitEffect* hitEffect : hitEffects_) {
		hitEffect->Update();
	}

	std::list<Enemy*> newEnemies;
	auto it = enemys_.begin();
	while (it != enemys_.end()) {
		Enemy* enemy = *it;

		if (enemy->isDead()) {
			if (enemy->GetType() == Enemy::Type::kFlee) {
				Vector3 pos = enemy->GetWorldPosition();
				MapChipField::IndexSet index = mapChipField_->GetMapChipIndexSetByPosition(pos);
				mapChipField_->SetMapChipType(index.xIndex, index.yIndex, MapChipType::kStarCoin_);
			}

			if (enemy->GetIsSplit()) {
				for (int i = 0; i < 2; i++) {
					Enemy* newEnemy = new Enemy();
					Vector3 pos = enemy->GetWorldPosition();
					newEnemy->Initialize(enemy_model_Walk, &camera_, pos, Enemy::Type::kWalk);
					newEnemy->SetMapChipField(mapChipField_);
					newEnemy->SetPlayer(player_);
					newEnemy->SetIsSplit(false);
					Vector3 velocity = (i == 0) ? Vector3(-0.1f, 0.5f, 0.0f) : Vector3(0.1f, 0.5f, 0.0f);
					newEnemy->SetVelocity(velocity);
					newEnemies.push_back(newEnemy);
				}
			}

			if (currentStageID_ == 5 && enemy->GetType() == Enemy::Type::kBoss) {
				if (!isBossDefeated_) {
					isBossDefeated_ = true;
					Vector3 deadPos = enemy->GetWorldPosition();
					MapChipField::IndexSet index = mapChipField_->GetMapChipIndexSetByPosition(deadPos);
					mapChipField_->SetMapChipType(index.xIndex, index.yIndex, MapChipType::kGoal_);
					if (index.yIndex < worldTransformBlocks_.size() && index.xIndex < worldTransformBlocks_[index.yIndex].size()) {
						if (worldTransformBlocks_[index.yIndex][index.xIndex] != nullptr) {
							delete worldTransformBlocks_[index.yIndex][index.xIndex];
							worldTransformBlocks_[index.yIndex][index.xIndex] = nullptr;
						}
					}
					Vector3 goalPos = mapChipField_->GetChipPositionIndex(index.xIndex, index.yIndex);
					if (gorl_ == nullptr) {
						gorl_ = new Gorl();
						gorl_->Initialize(goalModel_, &camera_, goalPos);
					}
				}
			}
			delete enemy;
			it = enemys_.erase(it);
		} else {
			++it;
		}
	}

	for (Enemy* newEnemy : newEnemies) {
		enemys_.push_back(newEnemy);
	}
	for (Enemy* newEnemy : pendingEnemies_) {
		enemys_.push_back(newEnemy);
	}
	pendingEnemies_.clear();

	hitEffects_.remove_if([](HitEffect* effect) {
		if (effect->IsDead()) {
			delete effect;
			return true;
		}
		return false;
	});

	// ボスを探す（enemys_リストからボスを特定）
	if (phase_ == Phase::kPlay && currentStageID_ == 5) {

		Enemy* boss = nullptr;
		// ボスを探す（メンバ変数に boss_ を持っていない場合の検索）
		for (Enemy* e : enemys_) {
			if (e->GetType() == Enemy::Type::kBoss) {
				boss = e;
				break;
			}
		}

		if (boss && !boss->isDead()) {
			// 最大HPの取得（初回のみ）
			if (bossMaxHp_ == 0)
				bossMaxHp_ = boss->GetHp();

			// 割合計算
			float ratio = (float)boss->GetHp() / (float)bossMaxHp_;
			ratio = std::clamp(ratio, 0.0f, 1.0f); // 0.0~1.0に収める

			// サイズ更新
			if (spriteHPGauge_)
				spriteHPGauge_->SetSize({600.0f * ratio, 30.0f});

			// 位置補正（もし中心基準などでズレる場合はここで再設定）
			// spriteHPGauge_->SetPosition(...);
		} else {
			// ボスが死んだらバーを消す（サイズ0）
			if (spriteHPGauge_)
				spriteHPGauge_->SetSize({0.0f, 0.0f});
			if (spriteHPBack_)
				spriteHPBack_->SetSize({0.0f, 0.0f});
		}
	}
	if (particleManager_) {
		// マップ全体を走査
		for (uint32_t y = 0; y < mapChipField_->GetNumBlockVirtcal(); ++y) {
			for (uint32_t x = 0; x < mapChipField_->GetNumBlockHorizonal(); ++x) {

				// もしそこがスターコインなら
				if (mapChipField_->GetMapChipTypeByindex(x, y) == MapChipType::kStarCoin_) {

					// 30フレームに1回くらいの頻度でキラッとさせる
					if (rand() % 30 == 0) {
						// コインの中心座標を計算
						Vector3 pos = mapChipField_->GetChipPositionIndex(x, y);

						// 少し中心からずらす
						pos.x += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.5f;
						pos.y += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.5f;

						// 速度：ゆっくり上に昇る
						Vector3 vel = {0.0f, 0.05f, 0.0f};
						Vector3 accel = {0.0f, 0.0f, 0.0f};

						// 色：黄色
						Vector4 startColor = {1.0f, 1.0f, 0.0f, 0.8f}; // 黄色
						Vector4 endColor = {1.0f, 1.0f, 0.0f, 0.0f};  

						// 寿命：少し長め
						particleManager_->Emit(pos, vel, accel, 0.6f, 0.4f, 0.0f, startColor, endColor);
					}
				}
			}
		}
	}
#ifdef _DEBUG
	ImGui::Begin("Star Coin Debug");
	ImGui::Text("Current Stage: %d", currentStageID_);
	ImGui::Text("Now Coins (In Game): %d", currentPlayCoinCount_);
	int savedRecord = GameStateManager::GetInstance()->GetStarCoinRecord(currentStageID_);
	ImGui::Text("Saved Record (DB): %d", savedRecord);
	ImGui::Separator();
	if (ImGui::TreeNode("All Stage Records")) {
		for (int i = 1; i <= 5; i++) {
			int count = GameStateManager::GetInstance()->GetStarCoinRecord(i);
			ImGui::Text("Stage %d: %d", i, count);
		}
		ImGui::TreePop();
	}
	ImGui::End();
#endif
}
void GameScene::Draw() {
	DirectXCommon* dxcommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxcommon->GetCommandList());

	// コインのアニメーション用に時間を進める
	static float globalTime = 0.0f;
	globalTime += 1.0f / 60.0f;

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
					// std::cout << "Grass at (" << x << ", " << y << ") is being processed.\n";
				}

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
				case MapChipType::kJumpPad_:
					kinokoModel_->Draw(*block, camera_);
					break;
				case MapChipType::kIceFloor_:
					iceBlockModel_->Draw(*block, camera_);
					break;
				case MapChipType::kLadder_:
					hasigoModel_->Draw(*block, camera_);
					break;
				case MapChipType::kCloud_:
					kumoModel_->Draw(*block, camera_);
					break;
				case MapChipType::kWallBreak_:
					iwaModel_->Draw(*block, camera_);
					break;

				// ==========================================
				// スターコインのアニメーション
				// ==========================================
				case MapChipType::kStarCoin_:
					// 1. くるくる回す (Y軸回転)
					block->rotation_.y += 0.05f;
					{ 
						Vector3 originPos = mapChipField_->GetChipPositionIndex(x, y);
						block->translation_.y = originPos.y + std::sin(globalTime * 3.0f) * 0.5f;
					}


					starCoinModel_->Draw(*block, camera_);
					break;
				}
			}
		}
	}

	for (auto& floor : breakableFloors_) {
		floor->Draw(breakableBlockModel_, &camera_); // 破壊可能な床の描画
	}



	switch (currentStageID_) {
	case 1:
		togeKabe_->Draw(); // トゲ壁の描画
		break;
	case 2:
		togeKabe_->Draw(); // トゲ壁の描画
		break;
	case 3:
		togeKabe_->Draw(); // トゲ壁の描画
		break;
	}

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
	if (gorl_) {
		Model::PreDraw(dxcommon->GetCommandList());
		gorl_->Draw();
		Model::PostDraw();
	}

	if (particleManager_) {
		particleManager_->Draw(dxcommon->GetCommandList());
	}
	// 🎉 ゲームクリア表示
	//if (isGameClear_) {
	//	Model::PreDraw(dxcommon->GetCommandList());
	//	GameClearTextModel_->Draw(GameClearTextWorldTransform_, camera_);
	//	Model::PostDraw();
	//}
	Sprite::PreDraw(dxcommon->GetCommandList());
	// 🌗 フェード描画

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

	switch (currentStageID_) {
	case 1:
		sprite1_1_->Draw();
		break;
	case 2:
		sprite1_2_->Draw();
		break;
	case 3:
		sprite1_3_->Draw();
		break;
	}
	fade_->Draw(dxcommon->GetCommandList());
	Model::PostDraw();

	Sprite::PreDraw(dxcommon->GetCommandList());
	if (countdownState_ == CountdownState::kCounting) {
		spriteCountdown_->Draw();
	}
	if (isSprite && !isGameClear_ && !isPaused_) {
		TextSprite1_1->Draw();
		pauseTextSprite_->Draw(); // ポーズスプライトの描画
	}

	if (isPaused_) {
		poseSprite->Draw();
		yazirusiSprite->Draw();
		enterSprite_->Draw(); // エンターキー用スプライトの描画
	}

	// ゲームクリアテキストスプライトの描画
	if (isGameClear_ && isSprite) {
		GameClearTextSprite_->Draw();
		enterSprite_->Draw(); // エンターキー用スプライトの描画
	}

if (currentStageID_ != 5) {

		for (int i = 0; i < 3; i++) {
			if (uiStarCoins_[i] != nullptr) {
				uiStarCoins_[i]->Draw();
			}
		}
	}
if (currentStageID_ == 5) {

	// 1. HPバーは「戦闘中（Play）」かつ「初期化済み」の時だけ
	if (phase_ == Phase::kPlay && bossHpInitialized_) {
		if (spriteHPBack_)
			spriteHPBack_->Draw();
		if (spriteHPGauge_)
			spriteHPGauge_->Draw();
		if (spriteBossName_) {
			spriteBossName_->Draw();
		}
	}


}

	// if (spriteBossName_) spriteBossName_->Draw();
	Sprite::PostDraw();


}

void GameScene::GenerrateBlock() {
	const uint32_t kNumBlockVirtal = mapChipField_->GetNumBlockVirtcal();
	const uint32_t kNumBlockHorizontal = mapChipField_->GetNumBlockHorizonal();

	worldTransformBlocks_.resize(kNumBlockVirtal);
	for (uint32_t i = 0; i < kNumBlockVirtal; ++i) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal, nullptr); // nullptrで初期化
	}

	for (uint32_t y = 0; y < kNumBlockVirtal; ++y) {
		for (uint32_t x = 0; x < kNumBlockHorizontal; ++x) {
			MapChipType type = mapChipField_->GetMapChipTypeByindex(x, y);
			Vector3 pos = mapChipField_->GetChipPositionIndex(x, y);

			// 1. 何もない場所はスキップ
			if (type == MapChipType::kBlank_) {
				continue;
			}
			    // ==========================================
			    //  2. 敵の生成処理
			    // ==========================================
			    if (type == MapChipType::kEnemyWalk_ || type == MapChipType::kEnemyShooter_ || type == MapChipType::kEnemyHoming_ || type == MapChipType::kEnemySplit_ || type == MapChipType::kSlime ||
			        type == MapChipType::kFlee || type == MapChipType::kEnemyFlying_) {

				Enemy* newEnemy = new Enemy();

				// タイプ別に初期化
				if (type == MapChipType::kEnemyWalk_) {
					newEnemy->Initialize(enemy_model_Walk, &camera_, pos, Enemy::Type::kWalk);
				} else if (type == MapChipType::kEnemyShooter_) {
					newEnemy->Initialize(enemy_model_Shooter, &camera_, pos, Enemy::Type::kShooter);
				} else if (type == MapChipType::kEnemyHoming_) {
					newEnemy->Initialize(enemy_model_Homing, &camera_, pos, Enemy::Type::kHoming);
				} else if (type == MapChipType::kEnemySplit_) {
					newEnemy->Initialize(enemy_model_Walk, &camera_, pos, Enemy::Type::kSplit);
				} else if (type == MapChipType::kSlime) {
					newEnemy->Initialize(enemy_model_Walk, &camera_, pos, Enemy::Type::kSlime);
				} else if (type == MapChipType::kFlee) {
					newEnemy->Initialize(enemy_model_Walk, &camera_, pos, Enemy::Type::kFlee);
				}
				else if (type == MapChipType::kEnemyFlying_) {
					newEnemy->Initialize(enemy_model_Walk, &camera_, pos, Enemy::Type::kFlying);
				}

				newEnemy->SetPlayer(player_);
				newEnemy->SetMapChipField(mapChipField_);
				newEnemy->SetParticleManager(particleManager_);
				// リストに追加
				enemys_.push_back(newEnemy);

				mapChipField_->SetMapChipType(x, y, MapChipType::kBlank_);

				continue; // 次のループへ（ブロック生成はしない）
			}

			// ==========================================
			// 3. 壊れる床
			// ==========================================
			if (type == MapChipType::kBreakable_) {
				auto floor = std::make_unique<BreakableFloor>();
				floor->Initialize(pos, x, y, mapChipField_);
				breakableFloors_.push_back(std::move(floor));

				worldTransformBlocks_[y][x] = nullptr;
			}
			// ==========================================
			// 4. 通常ブロック (それ以外)
			// ==========================================
			else {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransform->translation_ = pos;
				worldTransformBlocks_[y][x] = worldTransform;
			}
		}
	}
}

void GameScene::CheekAllcollision() {
	// プレイヤーAABB
	AABB aabb1 = player_->GetAABB();
	const auto& pBullets = player_->GetBullets(); // プレイヤーの弾リスト
	for (Enemy* enemy : enemys_) {
		// 死亡しているか、当たり判定が無効化されている敵はスキップ
		if (enemy->isDead() || enemy->isCollisonDisabled()) {
			continue;
		}

		AABB aabb2 = enemy->GetAABB();
		if (math->IsCollision(aabb1, aabb2)) {

			if (enemy->GetType() == Enemy::Type::kHoming) {

				// 1. 大ダメージを与える
				player_->TakeDamage(2);

				// 2. 派手なSEを鳴らす（ダメージ音や爆発音）
		/*		Audio::GetInstance()->PlayWave(seDamageHandle_, false, 1.0f);*/

				// 3. 敵は自爆して消滅
				enemy->SetDead(true);

				// 4. その場にパーティクル（爆発エフェクト）を出す
				// 既存の DeathParticles を流用して爆発っぽく見せる
				deatparticles_->Initialize(deatparticlesModel_, &camera_, player_, enemy->GetWorldPosition());

				// 衝突処理終わり
				continue;
			}




			// ここでは、踏んだかどうかの判定のみに集中する
			if (aabb1.min.y >= aabb2.max.y - 4.0f && !player_->GetOnGround()) {

				if (enemy->GetType() == Enemy::Type::kFlying) {
					player_->SetVelocityY(0.8f); // 大ジャンプ
				} else {
					player_->SetVelocityY(0.3f); // 通常ジャンプ

				}
				enemy->TakeDamage(5);
			} else {
				player_->TakeDamage(1);
			}

		}

		for (EnemyBullet* bullet : enemy->GetBullets()) {
			if (bullet->IsDead())
				continue;

			AABB bulletAABB = bullet->GetAABB();
			if (math->IsCollision(aabb1, bulletAABB)) {
				// プレイヤーにダメージ
				player_->TakeDamage(1);

				// 弾を消す
				bullet->OnCollision();
			}
		}

		for (PlayerBullet* bullet : pBullets) {
			if (bullet->IsDead())
				continue;

			AABB bulletAABB = bullet->GetAABB();
			if (math->IsCollision(aabb2, bulletAABB)) {

				// 命中！

				// 弾が当たった！
				bullet->OnCollision();

				
				enemy->TakeDamage(bullet->GetDamage());

				CreateHitEffect(enemy->GetWorldPosition());
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
			if (type != MapChipType::kGoal_ && type != MapChipType::kSpike_ && type != MapChipType::kStarCoin_) {
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

					isGameClear_ = true;
					player_->StartVictoryPose(); // 勝利ポーズ開始
					CController_->StartVictoryZoom(player_);
					// CController_->StartVictoryZoom(player_);
				} else if (type == MapChipType::kSpike_) {
					// 棘のダメージ処理
					player_->TakeDamage(1);
				} else if (type == MapChipType::kStarCoin_) {
					if (currentPlayCoinCount_ < 3) {
						uiStarCoins_[currentPlayCoinCount_]->SetTextureHandle(texHandleCoinGet_);
					}
					// 1. カウントを増やす
					currentPlayCoinCount_++;

					// 2. マップデータを「空白(0)」に書き換える（これで消える＆次から当たらない）
					mapChipField_->SetMapChipType(x, y, MapChipType::kBlank_);

					// 3. SEを鳴らす（あれば）
					// Audio::GetInstance()->PlayWave(soundHandleCoin_);

				}
			}
		}
	}

}


void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kFadeIn:
		if (fade_->isFinished()) {
			countdownState_ = CountdownState::kCounting;
			phase_ = Phase::kCountdown; // フェードインが完了したらカウントダウンフェーズへ
		}
		break;
	case Phase::kCountdown:
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
	//  ステージセレクト画面の1-1看板のマップチップ座標を指定
		Vector3 signboardPosition;

		switch (currentStageID_) {
		case 1:
			// ステージ1の看板の位置
			signboardPosition = mapChipField_->GetChipPositionIndex(10, 17);
			break;
		case 2:
			// ステージ2の看板の位置
			signboardPosition = mapChipField_->GetChipPositionIndex(21, 9);
			break;
		case 3:
			// ステージ3の看板の位置
			signboardPosition = mapChipField_->GetChipPositionIndex(36, 9);
			break;
		case 4:
			// ステージ4の看板の位置
			signboardPosition = mapChipField_->GetChipPositionIndex(57, 17);
			break;
		case 5:
			// ステージ5の看板の位置
			signboardPosition = mapChipField_->GetChipPositionIndex(75, 17);
			break;
		default:
			// デフォルト
			signboardPosition = mapChipField_->GetChipPositionIndex(10, 17);
			break;
		}

		// 次のプレイヤー初期位置をGameStateManagerに保存
		GameStateManager::GetInstance()->SetPlayerStartPosition(signboardPosition);
		if (finishedTimer >= 180) {
			finished_ = true;
		}
		break;
	case Phase::GameClear:

		
            // ステージクリア情報を保存
		GameStateManager::GetInstance()->SetStageClear(currentStageID_, true);
		    GameStateManager::GetInstance()->SaveStarCoinRecord(currentStageID_, currentPlayCoinCount_);
		Vector3 returnPos = {};

		switch (currentStageID_) {
		case 1:
			// ステージ1の看板の位置
			returnPos = mapChipField_->GetChipPositionIndex(10, 17);
			break;
		case 2:
			// ステージ2の看板の位置
			returnPos = mapChipField_->GetChipPositionIndex(21, 9);
			break;
		case 3:
			// ステージ3の看板の位置
			returnPos = mapChipField_->GetChipPositionIndex(36, 9);
			break;
		case 4:
			// ステージ4の看板の位置
			returnPos = mapChipField_->GetChipPositionIndex(57, 17);
			break;
		case 5:
			// ステージ5の看板の位置
			returnPos = mapChipField_->GetChipPositionIndex(75, 17);
			break;
		default:
			// デフォルト
			returnPos = mapChipField_->GetChipPositionIndex(10, 17);
			break;
		}

		// 計算した座標を次の初期位置としてセット
		GameStateManager::GetInstance()->SetPlayerStartPosition(returnPos);

		// 次のプレイヤー初期位置をGameStateManagerに保存
		player_->UpdateVictoryAnimation(); // 勝利ポーズのアニメを更新
		CController_->Update();
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			isTimerFinished_ = true;                   // スペースキーが押されたらタイマー終了フラグを立てる
			isSprite = false;                          // スプライト表示
			fade_->Start(Fade::Status::FadeOut, 3.0f); // フェードアウト開始
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

void GameScene::CreateHitEffect(const KamataEngine::Vector3& position) {
	//HitEffect* newHitEffect = HitEffect::create(position); // 新しいヒットエフェクトを生成
	//hitEffects_.push_back(newHitEffect);                   // ヒットエフェクトをリストに追加)
	position;
}

void GameScene::LimitPlayerPosition() 
{
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
GameScene::~GameScene() {
	// 生成したインスタンスの解放
	delete dirtModel_;    // ブロックモデルの解放
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

