#include "GameScene.h" // GameSceneクラスのヘッダーファイルをインクルード

using namespace KamataEngine; // KamataEngine名前空間を使用

// GameSceneの初期化処理
void GameScene::Initialize() {
	// テクスチャのロード
	textureHandel_ = TextureManager::Load("sample.png");

	// モデルのロード
	blockModel_ = Model::CreateFromOBJ("block", true);                 // ブロックモデルの読み込み
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);             // スカイドームモデルの読み込み
	playerModel_ = Model::CreateFromOBJ("player", true);               // プレイヤーモデルの読み込み
	enemy_model_ = Model::CreateFromOBJ("enemy", true);                // 敵モデルの読み込み
	deatparticlesModel_ = Model::CreateFromOBJ("deathParticle", true); // パーティクルモデル

	// カメラの設定と初期化
	camera_.farZ = 1280.0f; // カメラのZ軸方向の最も遠いクリップ面を設定
	camera_.Initialize();   // カメラの初期化

	// デバッグカメラの生成と初期化
	debaucamera_ = new DebugCamera(100, 50); // デバッグカメラのインスタンスを生成
	debaucamera_->SetFarZ(1280.0f);          // デバッグカメラのZ軸方向の最も遠いクリップ面を設定

	// マップチップフィールドの生成とCSVファイルの読み込み
	mapChipField_ = new MapChipField();                    // MapChipFieldのインスタンスを生成
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv"); // マップチップのCSVデータを読み込み

	// プレイヤーの生成と初期化
	player_ = new Player();                                              // Playerのインスタンスを生成
	Vector3 playerPosition = mapChipField_->GetChipPositionIndex(1, 18); // マップチップのインデックスからプレイヤーの初期位置を取得
	player_->Initialize(playerModel_, &camera_, playerPosition);         // プレイヤーを初期化（モデル、カメラ、初期位置を設定）
	player_->SetMapChipField(mapChipField_);                             // プレイヤーにマップチップフィールドを設定
	for (int i = 0; i < kEnemyMax; i++) {
		// 敵クラスの生成
		Enemy* newEnemy = new Enemy(); // Enemyのインスタンスを生成
		// 敵の位置設定と敵クラスの初期化
		Vector3 enemyPosition = mapChipField_->GetChipPositionIndex(20, 12 + i); // マップチップのインデックスから敵の初期位置を取得
		newEnemy->Initialize(enemy_model_, &camera_, enemyPosition);             // 敵を初期化（モデル、カメラ、初期位置を設定）
		enemys_.push_back(newEnemy);
	}
	// パーティクルの生成
	deatparticles_ = new DeathParticles();
	deatparticles_->Initialize(deatparticlesModel_, &camera_, player_, playerPosition);

	// スカイドームの生成と初期化
	skydome_ = new Skydome();                      // Skydomeのインスタンスを生成
	skydome_->Initialize(modelSkydome_, &camera_); // スカイドームを初期化（モデル、カメラを設定）

	// カメラコントローラーの生成と初期化
	CController_ = new CameraController(); // CameraControllerのインスタンスを生成
	CController_->Initialize(&camera_);    // カメラコントローラーを初期化（カメラを設定）
	CController_->SetTarget(player_);      // 追従対象をプレイヤーに設定
	CController_->Reset();                 // カメラコントローラーをリセット

	// カメラの可動範囲を設定
	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f}; // 可動範囲の矩形を定義
	CController_->SetMovableSrea(cameraArea);                             // カメラの可動範囲を設定

	// ブロックの生成
	GenerrateBlock(); // マップチップデータに基づいてブロックを生成

	//ゲームプレイフェーズから開始
	phase_ = Phase::kPlay;
	finishedTimer = 0;

}

