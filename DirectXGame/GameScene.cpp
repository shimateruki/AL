#include "GameScene.h"

using namespace KamataEngine;

void GameScene::Initialize() {
	textureHandel_ = TextureManager::Load("sample.png");

	// モデルロード

	blockModel_= Model::CreateFromOBJ("block", true);

	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	playerModel_ = Model::CreateFromOBJ("player", true);

	// カメラ
	camera_.farZ = 1280.0f;
	camera_.Initialize();

	// Debugカメラ
	debaucamera_ = new DebugCamera(100, 50);
	debaucamera_->SetFarZ(1280.0f);

		// マップチップフィールド
	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	// プレイヤー
	player_ = new Player();
	Vector3 playerPosition = mapChipField_->GetChipPositionIndex(1, 18);
	player_->Initialize(playerModel_, &camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);


	// スカイドーム
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);



	CController_ = new CameraController(); // 生成
	CController_->Initialize(&camera_);     // 初期化
	CController_->SetTarget(player_);      // 追従対象セット
	CController_->Reset(); 


		CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	CController_->SetMovableSrea(cameraArea);

	GenerrateBlock();

	

	

}

// 更新
void GameScene::Update() {

	player_->Update();



//更新
void GameScene::Update() 
{

	player_->Update(); 

	CController_->Update();


	


	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlocks : worldTransformBlockLine) {
			if (!worldTransformBlocks) {
				continue;
			}
			worldTransformBlocks->matWorld_ = math->MakeAffineMatrix(worldTransformBlocks->scale_, worldTransformBlocks->rotation_, worldTransformBlocks->translation_);
			worldTransformBlocks->TransferMatrix();

		}
	}
	debaucamera_->Update();


	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		if (!isDebugCameraActive_) {
			isDebugCameraActive_ = true;
		} else {
			isDebugCameraActive_ = false;
		}
	}

#endif // !_DEBUG
	// カメラの処理
	if (isDebugCameraActive_) {
		camera_.matView = debaucamera_->GetCamera().matView;
		camera_.matProjection = debaucamera_->GetCamera().matProjection;


		//ビュープロジェクション行列の転送

		camera_.TransferMatrix();
	} else {
		camera_.UpdateMatrix();
	}

	skydome_->Update();
}

// 描画

void GameScene::Draw()

{
	DirectXCommon* dxcommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxcommon->GetCommandList());

	 player_->Draw();

	 	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		 for (WorldTransform* worldTransformBlocks : worldTransformBlockLine) {
			 if (!worldTransformBlocks) {
				 continue;
			 }

			 blockModel_->Draw(*worldTransformBlocks, camera_);
		 }
	 }


	skydome_->Draw();

	Model::PostDraw();
}

void GameScene::GenerrateBlock() 
{
	const uint32_t kNumBlockVirtal = mapChipField_->GetNumBlockVirtcal();
	const uint32_t kNumBlockHorizotal = mapChipField_->GetNumBlockHorizonal();

	worldTransformBlocks_.resize(kNumBlockVirtal);
	for (uint32_t i = 0; i < kNumBlockVirtal; ++i) {
		worldTransformBlocks_[i].resize(kNumBlockHorizotal);
	}

	for (uint32_t i = 0; i < kNumBlockVirtal; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizotal; ++j) {

			if (mapChipField_->GetMapChipTypeByindex(j, i) == MapChipType::kBlock_) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetChipPositionIndex(j, i);
			}
		}
	}
	
	
}
GameScene::~GameScene()
{
	delete blockModel_; 
	delete debaucamera_;
	delete modelSkydome_;
	delete player_;
	delete mapChipField_;


	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlocks : worldTransformBlockLine) {
			delete worldTransformBlocks;
		}
	}
	worldTransformBlocks_.clear();
}
