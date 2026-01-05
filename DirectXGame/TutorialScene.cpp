#include"TutorialScene.h"
#include "GameStateManager.h"
#include <iostream>

using namespace KamataEngine;

void TutorialScene::Initialize() {
	//========================
	// 📦 リソースの読み込み
	//========================
	textureHandel_ = TextureManager::Load("sample.png");
	dirtModel_ = Model::CreateFromOBJ("tutiBlock", true);
	grassModel_ = Model::CreateFromOBJ("tutiBlockSibahu", true);
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	playerModel_ = Model::CreateFromOBJ("player", true);
	deatparticlesModel_ = Model::CreateFromOBJ("deathParticle", true);
	breakableBlockModel_ = Model::CreateFromOBJ("Breakable", true); // 破壊可能ブロックのモデル読み込み
	hitEffectModel_ = Model::CreateFromOBJ("hit", true);
	goalModel_ = Model::CreateFromOBJ("gorl", true);
	GameClearTextModel_ = Model::CreateFromOBJ("GameClear", true);
	togeKabeModel_ = Model::CreateFromOBJ("kabeToge", true);
	togeModel_ = Model::CreateFromOBJ("toge", true);
	yamaModel = Model::CreateFromOBJ("yama", true);                   // 山モデルの読み込み
	kinokoModel_ = Model::CreateFromOBJ("kinoko", true);              // キノコモデルの読み込み
	enemy_model_Walk = Model::CreateFromOBJ("enemy", true);           // 敵モデルの読み込み
	enemy_model_Shooter = Model::CreateFromOBJ("enemyShooter", true); // 敵モデルの読み込み
	enemy_model_Homing = Model::CreateFromOBJ("enemyHoming", true);   // 敵モデルの読み込み
	iceBlockModel_ = Model::CreateFromOBJ("iceBlock", true);
	umbrellaModel_ = Model::CreateFromOBJ("parasol", true); // 傘モデルの読み込み
	hasigoModel_ = Model::CreateFromOBJ("hasigo", true);    // 傘モデルの読み込み
	kumoModel_ = Model::CreateFromOBJ("kumo", true);        // 雲モデルの読み込み
	iwaModel_ = Model::CreateFromOBJ("iwa", true);          // 岩モデルの読み込み

	// パーティクル用のモデル読み込み
	particleModel_ = Model::CreateFromOBJ("particle", true);

	// 数字表示用テクスチャの読み込み
	textureHandlePhose_ = TextureManager::Load("Phose.png");
	TextureHandleYazirusi_ = TextureManager::Load("yazirusi.png");
	textureHandleEnter_ = TextureManager::Load("enter.png");                       // エンターキー用テクスチャの読み込み
	textureHandleGameClearText_ = TextureManager::Load("TextSpriteGameClear.png"); // ゲームクリアテキスト用テクスチャの読み込み
	textureHandlePauseText_ = TextureManager::Load("phoseText.png");               // ポーズテキスト用テクスチャの読み込み
	treeModel_ = Model::CreateFromOBJ("tree", true);
	// チュートリアル用テクスチャの読み込み
	uiMoveHandle_ = TextureManager::Load("wdMoveText.png");                             // "移動してみよう"
	uiJumpHandle_ = TextureManager::Load("spaceMoveText.png");                             // "ジャンプしてみよう"
	uiGlideHandle_ = TextureManager::Load("spacePrasplMoveText.png");                            // "滑空してみよう"
	uiAttackHandle_ = TextureManager::Load("FKeyslShotText.png");                           // "攻撃してみよう"
	uiFinishHandle_ = TextureManager::Load("tutorialWinText.png");                           // "おつかれさま！"



	// パーティクルの初期化
	particleManager_ = new ParticleManager();
	particleManager_->Initialize(particleModel_, &camera_);

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
	mapChipField_->LoadMapChipCsv("Resources/map/heimen.csv");

	CController_ = new CameraController();
	CController_->Initialize(&camera_);
	//========================
	// 🧍 プレイヤーの初期化
	//========================
	// 1. まずデフォルトの位置を決めておく（万が一90番が見つからなかった時の保険）
	Vector3 playerPosition = mapChipField_->GetChipPositionIndex(3, 2);

	// 2. マップ全体を調べて「kPlayerStart_ (90)」を探す
	for (uint32_t y = 0; y < mapChipField_->GetNumBlockVirtcal(); ++y) {
		for (uint32_t x = 0; x < mapChipField_->GetNumBlockHorizonal(); ++x) {

			// もし「プレイヤーのスタート地点(90)」なら
			if (mapChipField_->GetMapChipTypeByindex(x, y) == MapChipType::kPlayerStart_) {
				// その座標を取得して playerPosition を上書き！
				playerPosition = mapChipField_->GetChipPositionIndex(x, y);

				// ：見つけた後は、その場所を「空白(0)」に戻しておく
				mapChipField_->SetMapChipType(x, y, MapChipType::kBlank_);
			}
		}
	}

	// 3. 決定した座標を使って初期化
	player_ = new Player();
	player_->Initialize(playerModel_, &camera_, playerPosition); 

	player_->SetMapChipField(mapChipField_);
	player_->SetisMove(true);
	player_->SetParticleManager(particleManager_);
	player_->SetUmbrellaModel(umbrellaModel_);
	player_->SetCameraController(CController_);

	//========================
	// 🎮 カメラコントローラー
	//========================

	CController_->SetTarget(player_);
	CController_->Reset();
	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 40.0f};
	CController_->SetMovableSrea(cameraArea);

	//========================
	// 💥 ヒットエフェクト設定
	//========================
	HitEffect::SetModel(hitEffectModel_);
	HitEffect::SetCamera(&camera_);


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

	// 山の初期化
	yama_.push_back(new Yama());
	yama_.back()->Initialize(yamaModel, &camera_, mapChipField_->GetChipPositionIndex(20, 19));
	yama_.push_back(new Yama());
	yama_.back()->Initialize(yamaModel, &camera_, mapChipField_->GetChipPositionIndex(70, 19));

	// 木の初期化
	tree_.push_back(new Tree());
	tree_.back()->Initialize(treeModel_, &camera_, mapChipField_->GetChipPositionIndex(5, 19));
	tree_.push_back(new Tree());
	tree_.back()->Initialize(treeModel_, &camera_, mapChipField_->GetChipPositionIndex(90, 19));

	// spriteの初期化
	TextSprite1_1 = Sprite::Create(textureHandle, {100.50});
	poseSprite = Sprite::Create(textureHandlePhose_, {0.0});
	yazirusiSprite = Sprite::Create(TextureHandleYazirusi_, {180, 190});
	enterSprite_ = Sprite::Create(textureHandleEnter_, {0.0f, 0.0f});                 // エンターキー用スプライトの作成
	pauseTextSprite_ = Sprite::Create(textureHandlePauseText_, {0.0f, 0.0f});         // ポーズメニュー用スプライトの作成
	GameClearTextSprite_ = Sprite::Create(textureHandleGameClearText_, {0.0f, 0.0f}); // ゲームクリアテキスト用スプライトの作成
	spriteCountdown_ = Sprite::Create(textureHandleCountdown3_, {0, 0});              // 初期スプライト（3）
	Vector2 spritePos = {200.0f, 0.0f};


	float uiX = 1280.0f / 2.0f - 200.0f; // 画面中央あたり
	float uiY = 300.0f;                  // 上の方

	// 1. 移動 (白)
	uiMove_ = Sprite::Create(uiMoveHandle_, {uiX, uiY});

	// 2. ジャンプ (赤)
	uiJump_ = Sprite::Create(uiJumpHandle_, {uiX, uiY});

	// 3. 滑空 (青)
	uiGlide_ = Sprite::Create(uiGlideHandle_, {uiX, uiY});


	// 4. 攻撃 (黄色)
	uiAttack_ = Sprite::Create(uiAttackHandle_, {uiX, uiY});


	// 5. 終了 (緑)
	uiFinish_ = Sprite::Create(uiFinishHandle_, {uiX, uiY});


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

	GameStateManager::GetInstance()->SetCurrentStageID(currentStageID_); // ステージ1

	isSprite = true;
	firstFrame = true;
	currentSelectIndex_ = 0; // 初期選択インデックス
}

