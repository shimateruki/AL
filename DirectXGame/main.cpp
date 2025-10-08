
#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include <Windows.h>
#include"StageSelectGameScene.h"
#include "GameScene1_2.h"
#include "GameScene1_3.h"
#include"GameScene2_1.h"
#include "GameStateManager.h"
#include"GameOver.h"

using namespace KamataEngine;

enum class Scene {
	kUnknown = 0,
	kTitle,
	kStageSelect,
	kGame,
	kGame1_2, // 追加: 1-2ステージのゲームシーン
	kGame1_3, // 追加: 1-3ステージのゲームシーン
	kGame2_1,
	kGaameOver
	
};

    // Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	// エンジンの初期化
	Initialize(L"スライムラン");

	ImGuiManager* inguiManager = ImGuiManager::GetInstance();
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// シーンのポインタを管理する変数
	// IScene を使用しない場合、以下のように個別管理します。
	GameScene* gameScene = nullptr;
	TitleScene* titleScene = nullptr;
	StageSelectGameScene* stageSelectScene = nullptr;
	GameScene1_2* gameScene1_2 = nullptr; // 1-2ステージのゲームシーン
	GameScene1_3* gameScene1_3 = nullptr; // 1-3ステージのゲームシーン
	GameScene2_1* gameScene2_1 = nullptr;
	GameOver* gameOver = nullptr;

	// 現在のシーンの状態
	Scene currentSceneEnum = Scene::kTitle;

	// 初期シーンの設定
	titleScene = new TitleScene();
	titleScene->Initialize();

	gameOver = new GameOver();
	gameOver->Initialize();

	// メインループ
	while (true) {
		inguiManager->Begin();

		// エンジン更新（ウィンドウメッセージ処理、DirectX共通処理など）
		if (Update()) { // ウィンドウが閉じられたらループを抜ける
			break;
		}

		// --- シーン切り替え、更新、描画のロジックをここに統合 ---

		Scene nextSceneEnum = currentSceneEnum; // 次のシーンを一時的に現在のシーンと同じにする

		// ★シーン終了条件のチェックと次シーンの決定
		switch (currentSceneEnum) {
		case Scene::kTitle:
			if (titleScene != nullptr && titleScene->isfinished()) { // タイトルシーンが終了条件を満たしたら
				nextSceneEnum = Scene::kStageSelect;                        // 次のシーンはゲームシーン
				                                                     // タイトルに戻る際は、次のステージセレクトの初期位置をリセット
				Vector3 newDefaultPos = {3.0f, 5.0f, 0.0f};
				GameStateManager::GetInstance()->SetPlayerStartPosition(newDefaultPos);
			}
			break;
		case Scene::kStageSelect:
			// ステージセレクトシーンの終了条件をここに追加する場合
			 if (stageSelectScene != nullptr && stageSelectScene->isFinished()) { 
				 if (stageSelectScene->GetNextStageID() ==0) {
					 nextSceneEnum = Scene::kTitle;
				 }

				 else if (stageSelectScene->GetNextStageID()==1) {
					 nextSceneEnum = Scene::kGame; // 次のシーンはゲームシーン
				 } else if (stageSelectScene->GetNextStageID() == 2) {
					 nextSceneEnum = Scene::kGame1_2; // 次のシーンはゲームシーン
				 } else if (stageSelectScene->GetNextStageID() == 3) {
					 nextSceneEnum = Scene::kGame1_3; // 次のシーンはゲームシーン
				 } else if (stageSelectScene->GetNextStageID() == 4) {
					 nextSceneEnum = Scene::kGame2_1; // 次のシーンはゲームシーン

				 }
			 }
			break;
		case Scene::kGame1_2:
			if (gameScene1_2 != nullptr && gameScene1_2->isFinished() && gameScene1_2->currentSelectIndex() == 1) {
				nextSceneEnum = Scene::kTitle;
			} else if (gameScene1_2 != nullptr && gameScene1_2->isFinished()) {
				// GameSceneのGetNextScene()メソッドを使って、次のシーンを決定
				if (gameScene1_2->GetNextScene() == GameScene1_2::NextScene::kGameOver) {
					nextSceneEnum = Scene::kGaameOver;
				} else {
					// デフォルトの遷移先（ステージセレクト）
					nextSceneEnum = Scene::kStageSelect;
				}
			}
		case Scene::kGame1_3:
			if (gameScene1_3 != nullptr && gameScene1_3->isFinished() && gameScene1_3->currentSelectIndex() == 1) {
				nextSceneEnum = Scene::kTitle;
			} else if (gameScene1_3 != nullptr && gameScene1_3->isFinished()) {
				// GameSceneのGetNextScene()メソッドを使って、次のシーンを決定
				if (gameScene1_3->GetNextScene() == GameScene1_3::NextScene::kGameOver) {
					nextSceneEnum = Scene::kGaameOver;
				} else {
					// デフォルトの遷移先（ステージセレクト）
					nextSceneEnum = Scene::kStageSelect;
				}
			}
			break;
		case Scene::kGame:
			if (gameScene != nullptr && gameScene->isFinished()&&gameScene->currentSelectIndex()==1) {
				nextSceneEnum = Scene::kTitle;
			}
			else if (gameScene != nullptr && gameScene->isFinished()) {
				// GameSceneのGetNextScene()メソッドを使って、次のシーンを決定
				if (gameScene->GetNextScene() == GameScene::NextScene::kGameOver) {
					nextSceneEnum = Scene::kGaameOver;
				} else {
					// デフォルトの遷移先（ステージセレクト）
					nextSceneEnum = Scene::kStageSelect;
				}
			}
			break;
		case Scene::kGame2_1:
			if (gameScene2_1 != nullptr && gameScene2_1->isFinished() && gameScene2_1->currentSelectIndex() == 1) {
				nextSceneEnum = Scene::kTitle;
			} else if (gameScene2_1 != nullptr && gameScene2_1->isFinished()) {
				// GameSceneのGetNextScene()メソッドを使って、次のシーンを決定
				if (gameScene2_1->GetNextScene() == GameScene2_1::NextScene::kGameOver) {
					nextSceneEnum = Scene::kGaameOver;
				} else {
					// デフォルトの遷移先（ステージセレクト）
					nextSceneEnum = Scene::kStageSelect;
				}
			}
			// 次のシーンはタイトルシーン
		case Scene::kGaameOver:
			if (gameOver != nullptr && gameOver->isFinished()) {
				if (gameOver->GetNextScene() == GameOver::NextScene::kRetry) {
					// GameStateManagerからゲームオーバーになったステージIDを取得
					int stageID = GameStateManager::GetInstance()->GetCurrentStageID();

					// 取得したIDに基づいて次のシーンを決定
					if (stageID == 1) {
						nextSceneEnum = Scene::kGame;
					} else if (stageID == 2) {
						nextSceneEnum = Scene::kGame1_2;
					} else if (stageID == 3) {
						nextSceneEnum = Scene::kGame1_3;
					} else if (stageID == 4) {
						nextSceneEnum = Scene::kGame2_1;
					}
				} else if (gameOver->GetNextScene() == GameOver::NextScene::kStageSelect) {
					nextSceneEnum = Scene::kStageSelect;
				} else if (gameOver->GetNextScene() == GameOver::NextScene::kTitle) {
					nextSceneEnum = Scene::kTitle;
				}
			}
			break;
	
		}

		// ★シーンが切り替わる必要がある場合
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
			case Scene::kGame1_2:
				delete gameScene1_2;
				gameScene1_2 = nullptr;
				break;
			case Scene::kGame1_3:
				delete gameScene1_3;
				gameScene1_3 = nullptr;
				break;
			case Scene::kGame2_1:
				delete gameScene2_1;
				gameScene2_1 = nullptr;
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
			case Scene::kGame1_2:
				gameScene1_2 = new GameScene1_2();
				gameScene1_2->Initialize();
				break;
			case Scene::kGame1_3:
				gameScene1_3 = new GameScene1_3();
				gameScene1_3->Initialize();
				break;
			case Scene::kGame2_1:
				gameScene2_1 = new GameScene2_1();
				gameScene2_1->Initialize();
				break;
			case Scene::kGame:
				gameScene = new GameScene();
				gameScene->Initialize();
				break;
			case Scene::kGaameOver:
				gameOver = new GameOver();
				gameOver->Initialize();
				break;
			}
		}

		// ★現在のシーンの更新 (毎フレーム)
		switch (currentSceneEnum) {
		case Scene::kTitle:
			if (titleScene != nullptr) {
				titleScene->Update();
			}
			break;
		case Scene::kStageSelect:
			if (stageSelectScene != nullptr) {
				stageSelectScene->Update();
			}
			break;
		case Scene::kGame1_2:
			if (gameScene1_2 != nullptr) {
				gameScene1_2->Update();
			}
			break;
		case Scene::kGame1_3:
			if (gameScene1_3 != nullptr) {
				gameScene1_3->Update();
			}
			break;
		case Scene::kGame2_1:
			if (gameScene2_1 != nullptr) {
				gameScene2_1->Update();
			}
			break;
		case Scene::kGame:
			if (gameScene != nullptr) {
				gameScene->Update();
			}
			break;
		case Scene::kGaameOver:
			if (gameOver != nullptr) {
				gameOver->Update();
			}
			break;
		}

		inguiManager->End();

		// 描画開始
		dxCommon->PreDraw();

		// ★現在のシーンの描画 (毎フレーム)
		switch (currentSceneEnum) {
		case Scene::kTitle:
			if (titleScene != nullptr) {
				titleScene->Draw();
			}
			break;
		case Scene::kStageSelect:
			if (stageSelectScene != nullptr) {
				stageSelectScene->Draw();
			}
			break;
		case Scene::kGame1_2:
			if (gameScene1_2 != nullptr) {
				gameScene1_2->Draw();
			}
			break;
		case Scene::kGame1_3:
			if (gameScene1_3 != nullptr) {
				gameScene1_3->Draw();
			}
			break;
		case Scene::kGame2_1:
			if (gameScene2_1 != nullptr) {
				gameScene2_1->Draw();
			}
			break;
		case Scene::kGame:
			if (gameScene != nullptr) {
				gameScene->Draw();
			}
			break;
		case Scene::kGaameOver:
			if (gameOver != nullptr) {
				gameOver->Draw();
			}
			break;
		}
		inguiManager->Draw();
		// 描画終了
		dxCommon->PostDraw();
	}

	// ループ終了後の最終解放処理
	// 最後に残っているシーンを解放
	switch (currentSceneEnum) {
	case Scene::kTitle:
		delete titleScene;
		titleScene = nullptr;
		break;
	case Scene::kStageSelect:
		delete stageSelectScene;
		stageSelectScene = nullptr;
		break;
	case Scene::kGame1_2:
		delete gameScene1_2;
		gameScene1_2 = nullptr;
		break;
	case Scene::kGame1_3:
		delete gameScene1_3;
		gameScene1_3 = nullptr;
		break;
	case Scene::kGame2_1:
		delete gameScene2_1;
		gameScene2_1 = nullptr;
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

	// エンジン終了処理
	FilePathSet();
	return 0;
}
