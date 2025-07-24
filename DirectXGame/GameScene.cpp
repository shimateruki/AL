#include "GameScene.h"
using namespace KamataEngine; // KamataEngine名前空間を使用
//初期化
void GameScene::Initialize() 
{
	model_ = Model::Create();
	textureHandel_ = TextureManager::Load("sample.png");
	player_.Initialize(model_, textureHandel_);
	viewProjection_.Initialize();
}

//更新
void GameScene::Update() 
{
	player_.Update();
}

//描画
void GameScene::Draw() 
{
	DirectXCommon* dxcommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxcommon->GetCommandList());
	player_.Draw(viewProjection_); 
	Model::PostDraw();
}

GameScene::~GameScene() 
{ }