// 更新処理
void GameScene::Update() {
	ChangePhase();
	switch (phase_) {
	case Phase::kPlay:
		player_->Update();             // プレイヤーの更新処理
		for (Enemy* enemy : enemys_) { // C++11以降の範囲ベースforループ
			enemy->Update();
		}

		

		CheakAllcollision();
	

		CController_->Update(); // カメラコントローラーの更新処理

		// ブロックのワールド行列を更新し、GPUに転送
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlocks : worldTransformBlockLine) {
				if (!worldTransformBlocks) {
					continue; // nullptrの場合はスキップ
				}
				// ワールド行列を計算（スケール、回転、平行移動）
				worldTransformBlocks->matWorld_ = math->MakeAffineMatrix(worldTransformBlocks->scale_, worldTransformBlocks->rotation_, worldTransformBlocks->translation_);
				worldTransformBlocks->TransferMatrix(); // 行列データをGPUに転送
			}
		}
		debaucamera_->Update(); // デバッグカメラの更新処理

#ifdef _DEBUG // デバッグビルド時のみ有効なコード
		// スペースキーが押されたらデバッグカメラの有効/無効を切り替える
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			if (!isDebugCameraActive_) {
				isDebugCameraActive_ = true;
			} else {
				isDebugCameraActive_ = false;
			}
		}
#endif // !_DEBUG

		// カメラの処理
		if (isDebugCameraActive_) {                                          // デバッグカメラが有効な場合
			camera_.matView = debaucamera_->GetCamera().matView;             // デバッグカメラのビュー行列を設定
			camera_.matProjection = debaucamera_->GetCamera().matProjection; // デバッグカメラの射影行列を設定
			camera_.TransferMatrix();                                        // ビュープロジェクション行列をGPUに転送
		} else {                                                             // 通常カメラが有効な場合
			camera_.UpdateMatrix();                                          // 通常カメラの行列を更新
		}

		skydome_->Update(); // スカイドームの更新処理
		break;

	case Phase::kDeath:
		finishedTimer++;
		for (Enemy* enemy : enemys_) { // C++11以降の範囲ベースforループ
			enemy->Update();
		}

		// ブロックのワールド行列を更新し、GPUに転送
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlocks : worldTransformBlockLine) {
				if (!worldTransformBlocks) {
					continue; // nullptrの場合はスキップ
				}
				// ワールド行列を計算（スケール、回転、平行移動）
				worldTransformBlocks->matWorld_ = math->MakeAffineMatrix(worldTransformBlocks->scale_, worldTransformBlocks->rotation_, worldTransformBlocks->translation_);
				worldTransformBlocks->TransferMatrix(); // 行列データをGPUに転送
			}
		}
		debaucamera_->Update(); // デバッグカメラの更新処理

#ifdef _DEBUG // デバッグビルド時のみ有効なコード
		// スペースキーが押されたらデバッグカメラの有効/無効を切り替える
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			if (!isDebugCameraActive_) {
				isDebugCameraActive_ = true;
			} else {
				isDebugCameraActive_ = false;
			}
		}
#endif // !_DEBUG

		// カメラの処理
		if (isDebugCameraActive_) {                                          // デバッグカメラが有効な場合
			camera_.matView = debaucamera_->GetCamera().matView;             // デバッグカメラのビュー行列を設定
			camera_.matProjection = debaucamera_->GetCamera().matProjection; // デバッグカメラの射影行列を設定
			camera_.TransferMatrix();                                        // ビュープロジェクション行列をGPUに転送
		} else {                                                             // 通常カメラが有効な場合
			camera_.UpdateMatrix();                                          // 通常カメラの行列を更新
		}

		skydome_->Update(); // スカイドームの更新処理
		if (finishedTimer >=120) {
			finished_ = true;
		}
		break;

	}

	
	

}

