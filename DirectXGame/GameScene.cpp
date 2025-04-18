#include "GameScene.h"
using namespace KamataEngine;
//初期化
void GameScene::Initialize() 
{

	textureHandle_ = TextureManager::Load("sample.png");
	model_ = Model::Create();
	sprite_ = Sprite::Create(textureHandle_, {100, 50});
	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	//カメラの初期化
	camera_.Initialize();
	soundDatahandle_ = Audio::GetInstance()->LoadWave("mokugyo.wav");
	Audio::GetInstance()->PlayWave(soundDatahandle_);
	voiceHandle_ = Audio::GetInstance()->PlayWave(soundDatahandle_, true);
	//ライン描画が参照するカメラを指定する
	PrimitiveDrawer::GetInstance()->SetCamera(&camera_);
	//デバッグカメラ
	debugcamera_ = new DebugCamera(1280, 720);
	//軸方向の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向表示が参照するビュープロジェクトクションを指定する
	AxisIndicator::GetInstance()->SetTargetCamera(&debugcamera_->GetCamera());


}
//更新
void GameScene::Update() 
{
	//画像を動かす処理
	Vector2 position = sprite_->GetPosition();
	position.x += 2.0f;
	position.y += 1.0f;
	sprite_->SetPosition(position);

	//音声ストップ
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) 
	{
		Audio::GetInstance()->StopWave(voiceHandle_);
	}

	//ウィンドウ作成
/*	ImGui::Begin("Debug1")*/;
#ifdef _DEBUG
	ImGui::InputFloat3("inputFloat3", inputFloat3);
	ImGui::SliderFloat3("sliderFloat3", inputFloat3, 0.0f, 1.0f);

	ImGui::Text("kamata tarou %d %d %d", 2050, 12, 31);

	ImGui::End();

	//デモウィンドウ
	ImGui::ShowDebugLogWindow();
#endif // !_DEBUG

	// デバッグカメラの更新
	debugcamera_->Update();
	
}

//描画
void GameScene::Draw() {
	// インスタンスの取得
	DirectXCommon* dxCmoon = DirectXCommon::GetInstance();

	// スプライト描画処理
	Sprite::PreDraw(dxCmoon->GetCommandList());

	// 3dモデル描画
	sprite_->Draw();

	// スプライト描画処理
	Sprite::PostDraw();

	// 3d描画処理
	Model::PreDraw(dxCmoon->GetCommandList());

	// モデルの連動
	model_->Draw(worldTransform_, debugcamera_->GetCamera(), textureHandle_);

	//ラインを描画する
	PrimitiveDrawer::GetInstance()->DrawLine3d({0, 0, 0}, {0, 10, 0}, {1.0f, 0.0f, 0.0f, 1.0f});

	// 3d描画後処理
	Model::PostDraw();



	
}

GameScene::~GameScene() 

{
	delete sprite_;
	delete model_;
	delete debugcamera_;
}
