#include "StageSelectGameScene.h"
#include "GameStateManager.h"
using namespace KamataEngine;
void StageSelectGameScene::Initialize() {
	dirtModel_ = Model::CreateFromOBJ("tutiBlock", true);
	grassModel_ = Model::CreateFromOBJ("tutiBlockSibahu", true);
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	playerModel_ = Model::CreateFromOBJ("player", true);
	signboardModel_ = Model::CreateFromOBJ("kanban", true); // 看板モデルの読み込み
	WModel = Model::CreateFromOBJ("W", true);               // Wモデルの読み込み
	keyHeimenModel = Model::CreateFromOBJ("keyHeimen", true);           // キーモデルの読み込み
	// ゲームクリアテキストモデルの読み込み
	TitleTextModel = Model::CreateFromOBJ("TitleText", true); // タイトルテキストモデルの読み込み
	// ステージの看板に貼り付けるテキストモデルの読み込み
	Textmodel1_1 = Model::CreateFromOBJ("Text1-1", true);
	Textmodel1_2 = Model::CreateFromOBJ("Text1-2", true);  
	Textmodel1_3 = Model::CreateFromOBJ("Text1-3", true);
	Textmodel1_4 = Model::CreateFromOBJ("Text1-4", true);  

	umbrellaModel_ = Model::CreateFromOBJ("parasol", true);
	yamaModel = Model::CreateFromOBJ("yama", true);         // 山モデルの読み込み
	hasigoModel_ = Model::CreateFromOBJ("hasigo", true); // 傘モデルの読み込み
	kumoModel_ = Model::CreateFromOBJ("kumo", true);     // 雲モデルの読み込み
	kinokoModel_ = Model::CreateFromOBJ("kinoko", true); // キノコモデルの読み込み
	starCoinModel_ = Model::CreateFromOBJ("koin", true); // スターコインモデルの読み込み


	textureHandel1_1 = TextureManager::Load("1-1.png"); // 数字表示用テクスチャの読み込み
	textureHandel1_2 = TextureManager::Load("1-2.png"); // 数字表示用テクスチャの読み込み
	textureHandel1_3 = TextureManager::Load("1-3.png"); // 数字表示用テクスチャの読み込み
	textureHandel1_4 = TextureManager::Load("1-4.png"); // 数字表示用テクスチャの読み込み
	textureHandleMove = TextureManager::Load("TextSpriteMove.png"); // 移動用テクスチャの読み込み
	textureHandleJump = TextureManager::Load("TextSpriteSpaceJump.png"); // ジャンプ用テクスチャの読み込み
	textureHandleSutage = TextureManager::Load("SutageSelectTextUi.png"); // ステージ用テクスチャの読み込み
	treeModel_ = Model::CreateFromOBJ("tree", true);                     // 木モデルの読み込み






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
	mapChipField_->LoadMapChipCsv("Resources/map/stageSelect.csv");

	//========================
	// 🧍 プレイヤーの初期化
	//========================
	player_ = new Player();
	Vector3 playerPosition = GameStateManager::GetInstance()->GetPlayerStartPosition();
	player_->Initialize(playerModel_, &camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);
	player_->SetisMove(true);
	player_->SetUmbrellaModel(umbrellaModel_);


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
	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 40.0f};
	CController_->SetMovableSrea(cameraArea);
	//========================
	// 🧱 ブロック生成
	//========================

	GenerateBlocks();

	//========================
	// 🌗 フェード処理の初期化
	//========================
	fade_ = new Fade();
	fade_->Initalize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);
	finishedTimer = 0;

	   // 複数配置
	   
	//看板
	//タイトル
	signboards_.push_back(new Signboard());
	signboards_.back()->Initialize(signboardModel_, &camera_, mapChipField_->GetChipPositionIndex(3, 17), 0);
	spriteTexts_.push_back(new SpriteText());
	spriteTexts_.back()->Initialize(TitleTextModel, &camera_, mapChipField_->GetChipPositionIndex(3, 17));

	


	// 1-1
	signboards_.push_back(new Signboard());
	signboards_.back()->Initialize(signboardModel_, &camera_, mapChipField_->GetChipPositionIndex(10, 17), 1);
	spriteTexts_.push_back(new SpriteText());
	spriteTexts_.back()->Initialize(Textmodel1_1, &camera_, mapChipField_->GetChipPositionIndex(10, 17));
	//  クリア済みの場合は色を青色に変更
	if (GameStateManager::GetInstance()->IsStageClear(1)) {
		spriteTexts_.back()->SetColor({0.0f, 0.0f, 1.0f, 1.0f}); // 青色に変更
	}

		keySprite_.push_back(new keyModel());
	keySprite_.back()->Initialize(keyHeimenModel, &camera_, mapChipField_->GetChipPositionIndex(10, 15));
	keySprite_.back()->SetColor({0.0f, 0.0f, 0.0f, 1.0f}); // 黒色に設定
	keySprite_.push_back(new keyModel());
	keySprite_.back()->Initialize(WModel, &camera_, mapChipField_->GetChipPositionIndex(10, 15));



	// 1-2
	signboards_.push_back(new Signboard());
	signboards_.back()->Initialize(signboardModel_, &camera_, mapChipField_->GetChipPositionIndex(21, 9), 2);
	spriteTexts_.push_back(new SpriteText());
	spriteTexts_.back()->Initialize(Textmodel1_2, &camera_, mapChipField_->GetChipPositionIndex(21, 9));
	//  クリア済みの場合は色を青色に変更
	if (GameStateManager::GetInstance()->IsStageClear(2)) {
		spriteTexts_.back()->SetColor({0.0f, 0.0f, 1.0f, 1.0f}); // 青色に変更
	}



	// 1-3
	signboards_.push_back(new Signboard());
	signboards_.back()->Initialize(signboardModel_, &camera_, mapChipField_->GetChipPositionIndex(36, 9), 3);
	spriteTexts_.push_back(new SpriteText());
	spriteTexts_.back()->Initialize(Textmodel1_3, &camera_, mapChipField_->GetChipPositionIndex(36, 9));
	//  クリア済みの場合は色を青色に変更
	if (GameStateManager::GetInstance()->IsStageClear(3)) {
		spriteTexts_.back()->SetColor({0.0f, 0.0f, 1.0f, 1.0f}); // 青色に変更
	}
	//1-4
	signboards_.push_back(new Signboard());
	signboards_.back()->Initialize(signboardModel_, &camera_, mapChipField_->GetChipPositionIndex(57, 17), 4);
	spriteTexts_.push_back(new SpriteText());
	spriteTexts_.back()->Initialize(Textmodel1_4, &camera_, mapChipField_->GetChipPositionIndex(57, 17));
	//  クリア済みの場合は色を青色に変更
	if (GameStateManager::GetInstance()->IsStageClear(4)) {
		spriteTexts_.back()->SetColor({0.0f, 0.0f, 1.0f, 1.0f}); // 青色に変更
	}

	//// 2-1
	//signboards_.push_back(new Signboard());
	//signboards_.back()->Initialize(signboardModel_, &camera_, mapChipField_->GetChipPositionIndex(40, 17), 4);

	yama_.push_back(new Yama());
	yama_.back()->Initialize(yamaModel, &camera_, mapChipField_->GetChipPositionIndex(20, 19));


	yama_.push_back(new Yama());
	yama_.back()->Initialize(yamaModel, &camera_, mapChipField_->GetChipPositionIndex(70, 19));

	tree_.push_back(new Tree());
	tree_.back()->Initialize(treeModel_, &camera_, mapChipField_->GetChipPositionIndex(25, 19));
	tree_.push_back(new Tree());
	tree_.back()->Initialize(treeModel_, &camera_, mapChipField_->GetChipPositionIndex(45, 19));
	tree_.push_back(new Tree());
	tree_.back()->Initialize(treeModel_, &camera_, mapChipField_->GetChipPositionIndex(60, 19));
	tree_.push_back(new Tree());
	tree_.back()->Initialize(treeModel_, &camera_, mapChipField_->GetChipPositionIndex(80, 19));


    
	Sprite1_1 = Sprite::Create(textureHandel1_1, {800.50}); // 数字表示用スプライトの作成
	Sprite1_2 = Sprite::Create(textureHandel1_2, {800.50}); // 数字表示用スプライトの作成
	Sprite1_3 = Sprite::Create(textureHandel1_3, {800.50}); // 数字表示用スプライトの作成
	Sprite1_4 = Sprite::Create(textureHandel1_4, {800.50}); // 数字表示用スプライトの作成
	SpriteSutage = Sprite::Create(textureHandleSutage, {0.0}); // ステージ用スプライトの作成

	// スターコインUIの初期化
	// 画像の読み込み 
	texHandleCoinEmpty_ = TextureManager::Load("noKoinUi.png");
	texHandleCoinGet_ = TextureManager::Load("koinUi.png");

	// スプライト生成と配置（左上に並べる）
	for (int i = 0; i < 3; i++) {
		uiStarCoins_[i] = Sprite::Create(texHandleCoinEmpty_, {0, 0});
		uiStarCoins_[i]->SetPosition({30.0f + (i * 50.0f), 130.0f});
	}

	// 最初は非表示
	isShowCoinUI_ = false;


	firstFrame = true; // 初回フレームフラグを設定
	isCoinsSetup_ = false;
	isSprite = true;

}