// ==============================
// 更新処理
// ==============================
void TutorialScene::Update() {
	// フェードの更新 & フェーズ管理
	fade_->Update();
	LimitPlayerPosition();
	for (Yama* yama : yama_) {
		yama->Update();
	}
	for (Tree* tree : tree_) {
		tree->Update();
	}
	ChangePhase();
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

		// 決定（Spaceキー）
		if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			if (currentSelect_ == PauseSelect::kContinue) {
				isPaused_ = false; // ポーズを解除
			} else if (currentSelect_ == PauseSelect::kStageSelect) {
				// ★ 修正: ステージセレクト画面の1-1看板のマップチップ座標を指定
				Vector3 signboardPosition = mapChipField_->GetChipPositionIndex(10, 17);

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

		togeKabe_->Update();
		// 敵の更新
		for (Enemy* enemy : enemys_) {
			enemy->Update();
		}

		// 当たり判定
		CheekAllcollision();

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
		if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_E)) {
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

	// ------------------------------
	// プレイ中の処理
	// ------------------------------
	case Phase::kPlay:
		particleManager_->Update();
		player_->Update();
		togeKabe_->Update();
		for (Enemy* enemy : enemys_) {
			enemy->Update();
		}
		// player_->CheckAndResolveTogeKabeCollision(togeKabe_);
		CheekAllcollision();
		CController_->Update();
		UpdateTutorialLogic();
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
					floor->OnStepped(); // ← 踏んだその1枚だけ開始
					break;              // 見つかったら終わり
				}
			}
		}

		break;

	// ------------------------------
	// 死亡中の処理（プレイヤー死亡）
	// ------------------------------
	case Phase::kDeath:

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
		// 壊れる床の更新
		for (auto it = breakableFloors_.begin(); it != breakableFloors_.end();) {
			(*it)->Update();
			if ((*it)->IsBroken()) {
				it = breakableFloors_.erase(it); // 消滅したらリストから消す
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

#ifdef _DEBUG
	ImGui::Begin("Tutorial Debug");
	ImGui::Text("Phase: %d", (int)phase_);

	// 現在のステップを文字で表示
	const char* stepName = "Unknown";
	switch (tutorialStep_) {
	case TutorialStep::kWelcome:
		stepName = "Welcome (Wait)";
		break;
	case TutorialStep::kMove:
		stepName = "Move (Press A/D)";
		break;
	case TutorialStep::kJump:
		stepName = "Jump (Press Space)";
		break;
	case TutorialStep::kGlide:
		stepName = "Glide (Hold Space)";
		break;
	case TutorialStep::kAttack:
		stepName = "Attack (Shoot Bullet)";
		break;
	case TutorialStep::kFinish:
		stepName = "Finish (Wait)";
		break;
	}
	ImGui::Text("Step: %s", stepName);

	// タイマー等の確認
	ImGui::Text("Timer: %d", stepTimer_);
	ImGui::Text("ActionCount: %d", actionCount_);
	ImGui::End();
#endif
}
void TutorialScene::Draw() {
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
	switch (tutorialStep_) {
	case TutorialStep::kMove:
		if (uiMove_)
			uiMove_->Draw();
		break;
	case TutorialStep::kJump:
		if (uiJump_)
			uiJump_->Draw();
		break;
	case TutorialStep::kGlide:
		if (uiGlide_)
			uiGlide_->Draw();
		break;
	case TutorialStep::kAttack:
		if (uiAttack_)
			uiAttack_->Draw();
		break;
	case TutorialStep::kFinish:
		if (uiFinish_)
			uiFinish_->Draw();
		break;
	}
	Sprite::PostDraw();

	if (particleManager_) {
		particleManager_->Draw(dxcommon->GetCommandList());
	}
}

void TutorialScene::GenerrateBlock() {
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

void TutorialScene::CheekAllcollision() {
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
				enemy->TakeDamage(999);
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

					isGameClear_ = true;
					player_->StartVictoryPose(); // 勝利ポーズ開始
					CController_->StartVictoryZoom(player_);
					// CController_->StartVictoryZoom(player_);
				} else if (type == MapChipType::kSpike_) {
					// 棘のダメージ処理
					player_->TakeDamage(1);
				}
			}
		}
	}
}

