#include "GameScene.h" 
#include <iostream>
#include "GameStateManager.h"

using namespace KamataEngine; 

// GameSceneの初期化処理
void GameScene::Initialize(int stageID) {
	currentStageID_ = stageID;
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
	yamaModel = Model::CreateFromOBJ("yama", true); // 山モデルの読み込み
	kinokoModel_ = Model::CreateFromOBJ("kinoko", true); // キノコモデルの読み込み
	enemy_model_Walk = Model::CreateFromOBJ("enemy", true);  // 敵モデルの読み込み
	enemy_model_Shooter = Model::CreateFromOBJ("enemyShooter", true); // 敵モデルの読み込み
	enemy_model_Homing = Model::CreateFromOBJ("enemyHoming", true);  // 敵モデルの読み込み
	iceBlockModel_ = Model::CreateFromOBJ("iceBlock", true);
	umbrellaModel_ = Model::CreateFromOBJ("parasol", true); // 傘モデルの読み込み
	hasigoModel_ = Model::CreateFromOBJ("hasigo", true); // 傘モデルの読み込み
	kumoModel_ = Model::CreateFromOBJ("kumo", true);        // 雲モデルの読み込み
	iwaModel_ = Model::CreateFromOBJ("iwa", true);       // 岩モデルの読み込み
	starCoinModel_ = Model::CreateFromOBJ("koin", true);  // スターコインモデルの読み込み

	// パーティクル用のモデル読み込み
	particleModel_ = Model::CreateFromOBJ("particle", true);

	// 数字表示用テクスチャの読み込み
	textureHandlePhose_ = TextureManager::Load("Phose.png");
	TextureHandleYazirusi_ = TextureManager::Load("yazirusi.png");
	textureHandleEnter_ = TextureManager::Load("enter.png"); // エンターキー用テクスチャの読み込み
	textureHandleGameClearText_ = TextureManager::Load("TextSpriteGameClear.png"); // ゲームクリアテキスト用テクスチャの読み込み
	textureHandlePauseText_ = TextureManager::Load("phoseText.png");               // ポーズテキスト用テクスチャの読み込み
	treeModel_ = Model::CreateFromOBJ("tree", true);                               // 木モデルの読み込
	textureHandle1_1_ = TextureManager::Load("1-1.png");
	textureHandle1_2_ = TextureManager::Load("1-2.png");
	textureHandle1_3_ = TextureManager::Load("1-3.png");


	textureHandleCountdown3_ = TextureManager::Load("3.png");                   // 3の画像
	textureHandleCountdown2_ = TextureManager::Load("2.png");                   // 2の画像
	textureHandleCountdown1_ = TextureManager::Load("1.png");                   // 1の画像
	textureHandleCountdownGo_ = TextureManager::Load("go.png");                 // GOの画像

	//パーティクルの初期化
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
	std::stringstream ss;
	ss << "Resources/map/1-" << stageID << ".csv";
	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv(ss.str()); 
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
	player_->Initialize(playerModel_, &camera_, playerPosition); // <--- ここに見つけた座標が入る

	player_->SetMapChipField(mapChipField_);
	player_->SetisMove(false);
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

	//山の初期化
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
	enterSprite_ = Sprite::Create(textureHandleEnter_, {0.0f, 0.0f}); // エンターキー用スプライトの作成
	pauseTextSprite_ = Sprite::Create(textureHandlePauseText_, {0.0f, 0.0f}); // ポーズメニュー用スプライトの作成
	GameClearTextSprite_ = Sprite::Create(textureHandleGameClearText_, {0.0f, 0.0f}); // ゲームクリアテキスト用スプライトの作成
	spriteCountdown_ = Sprite::Create(textureHandleCountdown3_, {0, 0});              // 初期スプライト（3）
	Vector2 spritePos = {200.0f, 0.0f};
	sprite1_1_ = Sprite::Create(textureHandle1_1_, spritePos);
	sprite1_2_ = Sprite::Create(textureHandle1_2_, spritePos);
	sprite1_3_ = Sprite::Create(textureHandle1_3_, spritePos);
	// HPアイコンのテクスチャを読み込む
	textureHandleHpIconNormal_ = TextureManager::Load("playerAikon.png");
	textureHandleHpIconDamage_ = TextureManager::Load("playerAikonDamage.png");

	// スプライトを生成
	Vector2 hpIconPos = {20.0f, 650.0f}; // 表示したい座標
	spriteHpIconNormal_ = Sprite::Create(textureHandleHpIconNormal_, hpIconPos);
	spriteHpIconDamage_ = Sprite::Create(textureHandleHpIconDamage_, hpIconPos);

	textureHandleHeart_ = TextureManager::Load("playerHp.png");

	Vector2 heartBasePos = {100.0f, 650.0f}; // 1個目のハートの座標
	float heartSpacing = 64.0f;            // ハートとハートの間隔（画像の横幅+隙間）

	// HPの最大値(3回)ループして、ハートのスプライトを生成
	for (int i = 0; i < kMaxPlayerHp; i++) {
		// 表示座標を計算 
		Vector2 pos = {heartBasePos.x + (i * heartSpacing), heartBasePos.y};

		// 満タンハートのスプライトをvectorに追加
		spriteHearts_.push_back(Sprite::Create(textureHandleHeart_, pos));
	}

	texHandleCoinEmpty_ = TextureManager::Load("noKoinUi.png");
	texHandleCoinGet_ = TextureManager::Load("koinUi.png");

	// 1. セーブデータから、このステージのコイン取得数を取得
	currentStarCoinCount_ = GameStateManager::GetInstance()->GetStarCoinRecord(currentStageID_);

	// 2. もし変な値（3以上とか）が入ってたら困るので、一応制限をかけておく
	if (currentStarCoinCount_ > 3) {
		currentStarCoinCount_ = 3;
	}

	// 3. スプライトの生成と配置
	for (int i = 0; i < 3; i++) {
		// 基本は「Empty(未取得)」で作る
		uiStarCoins_[i] = Sprite::Create(texHandleCoinEmpty_, {0, 0});
		uiStarCoins_[i]->SetPosition({30.0f + (i * 50.0f), 30.0f});

		// もし既に持っている枚数分なら、最初から「Get(取得済み)」の画像にする
		if (i < currentStarCoinCount_) {
			uiStarCoins_[i]->SetTextureHandle(texHandleCoinGet_);
		}
	}

	GameStateManager::GetInstance()->SetCurrentStageID(currentStageID_); // ステージ1
	
	isSprite = true;
	firstFrame = true;
	currentPlayCoinCount_ = 0;
	starCoins_.clear();
	currentSelectIndex_ = 0; // 初期選択インデックス
	

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
	ChangePhase();
	// ポーズ状態の切り替え
	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_P)&&!player_->IsDead()&&!isGameClear_) {
		isPaused_ = !isPaused_;
	}

	if (currentSelect_ == PauseSelect::kContinue) {
		yazirusiSprite->SetPosition({180, 190});
	} else if (currentSelect_ == PauseSelect::kStageSelect) {
		yazirusiSprite->SetPosition({190, 320});
	}
	else if (currentSelect_ == PauseSelect::kTitle) {
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
				//  ステージセレクト画面の1-1看板のマップチップ座標を指定
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
	case Phase::kCountdown:
		// このフェーズでカウントダウンの更新処理を行う
		countdownTimer_ += 1.0f / 60.0f; // タイマーを減らす

		// カウントダウンのスプライト切り替え
		if (countdownTimer_ >=0.0f&&countdownTimer_<1.0f) {

			spriteCountdown_ = Sprite::Create(textureHandleCountdown3_, {-250, -100}); // 初期スプライト（3）
		} else if (countdownTimer_ >= 1.0f && countdownTimer_ < 2.0f) {
			spriteCountdown_ = Sprite::Create(textureHandleCountdown2_, {-250, -100}); // 初期スプライト（3）
		} else if (countdownTimer_ >= 2.0f && countdownTimer_ < 3.0f) {
			spriteCountdown_ = Sprite::Create(textureHandleCountdown1_, {-250, -100}); // 初期スプライト（3）
		} else {
			spriteCountdown_ = Sprite::Create(textureHandleCountdownGo_, {-250, -100}); // 初期スプライト（3）
			if (countdownTimer_ >= 3.5f) { // GO!表示後
				countdownState_ = CountdownState::kFinished;  
				phase_ = Phase::kPlay;      // プレイフェーズへ移行
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
	case Phase::kPlay:
		particleManager_->Update();
		player_->Update();
		togeKabe_->Update();
		for (Enemy* enemy : enemys_) {
			enemy->Update();
		}
		//player_->CheckAndResolveTogeKabeCollision(togeKabe_);
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
	// 1. 生まれた子供を一時的に保存するリスト
	std::list<Enemy*> newEnemies;

	// 2. ベクター用の削除ループ
	auto it = enemys_.begin();
	while (it != enemys_.end()) {
		Enemy* enemy = *it;

		// 敵が死んでいるかチェック
		if (enemy->isDead()) {

			if (enemy->GetType() == Enemy::Type::kFlee) {
				// 1. 敵の座標を取得
				Vector3 pos = enemy->GetWorldPosition();

				// 2. その座標に対応するマップチップのインデックスを取得
				// (MapChipField.h にあるこの関数を使います)
				MapChipField::IndexSet index = mapChipField_->GetMapChipIndexSetByPosition(pos);

				// 3. その場所を「スターコイン(99)」に書き換える！
				mapChipField_->SetMapChipType(index.xIndex, index.yIndex, MapChipType::kStarCoin_);
			}




			// 「分裂フラグ」が立っているなら子供を産む
			if (enemy->GetIsSplit()) {
				for (int i = 0; i < 2; i++) {
					// 子供を生成
					Enemy* newEnemy = new Enemy();
					Vector3 pos = enemy->GetWorldPosition();

					newEnemy->Initialize(enemy_model_Walk, &camera_, pos, Enemy::Type::kWalk);

					// 必要な情報をセット
					newEnemy->SetMapChipField(mapChipField_);
					newEnemy->SetPlayer(player_);

					// 子供はもう分裂しないようにする
					newEnemy->SetIsSplit(false);

					// 左右に飛び跳ねさせる
					Vector3 velocity = (i == 0) ? Vector3(-0.1f, 0.5f, 0.0f) : Vector3(0.1f, 0.5f, 0.0f);
					newEnemy->SetVelocity(velocity);

					// 一時リストに追加
					newEnemies.push_back(newEnemy);
				}
			}

			// 親をメモリから削除
			delete enemy;

			// リストから削除し、イテレータを次に進める
			it = enemys_.erase(it);
		} else {
			// 死んでいないなら、次の敵へ
			++it;
		}
	}

	// 3. 生まれた子供たちを本物のリストに合流させる
	for (Enemy* newEnemy : newEnemies) {
		enemys_.push_back(newEnemy);
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
	// ==========================================
	//  デバッグ表示: スターコイン確認用
	// ==========================================
	ImGui::Begin("Star Coin Debug");

	// 1. 今プレイ中の取得数
	ImGui::Text("Current Stage: %d", currentStageID_);
	ImGui::Text("Now Coins (In Game): %d", currentPlayCoinCount_);

	// 2. マネージャーに保存されている「過去の最高記録」
	int savedRecord = GameStateManager::GetInstance()->GetStarCoinRecord(currentStageID_);
	ImGui::Text("Saved Record (DB): %d", savedRecord);

	ImGui::Separator();

	// 3. 全ステージの保存状況確認
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

	for (int i = 0; i < 3; i++) {
		if (uiStarCoins_[i] != nullptr) {
			uiStarCoins_[i]->Draw();
		}
	}

	Sprite::PostDraw();

	if (particleManager_) {
		particleManager_->Draw(dxcommon->GetCommandList());
	}
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
			    type == MapChipType::kFlee) {

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
				} else if (type ==MapChipType::kFlee) {
					newEnemy->Initialize(enemy_model_Walk, &camera_, pos, Enemy::Type::kFlee);
				}

				newEnemy->SetPlayer(player_);
				newEnemy->SetMapChipField(mapChipField_);

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
		Vector3 signboardPosition = mapChipField_->GetChipPositionIndex(10, 17);

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