void StageSelectGameScene::LimitPlayerPosition() {
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


void StageSelectGameScene::Update() {
	// フェードの更新 & フェーズ管理
	fade_->Update();
	ChangePhase();
	LimitPlayerPosition();
	CheekAllcollision();
	for ( Yama *yama :yama_){
		yama->Update();
	}
	for (Tree * tree:tree_){
		tree->Update();
	}

	#ifdef _DEBUG
	ImGui::Begin("StageSelectGameScene Debug Info"); // ImGuiのデバッグウィンドウ開始
	ImGui::Text("player %f", player_->GetWorldTransform().translation_.x);
	ImGui::End();
#endif // !_DEBUG

	// ==============================
	// フェーズごとの処理
	// ==============================
	switch (phase_) {

	// ------------------------------
	// フェードイン中の処理
	// ------------------------------
	case Phase::kFadeIn:
		player_->Update(); // プレイヤーの更新

		// カメラコントローラーの更新
		CController_->Update();

		for (Signboard* signboard : signboards_) {
			signboard->Update();
		}
		for (SpriteText* spriteText : spriteTexts_) {
			spriteText->Update();
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

		// デバッグカメラ更新
		debaucamera_->Update();

#ifdef _DEBUG
		// デバッグカメラ切り替え
		if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_SPACE)) {
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

		CController_->Update();
		for (Signboard* signboard : signboards_) {
			signboard->Update();
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
		break;

	}
#ifdef _DEBUG
	// 既存のデバッグ表示があればその続きに...

	ImGui::Begin("Coin Display Debug");

	// 1. セーブデータの確認
	if (ImGui::TreeNode("1. Save Data (Manager)")) {
		for (int i = 1; i <= 3; i++) { // ステージ1～3を確認
			int count = GameStateManager::GetInstance()->GetStarCoinRecord(i);
			ImGui::Text("Stage %d: %d Coins", i, count);
		}
		ImGui::TreePop();
	}

	// 2. 看板の座標確認
	if (ImGui::TreeNode("2. Signboard Positions")) {
		for (size_t i = 0; i < signboards_.size(); ++i) {
			// 看板の座標を取得
			Vector3 pos = signboards_[i]->GetWorldPosition();
			int id = signboards_[i]->GetStageID();
			ImGui::Text("Sign[%d] (Stage%d): (%.1f, %.1f, %.1f)", (int)i, id, pos.x, pos.y, pos.z);

			// もし全部 (0.0, 0.0, 0.0) なら、Initializeのタイミング問題確定です
			if (pos.x == 0 && pos.y == 0) {
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "WARNING: Position is Zero!");
			}
		}
		ImGui::TreePop();
	}

	// 3. 現在表示されているコインの確認
	ImGui::Separator();
	ImGui::Text("Display Coins Total: %d", (int)uiDisplayCoins_.size());

	if (ImGui::TreeNode("3. Coin List")) {
		int index = 0;
		for (auto& coin : uiDisplayCoins_) {
			Vector3 pos = coin->translation_;
			ImGui::Text("[%d] Pos: (%.1f, %.1f, %.1f) Scale: %.1f", index, pos.x, pos.y, pos.z, coin->scale_.x);
			index++;
		}
		ImGui::TreePop();
	}


	ImGui::Separator();
	if (ImGui::Button("Manual Reload (SetupDisplayCoins)")) {
		SetupDisplayCoins();
	}

	ImGui::End();
#endif
	if (!isCoinsSetup_) {
		SetupDisplayCoins();
		isCoinsSetup_ = true; // もう呼ばないようにする
	}
}

void StageSelectGameScene::Draw() {
	DirectXCommon* dxcommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxcommon->GetCommandList());

	// 🧱 ブロック描画（草・土でモデル切替）
	for (uint32_t y = 0; y < worldTransformBlocks_.size(); ++y) {
		for (uint32_t x = 0; x < worldTransformBlocks_[y].size(); ++x) {
			WorldTransform* block = worldTransformBlocks_[y][x];
			if (block) {
				// タイプを取得
				MapChipType type = mapChipField_->GetMapChipTypeByindex(x, y);

				switch (type) {
				case MapChipType::kDirt_:
					dirtModel_->Draw(*block, camera_);
					break;
				case MapChipType::kGrass_:
					grassModel_->Draw(*block, camera_);
					break;
				case MapChipType::kJumpPad_:
					kinokoModel_->Draw(*block, camera_);
					break;
				case MapChipType::kLadder_:
					hasigoModel_->Draw(*block, camera_);
					break;
				case MapChipType::kCloud_:
					kumoModel_->Draw(*block, camera_);
					break;
	
				}
			
			}
		}
	}
	for (auto& coinWt : uiDisplayCoins_) {
		// 常にクルクル回す
		coinWt->rotation_.y += 0.05f;
		math->worldTransFormUpdate(*coinWt.get());
		starCoinModel_->Draw(*coinWt, camera_);
	}
	// 🌌 スカイドーム描画
	skydome_->Draw();

	// 🧍 プレイヤー描画
	if (!player_->IsDead())
		player_->Draw();

	// 看板の描画
	for (Signboard* signboard : signboards_) {
		signboard->Draw();
	}

		for (SpriteText* spriteText : spriteTexts_) {
		spriteText->Draw();
	}
	    for (Yama* yama : yama_) {
		    yama->Draw();
	    }
	    for (Tree* tree : tree_) {
		    tree->Draw();
	    }

	// 🌗 フェード描画
	fade_->Draw(dxcommon->GetCommandList());

	Model::PostDraw();

	  //  スプライト描画
	Sprite::PreDraw(dxcommon->GetCommandList());
	if (activeSprite_&&isSprite) {
		activeSprite_->Draw();
	}
	if (isSprite) {
		SpriteSutage->Draw(); // ステージ用スプライトの描画
	}

	if (isShowCoinUI_&&isSprite) {
		for (int i = 0; i < 3; i++) {
			if (uiStarCoins_[i]) {
				uiStarCoins_[i]->Draw();
			}
		}
	}

	Sprite::PostDraw();


}

