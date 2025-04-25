#include "GameScene.h"


using namespace KamataEngine;

//初期化
void GameScene::Initialize() 
{
	textureHandel_ = TextureManager::Load("sample.png");

	model_ = Model::Create();
	worldTransform_.Initialize();
	camera_.Initialize();
	player_ = new Player();
	player_->Initialize(model_, textureHandel_, &camera_);

}
//更新
void GameScene::Update() 
{

	player_->Update(); 



	
}

//描画

void GameScene::Draw() 

{ DirectXCommon* dxcommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxcommon->GetCommandList());
	player_->Draw();
	Model::PostDraw();
}
GameScene::~GameScene()
{
	delete model_; 
	delete player_;
	
}


