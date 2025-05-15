#include "GameScene.h"



using namespace KamataEngine;

void GameScene::Initialize() {
	textureHandel_ = TextureManager::Load("sample.png");

	// モデルロード
	blockModel_ = Model::CreateFromOBJ("block", true);
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	playerModel_ = Model::CreateFromOBJ("player", true);

	// カメラ
	camera_.farZ = 1280.0f;
	camera_.Initialize();

	// Debugカメラ
	debaucamera_ = new DebugCamera(100, 50);
	debaucamera_->SetFarZ(1280.0f);

	// プレイヤー
	player_ = new Player();
	player_->Initialize(playerModel_, textureHandel_, &camera_);

	// スカイドーム
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, &camera_);

	// ブロック生成
	const uint32_t kNumBlockVirtal = 10;
	const uint32_t kNumBlockHorizotal = 20;
	const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;

	for (uint32_t i = 0; i < kNumBlockVirtal; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizotal; ++j) {
			if ((i + j) % 2 == 1) {
				blocks_[i][j] = nullptr; 
				continue;
			}

			blocks_[i][j] = new Block();
			Vector3 pos = {kBlockWidth * j, kBlockHeight * i, 0.0f};
			blocks_[i][j]->Initialize(blockModel_, pos, &camera_);
		}
	}
}




//更新
void GameScene::Update() 
{

	player_->Update(); 

	


	
	//ブロックの更新

    for (int i = 0; i < 10; ++i) {
		for (int j = 0; j < 20; ++j) {
			if (blocks_[i][j]) {
				blocks_[i][j]->Update();
			}
		}
	}
	debaucamera_->Update();
#ifdef _DEBUG

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		if (!isDebugCameraActive_) {
			isDebugCameraActive_ = true;
		}
		else 
		{
			isDebugCameraActive_ = false;
		}
		

	}


#endif // !_DEBUG
	//カメラの処理
	if (isDebugCameraActive_) 
	{
		camera_.matView = debaucamera_->GetCamera().matView;
		camera_.matProjection = debaucamera_->GetCamera().matProjection;
		//ビュープロジェクション行列の転送
		camera_.TransferMatrix();
	}
	else 
	{
		camera_.UpdateMatrix();
	}


	skydome_->Update();
	
}

//描画

void GameScene::Draw()

{
	DirectXCommon* dxcommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxcommon->GetCommandList());
	 player_->Draw();

	 for (int i = 0; i < 10; ++i) {
		 for (int j = 0; j < 20; ++j) {
			 if (blocks_[i][j]) {
				 blocks_[i][j]->Draw();
			 }
		 }
	 }

skydome_->Draw();

	Model::PostDraw();

	
}
GameScene::~GameScene()
{
	delete model_; 
	delete debaucamera_;
	delete modelSkydome_;
	delete player_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine:worldTransformBlocks_) 
	{
		for (WorldTransform* worldTransformBlocks:worldTransformBlockLine ) 
		{
			delete worldTransformBlocks;
		}

	}
	worldTransformBlocks_.clear();
	
}