void StageSelectGameScene::ChangePhase() 
{
	switch (phase_) {
	case Phase::kFadeIn:
		if (fade_->isFinished()) {
			phase_ = Phase::kPlay;
		}
		break;
	case Phase::kPlay:
		if (isTimerFinished_) {
			finishedTimer++;
		}
		player_->UpdateVictoryAnimation();
		if (finishedTimer > 180) {
			
				finished_ = true;
			
		}

		break;
	
	
	}
}

void StageSelectGameScene::GenerateBlocks() 
{
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

void StageSelectGameScene::CheekAllcollision() {
	AABB aabb1 = player_->GetAABB();

	// スプライトを非表示にする初期化
	activeSprite_ = nullptr;
	isShowCoinUI_ = false;


	for (Signboard* signboard : signboards_) {
		AABB aabb2 = signboard->GetAABB();
		if (math->IsCollision(aabb1, aabb2)) {
			//  看板のIDに応じて表示するスプライトを切り替え
			int stageID = signboard->GetStageID();

			// 1. そのステージの過去のコイン取得数を取得
			int savedCount = GameStateManager::GetInstance()->GetStarCoinRecord(stageID);

			// 2. 3つのコインUIの色を切り替え
			for (int i = 0; i < 3; i++) {
				if (i < savedCount) {
					uiStarCoins_[i]->SetTextureHandle(texHandleCoinGet_); // 黄色（取得済み）
				} else {
					uiStarCoins_[i]->SetTextureHandle(texHandleCoinEmpty_); // グレー（未取得）
				}
			}

			// 3. UIを表示するフラグをON
			isShowCoinUI_ = true;
	
			switch (stageID) {
			case 1:
				activeSprite_ = Sprite1_1;
				break;
			case 2:
				activeSprite_ = Sprite1_2;
				break;
			case 3:
				activeSprite_ = Sprite1_3;
				break;
			case 4:
				activeSprite_ = Sprite1_4;
				break;
			default:
				activeSprite_ = nullptr; // 他の看板は非表示
				break;
			}

			if (Input::GetInstance()->TriggerKey(DIK_W) && !isTimerFinished_) {
				isTimerFinished_ = true;
				isSprite = false; // スプライト表示
				fade_->Start(Fade::Status::FadeOut, 3.0f);
				nextStageID_ = signboard->GetStageID();
				player_->StartVictoryPose();
			}
		}
	}
}

StageSelectGameScene::~StageSelectGameScene() {
	for (auto& blockLine : worldTransformBlocks_) {
		for (auto& block : blockLine) {
			delete block; // 各ブロックのWorldTransformを解放
		}
	}
	worldTransformBlocks_.clear(); // ブロック行列をクリア
	for (Signboard* signboard : signboards_) {
		delete signboard; // 看板の解放
	}
	signboards_.clear();
	delete player_;       // プレイヤーの解放
	delete mapChipField_; // マップチップフィールドの解放
	delete skydome_;      // スカイドームの解放
	delete fade_;         // フェードの解放
	delete debaucamera_;  // デバッグカメラの解放
	delete CController_;  // カメラコントローラーの解放

}

void StageSelectGameScene::SetupDisplayCoins() {
	// リストをクリア
	uiDisplayCoins_.clear();

	// デバッグ用：看板の数を確認
	// printf("Total Signboards: %d\n", (int)signboards_.size());

	for (Signboard* signboard : signboards_) {
		int stageID = signboard->GetStageID();
		int savedCount = GameStateManager::GetInstance()->GetStarCoinRecord(stageID);

		// デバッグ用：ステージIDと保存枚数、看板の座標を確認
		Vector3 pos = signboard->GetWorldPosition();
		printf("Stage: %d, Saved: %d, Pos: (%f, %f, %f)\n", stageID, savedCount, pos.x, pos.y, pos.z);

		if (savedCount <= 0)
			continue;

		// コイン生成
		for (int i = 0; i < savedCount; ++i) {
			auto coinWt = std::make_unique<WorldTransform>();
			coinWt->Initialize();

			// 位置計算
			float offsetX = (i - (savedCount - 1) / 2.0f) * 1.5f; // 少し間隔を広げました(1.0f -> 1.5f)

			coinWt->translation_.x = pos.x + offsetX;
			coinWt->translation_.y = pos.y + 4.0f;

			// カメラが手前（マイナス方向）にある場合、コインも少し手前に置かないと埋もれます
			coinWt->translation_.z = pos.z - 2.0f;

			coinWt->scale_ = {0.5f, 0.5f, 0.5f};

			// 行列更新
			math->worldTransFormUpdate(*coinWt.get()); // ※ユーザーの関数の書き方に合わせる

			uiDisplayCoins_.push_back(std::move(coinWt));
		}
	}
}