// 描画処理
void GameScene::Draw() {
	DirectXCommon* dxcommon = DirectXCommon::GetInstance(); // DirectXCommonのインスタンスを取得
	switch (phase_) {
	case Phase::kPlay:
		Model::PreDraw(dxcommon->GetCommandList()); // モデル描画の前処理（コマンドリストの設定など）

		player_->Draw(); // プレイヤーの描画処理

		for (Enemy* enemy : enemys_) { // C++11以降の範囲ベースforループ
			enemy->Draw();
		}
		deatparticles_->Draw();

		// ブロックの描画
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlocks : worldTransformBlockLine) {
				if (!worldTransformBlocks) {
					continue; // nullptrの場合はスキップ
				}
				blockModel_->Draw(*worldTransformBlocks, camera_); // ブロックモデルを描画
			}
		}

		skydome_->Draw(); // スカイドームの描画処理

		Model::PostDraw(); // モデル描画の後処理
		break;
	case Phase::kDeath:
		Model::PreDraw(dxcommon->GetCommandList()); // モデル描画の前処理（コマンドリストの設定など）



		for (Enemy* enemy : enemys_) { // C++11以降の範囲ベースforループ
			enemy->Draw();
		}
		deatparticles_->Draw();

		// ブロックの描画
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlocks : worldTransformBlockLine) {
				if (!worldTransformBlocks) {
					continue; // nullptrの場合はスキップ
				}
				blockModel_->Draw(*worldTransformBlocks, camera_); // ブロックモデルを描画
			}
		}

		skydome_->Draw(); // スカイドームの描画処理

		Model::PostDraw(); // モデル描画の後処理
		break;
	default:
		break;
	}


}

// ブロックの生成処理
void GameScene::GenerrateBlock() {
	const uint32_t kNumBlockVirtal = mapChipField_->GetNumBlockVirtcal();      // マップチップの垂直方向のブロック数を取得
	const uint32_t kNumBlockHorizotal = mapChipField_->GetNumBlockHorizonal(); // マップチップの水平方向のブロック数を取得

	worldTransformBlocks_.resize(kNumBlockVirtal); // 垂直方向のサイズにリサイズ
	for (uint32_t i = 0; i < kNumBlockVirtal; ++i) {
		worldTransformBlocks_[i].resize(kNumBlockHorizotal); // 水平方向のサイズにリサイズ
	}

	// マップチップのデータに基づいてブロックを配置
	for (uint32_t i = 0; i < kNumBlockVirtal; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizotal; ++j) {
			// マップチップのタイプがブロックの場合
			if (mapChipField_->GetMapChipTypeByindex(j, i) == MapChipType::kBlock_) {
				WorldTransform* worldTransform = new WorldTransform();                                 // 新しいWorldTransformインスタンスを生成
				worldTransform->Initialize();                                                          // 初期化
				worldTransformBlocks_[i][j] = worldTransform;                                          // 配列に格納
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetChipPositionIndex(j, i); // ブロックのワールド座標を設定
			}
		}
	}
}

void GameScene::CheakAllcollision() {
	// 判定対象１と２の座標
	AABB aabb1, aabb2;
	aabb1 = player_->GetAABB();
	for (Enemy* enemy : enemys_) {
		aabb2 = enemy->GetAABB();

		// AABB同士の考査判定
		if (math->IsCollision(aabb1, aabb2)) {
			player_->OnCollision(enemy);
			enemy->onCollision(player_);
		}
	}
}

void GameScene::ChangePhase() 
{
	switch (phase_) {
	case Phase::kPlay:
		if (player_->IsDead()) {
			// 死亡演出フェーズに切り替え
			phase_ = Phase::kDeath;
			const Vector3& deatParticlesPosition = player_->GetWorldPosition();
			deatparticles_->Initialize(deatparticlesModel_, &camera_, player_, deatParticlesPosition);
		

		}

		break;
	case Phase::kDeath:
		deatparticles_->Update();
		break;

	}
}

// デストラクタ
GameScene::~GameScene() {
	// 生成したインスタンスの解放
	delete blockModel_;   // ブロックモデルの解放
	delete debaucamera_;  // デバッグカメラの解放
	delete modelSkydome_; // スカイドームモデルの解放
	delete player_;       // プレイヤーの解放
	delete mapChipField_; // マップチップフィールドの解放
	delete deatparticles_;//パーティクルの解放

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