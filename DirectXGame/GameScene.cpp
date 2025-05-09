#include "GameScene.h"

#include "math.h"

using namespace KamataEngine;

//初期化
void GameScene::Initialize() 
{
	textureHandel_ = TextureManager::Load("sample.png");

	model_ = Model::Create();
	worldTransform_.Initialize();
	camera_.Initialize();
	//player_ = new Player();
	//player_->Initialize(model_, textureHandel_, &camera_);

	//要素数
	const uint32_t kNumBlockVirtal = 10;
	const uint32_t kNumBlockHorizotal = 20;
	//ブロック一個分
	const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;
	

	worldTransformBlocks_.resize(kNumBlockVirtal);
	for (uint32_t i = 0; i < kNumBlockVirtal; ++i) {
		worldTransformBlocks_[i].resize(kNumBlockHorizotal);
	}

	for (uint32_t  i = 0; i < kNumBlockVirtal; ++i) 
	{
		for (uint32_t j = 0; j < kNumBlockHorizotal; ++j) {
		
			if (i % 2==  0 && j % 2 ==0) {

				worldTransformBlocks_[i][j] = new WorldTransform();
				worldTransformBlocks_[i][j]->Initialize();
				worldTransformBlocks_[i][j]->translation_.x = kBlockWidth * j;
				worldTransformBlocks_[i][j]->translation_.y = kBlockHeight * i;
			}
		
		}
	}

	debaucamera_ = new DebugCamera(100, 50);
	
	//

}



//更新
void GameScene::Update() 
{

	//player_->Update(); 


	
	//ブロックの更新

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlocks : worldTransformBlockLine) 
		{
			if (!worldTransformBlocks) 
			{
				continue;
			}
			worldTransformBlocks->matWorld_ = MakeAffineMatrix(worldTransformBlocks->scale_, worldTransformBlocks->rotation_, worldTransformBlocks->translation_);
			worldTransformBlocks->TransferMatrix();
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
	
}

//描画

void GameScene::Draw()

{
	DirectXCommon* dxcommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxcommon->GetCommandList());
	// player_->Draw();

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) 
	{
		for (WorldTransform* worldTransformBlocks : worldTransformBlockLine) 
		{
			if (!worldTransformBlocks) 
			{
				continue;
			}
			
				model_->Draw(*worldTransformBlocks, camera_);
			
		}
		
	}
	Model::PostDraw();
}
GameScene::~GameScene()
{
	delete model_; 
	delete debaucamera_;
	//delete player_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine:worldTransformBlocks_) 
	{
		for (WorldTransform* worldTransformBlocks:worldTransformBlockLine ) 
		{
			delete worldTransformBlocks;
		}

	}
	worldTransformBlocks_.clear();
	
}


