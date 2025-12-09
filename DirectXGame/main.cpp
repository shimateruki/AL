#include "GameScene.h"
#include "KamataEngine.h"
#include "StageSelectGameScene.h"
#include "TitleScene.h"
#include <Windows.h>
// ★不要なヘッダーを削除（GameScene1_2.hなどはもう要りません）
#include "GameOver.h"
#include "GameStateManager.h"

using namespace KamataEngine;

enum class Scene {
	kUnknown = 0,
	kTitle,
	kStageSelect,
	kGame, 
	kGaameOver
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
	// ★個別のシーンポインタ (gameScene1_2等) は削除
	GameOver* gameOver = nullptr;

	// 現在のシーンの状態
	Scene currentSceneEnum = Scene::kTitle;

	// ★現在プレイするステージIDを保持する変数
	int currentStageID = 1;

	// 初期シーンの設定
	titleScene = new TitleScene();
	titleScene->Initialize();

	// メインループ
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
				nextSceneEnum = Scene::kStageSelect;
				Vector3 newDefaultPos = {3.0f, 5.0f, 0.0f};
				GameStateManager::GetInstance()->SetPlayerStartPosition(newDefaultPos);
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
				// ★GetNextScene()の判定は、GameScene側で共通化されている前提
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
		}

		// ==========================================
		// シーン切り替え処理
		// ==========================================
		if (nextSceneEnum != currentSceneEnum) {
			// 現在のシーンを解放
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
			}

			// 次のシーンを初期化
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
				// ★ここで引数としてステージIDを渡す！
				gameScene->Initialize(currentStageID);
				break;
			case Scene::kGaameOver:
				gameOver = new GameOver();
				gameOver->Initialize();
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
		}

		inguiManager->Draw();
		dxCommon->PostDraw();
	}

	// 終了処理
	delete titleScene;
	delete stageSelectScene;
	delete gameScene;
	delete gameOver;

	// エンジン終了処理
	FilePathSet();
	return 0;
}