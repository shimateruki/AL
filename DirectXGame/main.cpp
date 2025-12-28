#include "GameOver.h"
#include "GameScene.h"
#include "GameStateManager.h"
#include "KamataEngine.h"
#include "StageSelectGameScene.h"
#include "TitleScene.h"
#include <Windows.h>
// ★ 追加: チュートリアルシーンのヘッダー
#include "TutorialScene.h"

using namespace KamataEngine;

enum class Scene {
	kUnknown = 0,
	kTitle,
	kStageSelect,
	kGame,
	kGaameOver,
	kTutorial 
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	// エンジンの初期化
	Initialize(L"スライムラン");

	ImGuiManager* inguiManager = ImGuiManager::GetInstance();
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// シーンのポインタ
	GameScene* gameScene = nullptr;
	TitleScene* titleScene = nullptr;
	StageSelectGameScene* stageSelectScene = nullptr;
	GameOver* gameOver = nullptr;
	// ★ 追加: チュートリアル用のポインタ
	TutorialScene* tutorialScene = nullptr;

	// 現在のシーンの状態
	Scene currentSceneEnum = Scene::kTitle;

	// ★現在プレイするステージIDを保持する変数
	int currentStageID = 1;

	// 初期シーンの設定
	titleScene = new TitleScene();
	titleScene->Initialize();


	while (true) {

		inguiManager->Begin();

		if (Update()) {

			break;
		}


		Scene nextSceneEnum = currentSceneEnum;

		// ==========================================
		// シーン終了条件のチェックと次シーンの決定
		// ==========================================
		switch (currentSceneEnum) {
		case Scene::kTitle:
			if (titleScene != nullptr && titleScene->isfinished()) {

				// ★ タイトルで「チュートリアル」が選ばれたか確認
				if (titleScene->GetNextScene() == TitleScene::NextScene::kTutorial) {
					nextSceneEnum = Scene::kTutorial;
				} else {
					// 通常スタート（ステージセレクトへ）
					nextSceneEnum = Scene::kStageSelect;
					Vector3 newDefaultPos = {3.0f, 5.0f, 0.0f};
					GameStateManager::GetInstance()->SetPlayerStartPosition(newDefaultPos);
				}
			}
			break;

		case Scene::kStageSelect:
			if (stageSelectScene != nullptr && stageSelectScene->isFinished()) {
				int nextID = stageSelectScene->GetNextStageID();

				if (nextID == 0) {
					nextSceneEnum = Scene::kTitle;
				} else {
					// ★どのステージが選ばれても、次は kGame に行く
					currentStageID = nextID; // 選ばれたステージIDを保存
					nextSceneEnum = Scene::kGame;
				}
			}
			break;

		case Scene::kGame:
			// ★統合されたゲームシーンの処理
			if (gameScene != nullptr && gameScene->isFinished()) {

				if (gameScene->currentSelectIndex() == 1) { // ポーズからタイトルへ
					nextSceneEnum = Scene::kTitle;
				}
				// ★GetNextScene()の判定
				else if (gameScene->GetNextScene() == GameScene::NextScene::kGameOver) {
					nextSceneEnum = Scene::kGaameOver;
				} else {
					// クリアしたらステージセレクトへ
					nextSceneEnum = Scene::kStageSelect;
				}
			}
			break;

		case Scene::kGaameOver:
			if (gameOver != nullptr && gameOver->isFinished()) {
				if (gameOver->GetNextScene() == GameOver::NextScene::kRetry) {
					// リトライなら、さっき保存した currentStageID を使って kGame へ
					nextSceneEnum = Scene::kGame;
				} else if (gameOver->GetNextScene() == GameOver::NextScene::kStageSelect) {
					nextSceneEnum = Scene::kStageSelect;
				} else if (gameOver->GetNextScene() == GameOver::NextScene::kTitle) {
					nextSceneEnum = Scene::kTitle;
				}
			}
			break;

		//  チュートリアルの終了判定
		case Scene::kTutorial:
			if (tutorialScene != nullptr && tutorialScene->isFinished()) {
				// チュートリアルが終わったらタイトルに戻る
				nextSceneEnum = Scene::kTitle;
			}
			break;
		}

		// ==========================================
		// シーン切り替え処理
		// ==========================================
		if (nextSceneEnum != currentSceneEnum) {
			// 1. 現在のシーンを解放
			switch (currentSceneEnum) {
			case Scene::kTitle:
				delete titleScene;
				titleScene = nullptr;
				break;
			case Scene::kStageSelect:
				delete stageSelectScene;
				stageSelectScene = nullptr;
				break;
			case Scene::kGame:
				delete gameScene;
				gameScene = nullptr;
				break;
			case Scene::kGaameOver:
				delete gameOver;
				gameOver = nullptr;
				break;
			case Scene::kTutorial: 
				delete tutorialScene;
				tutorialScene = nullptr;
				break;
			}

			// 2. 次のシーンを初期化
			currentSceneEnum = nextSceneEnum;
			switch (currentSceneEnum) {
			case Scene::kTitle:
				titleScene = new TitleScene();
				titleScene->Initialize();
				break;
			case Scene::kStageSelect:
				stageSelectScene = new StageSelectGameScene();
				stageSelectScene->Initialize();
				break;
			case Scene::kGame:
				gameScene = new GameScene();
				gameScene->Initialize(currentStageID);
				break;
			case Scene::kGaameOver:
				gameOver = new GameOver();
				gameOver->Initialize();
				break;
			case Scene::kTutorial: 
				tutorialScene = new TutorialScene();
				tutorialScene->Initialize();
				break;
			}
		}

		// ==========================================
		// 更新処理 (Update)
		// ==========================================
		switch (currentSceneEnum) {
		case Scene::kTitle:
			if (titleScene)
				titleScene->Update();
			break;
		case Scene::kStageSelect:
			if (stageSelectScene)
				stageSelectScene->Update();
			break;
		case Scene::kGame:
			if (gameScene)
				gameScene->Update();
			break;
		case Scene::kGaameOver:
			if (gameOver)
				gameOver->Update();
			break;
		case Scene::kTutorial:
			if (tutorialScene)
				tutorialScene->Update();
			break;
		}

		inguiManager->End();

		// ==========================================
		// 描画処理 (Draw)
		// ==========================================
		dxCommon->PreDraw();

		switch (currentSceneEnum) {
		case Scene::kTitle:
			if (titleScene)
				titleScene->Draw();
			break;
		case Scene::kStageSelect:
			if (stageSelectScene)
				stageSelectScene->Draw();
			break;
		case Scene::kGame:
			if (gameScene)
				gameScene->Draw();
			break;
		case Scene::kGaameOver:
			if (gameOver)
				gameOver->Draw();
			break;
		case Scene::kTutorial: 
			if (tutorialScene)
				tutorialScene->Draw();
			break;
		}

		inguiManager->Draw();
		dxCommon->PostDraw();
	}

	// 終了処理
	delete titleScene;
	delete stageSelectScene;
	delete gameScene;
	delete gameOver;
	delete tutorialScene; 

	// エンジン終了処理
	 FilePathSet(); 
	return 0;
}