void TutorialScene::ChangePhase() {
	switch (phase_) {
	case Phase::kFadeIn:
		if (fade_->isFinished()) {
			countdownState_ = CountdownState::kCounting;
			phase_ = Phase::kPlay; // フェードインが完了したらカウントダウンフェーズへ
			tutorialStep_ = TutorialStep::kWelcome;
			stepTimer_ = 0;
			isEnemySpawned_ = false;
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

	case Phase::GameClear:

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

void TutorialScene::CreateHitEffect(const KamataEngine::Vector3& position) {
	// HitEffect* newHitEffect = HitEffect::create(position); // 新しいヒットエフェクトを生成
	// hitEffects_.push_back(newHitEffect);                   // ヒットエフェクトをリストに追加)
	position;
}

void TutorialScene::LimitPlayerPosition() {
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
TutorialScene::~TutorialScene() {
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

void TutorialScene::UpdateTutorialLogic() {
	using namespace KamataEngine; // Inputを使うため

	switch (tutorialStep_) {
	case TutorialStep::kWelcome:
		// 少し待ってから開始
		stepTimer_++;
		if (stepTimer_ > 60) {
			tutorialStep_ = TutorialStep::kMove;
			stepTimer_ = 0;
		}
		break;

	case TutorialStep::kMove:
		// 移動入力チェック
		if (Input::GetInstance()->PushKey(DIK_D) || Input::GetInstance()->PushKey(DIK_A)) {
			stepTimer_++;
			if (stepTimer_ > 60) {
				tutorialStep_ = TutorialStep::kJump;
				stepTimer_ = 0;
				actionCount_ = 0;
			}
		}
		break;

	case TutorialStep::kJump:
		// ジャンプ入力チェック
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			actionCount_++;
		}
		if (actionCount_ >= 2) {
			tutorialStep_ = TutorialStep::kGlide;
			stepTimer_ = 0;
		}
		break;

	case TutorialStep::kGlide:
		// 滑空チェック（空中でスペース長押し）
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			stepTimer_++;
			if (stepTimer_ > 30) {
				tutorialStep_ = TutorialStep::kAttack;
				stepTimer_ = 0;
			}
		}
		break;


	case TutorialStep::kAttack:

		if (!player_->GetBullets().empty()) {

			stepTimer_++;
			if (stepTimer_ > 30) {
				tutorialStep_ = TutorialStep::kFinish;
				stepTimer_ = 0;
			}
		}
		break;

	case TutorialStep::kFinish:
		stepTimer_++;
		if (stepTimer_ > 120) {
			// フェードアウトさせてタイトルへ
			fade_->Start(Fade::Status::FadeOut, 2.0f);
			phase_ = Phase::kFadeOut;
			finished_ = true;
		}
		break;
	}
}