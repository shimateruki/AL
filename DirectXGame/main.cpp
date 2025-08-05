#include <Windows.h>
#include "KamataEngine.h"
#include "GameScene.h"


using namespace KamataEngine;
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//エンジンの初期化
	Initialize(L"LE2C_13_シマ_テルキ_AL3");


	
	// DirectXCommonのインスタンスを取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	// ゲームシーンのインスタンスを作成
	GameScene* gameScene = new GameScene();

	// ゲームシーンの初期化
	gameScene->Initialize();

	//メインループ
	while (true)
	{
		//エンジン更新
		if (Update()) {
			break;
		}
		imguiManager->Begin();
		//ゲーム更新
		gameScene->Update();
		imguiManager->End();
		//描画開始
		dxCommon->PreDraw();
		
	

		gameScene->Draw();

			// ゲームシーンの描画
		imguiManager->Draw();
		//描画終了
		dxCommon->PostDraw();
	

	}
	//ゲームシーンの解放
	delete gameScene;
	// nullptrの代入
	gameScene = nullptr;
	//エンジン終了処理
	FilePathSet();
	return 0;
}
