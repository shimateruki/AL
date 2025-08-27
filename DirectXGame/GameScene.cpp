#include "GameScene.h" // GameSceneクラスのヘッダーファイルをインクルード
#include <iostream>
#include "GameStateManager.h"

using namespace KamataEngine; // KamataEngine名前空間を使用

// GameSceneの初期化処理
void GameScene::Initialize() {
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
	textureHandle = TextureManager::Load("1-1.png");
	yamaModel = Model::CreateFromOBJ("yama", true); // 山モデルの読み込み
	// 数字表示用テクスチャの読み込み
	textureHandlePhose_ = TextureManager::Load("Phose.png");
	TextureHandleYazirusi_ = TextureManager::Load("yazirusi.png");
	textureHandleEnter_ = TextureManager::Load("enter.png"); // エンターキー用テクスチャの読み込み
	textureHandleGameClearText_ = TextureManager::Load("TextSpriteGameClear.png"); // ゲームクリアテキスト用テクスチャの読み込み
	textureHandlePauseText_ = TextureManager::Load("phoseText.png");               // ポーズテキスト用テクスチャの読み込み
	treeModel_ = Model::CreateFromOBJ("tree", true);                               // 木モデルの読み込み

	
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
	mapChipField_->LoadMapChipCsv("Resources/1-1.csv");

	//========================
	// 🧍 プレイヤーの初期化
	//========================
	player_ = new Player();
	Vector3 playerPosition = mapChipField_->GetChipPositionIndex(3, 16);
	player_->Initialize(playerModel_, &camera_, playerPosition, playerAttackModel_);
	player_->SetMapChipField(mapChipField_);

	//========================
	// 💥 ヒットエフェクト設定
	//========================
	HitEffect::SetModel(hitEffectModel_);
	HitEffect::SetCamera(&camera_);

	//========================
	// 👾 敵の初期化
	//========================
	for (int i = 0; i < kEnemyMax; i++) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetChipPositionIndex(20, 12 + i);
		newEnemy->Initialize(enemy_model_, &camera_, enemyPosition);
		newEnemy->SetGameScene(this);
		enemys_.push_back(newEnemy);
	}

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


	yama_.push_back(new Yama());
	yama_.back()->Initialize(yamaModel, &camera_, mapChipField_->GetChipPositionIndex(20, 19));

	yama_.push_back(new Yama());
	yama_.back()->Initialize(yamaModel, &camera_, mapChipField_->GetChipPositionIndex(70, 19));

	
	tree_.push_back(new Tree());
	tree_.back()->Initialize(treeModel_, &camera_, mapChipField_->GetChipPositionIndex(5, 19));
	tree_.push_back(new Tree());
	tree_.back()->Initialize(treeModel_, &camera_, mapChipField_->GetChipPositionIndex(90, 19));



	Audio::GetInstance()->Initialize("Resources/BGM/");

	// BGMの読み込み（.wavファイル）
	bgmHandle_ = KamataEngine::Audio::GetInstance()->LoadWave("Clear1.wav");

	TextSprite1_1 = Sprite::Create(textureHandle, {100.50});
	poseSprite = Sprite::Create(textureHandlePhose_, {0.0});
	yazirusiSprite = Sprite::Create(TextureHandleYazirusi_, {180, 190});
	enterSprite_ = Sprite::Create(textureHandleEnter_, {0.0f, 0.0f}); // エンターキー用スプライトの作成
	pauseTextSprite_ = Sprite::Create(textureHandlePauseText_, {0.0f, 0.0f}); // ポーズメニュー用スプライトの作成
	// ゲームクリアテキストスプライトの作成
	GameClearTextSprite_ = Sprite::Create(textureHandleGameClearText_, {0.0f, 0.0f}); // ゲームクリアテキスト用スプライトの作成


	GameStateManager::GetInstance()->SetCurrentStageID(1); // ステージ1

	isSprite = true;
	firstFrame = true;
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

		// 決定（Enterキー）
		if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_RETURN)) {
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

		player_->Update();
		togeKabe_->Update();
		for (Enemy* enemy : enemys_) {
			enemy->Update();
		}

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

	// 死亡した敵の削除
	enemys_.remove_if([](Enemy* enemy) {
		if (enemy->GetIsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});
}
void GameScene::Draw() {
	DirectXCommon* dxcommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxcommon->GetCommandList());

	// 👾 敵の描画
	//for (Enemy* enemy : enemys_)
		//enemy->Draw();




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
	if (isSprite&&!isGameClear_&&!isPaused_) {
		TextSprite1_1->Draw();
		pauseTextSprite_->Draw(); // ポーズスプライトの描画
	}

	if (isPaused_) {
		poseSprite->Draw();
		yazirusiSprite->Draw();
		enterSprite_->Draw(); // エンターキー用スプライトの描画
	}

	// ゲームクリアテキストスプライトの描画
	if (isGameClear_&&isSprite) {
		GameClearTextSprite_->Draw();
		enterSprite_->Draw(); // エンターキー用スプライトの描画

	}
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

			// 何らかの描画が必要なブロックのみWorldTransformを生成
			if (type != MapChipType::kBlank_) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransform->translation_ = mapChipField_->GetChipPositionIndex(x, y);
				worldTransformBlocks_[y][x] = worldTransform;
			}
		}
	}
}

void GameScene::CheekAllcollision() {
	// プレイヤーAABB
	AABB aabb1 = player_->GetAABB();

	//// ==== 敵との当たり判定 ====
	//for (Enemy* enemy : enemys_) {
	//	if (enemy->isCollisonDisabled()) {
	//		continue; // 当たり判定が無効な敵はスキップ
	//	}
	//	AABB aabb2 = enemy->GetAABB();
	//	if (math->IsCollision(aabb1, aabb2)) {
	//		if (!player_->GetIsAttack()) {
	//			player_->OnCollision(enemy);
	//		}
	//		enemy->onCollision(player_);
	//	}
	//}
	AABB aabb3 = togeKabe_->GetAABB();
	if (math->IsCollision(aabb1, aabb3)) {
		player_->SetIsDead(true);
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
					bgmVoiceHandle_ = KamataEngine::Audio::GetInstance()->PlayWave(bgmHandle_, false, 0.5f);
					isGameClear_ = true;
				} else if (type == MapChipType::kSpike_) {
					// 棘のダメージ処理
					player_->SetIsDead(true); // プレイヤーを死亡状態に設定
				}
			}
		}
	}


}

void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kFadeIn:
		if (fade_->isFinished()) {
			phase_ = Phase::kPlay;
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
		Vector3 signboardPosition = mapChipField_->GetChipPositionIndex(10, 17);

		// 次のプレイヤー初期位置をGameStateManagerに保存
		GameStateManager::GetInstance()->SetPlayerStartPosition(signboardPosition);
		if (finishedTimer >= 180) {
			finished_ = true;
		}
		break;
	case Phase::GameClear:


	

		
            // ステージクリア情報を保存
		GameStateManager::GetInstance()->SetStageClear(1, true);

		// ★ 修正: ステージセレクト画面の1-1看板のマップチップ座標を指定
	    signboardPosition = mapChipField_->GetChipPositionIndex(10, 17);

		// 次のプレイヤー初期位置をGameStateManagerに保存
		GameStateManager::GetInstance()->SetPlayerStartPosition(signboardPosition);

		if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
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
	HitEffect* newHitEffect = HitEffect::create(position); // 新しいヒットエフェクトを生成
	hitEffects_.push_back(newHitEffect);                   // ヒットエフェクトをリストに追加)
	                                                       // ヒットエフェクトの数が最大数を超えた場合、最も古いものを削除
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