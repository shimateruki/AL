#include "GameScene.h"

 #include"imgui.h"
#include"imgui_impl_dx12.h"
#include "imgui_impl_win32.h"


using namespace KamataEngine; // KamataEngine名前空間を使用
//初期化
void GameScene::Initialize() 
{
	model_ = Model::Create();
	textureHandel_ = TextureManager::Load("player.png");
	player_.Initialize(model_, textureHandel_);
	viewProjection_.Initialize();
	// デバッグカメラの生成と初期化
	debaucamera_ = new DebugCamera(100, 50); // デバッグカメラのインスタンスを生成
	debaucamera_->SetFarZ(1280.0f);          // デバッグカメラのZ軸方向の最も遠いクリップ面を設定
	AxisIndicator::GetInstance()->Initialize();
	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetCamera(&viewProjection_.GetCamera()); // 軸インジケーターのカメラをデバッグカメラに設定

}

//更新
void GameScene::Update() 
{
	player_.Update();

	debaucamera_->Update(); // デバッグカメラの更新
	AxisIndicator::GetInstance()->Update(); // 軸インジケーターの更新
#ifdef _DEBUG // デバッグビルド時のみ有効なコード
	// スペースキーが押されたらデバッグカメラの有効/無効を切り替える
	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		if (!isDebugCameraActive_) {
			isDebugCameraActive_ = true;
		} else {
			isDebugCameraActive_ = false;
		}
	}
#endif // !_DEBUG

	// カメラの処理
	if (isDebugCameraActive_) {                                          // デバッグカメラが有効な場合
		viewProjection_.GetCamera().matView = debaucamera_->GetCamera().matView;             // デバッグカメラのビュー行列を設定
		viewProjection_.GetCamera().matProjection = debaucamera_->GetCamera().matProjection;   // デバッグカメラの射影行列を設定
		viewProjection_.GetCamera().TransferMatrix();                                            // ビュープロジェクション行列をGPUに転送
	} else {                                                             // 通常カメラが有効な場合
		viewProjection_.GetCamera().UpdateMatrix();                                              // 通常カメラの行列を更新
	}
}

//描画
void GameScene::Draw() 
{
	DirectXCommon* dxcommon = DirectXCommon::GetInstance();
	Model::PreDraw(dxcommon->GetCommandList());
	player_.Draw(viewProjection_); 
		AxisIndicator::GetInstance()->Draw();
	Model::PostDraw();

}

GameScene::~GameScene() 
{ }
