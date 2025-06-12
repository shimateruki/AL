#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include <Windows.h>
// 必要に応じて Input.h も追加
// #include "Input.h"

using namespace KamataEngine;

// IScene インターフェースの定義を推奨します。（もしあれば使ってください）
// TitleScene と GameScene が IScene を継承していると仮定します。
// 例:
// class IScene {
// public:
//     virtual ~IScene() = default;
//     virtual void Initialize() = 0;
//     virtual void Update() = 0;
//     virtual void Draw() = 0;
//     virtual bool isFinished() = 0; // isfinished と isFinished の両方がある場合、どちらかに統一を推奨
// };

enum class Scene {
	kUnknown = 0, // kunknown のtypoを修正
	kTitle,
	kGame,
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	// エンジンの初期化
	Initialize(L"LE2C_13_シマ_テルキ_AL3");

	ImGuiManager* inguiManager = ImGuiManager::GetInstance();
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// シーンのポインタを管理する変数
	// IScene を使用しない場合、以下のように個別管理します。
	GameScene* gameScene = nullptr;
	TitleScene* titleScene = nullptr;

	// 現在のシーンの状態
	Scene currentSceneEnum = Scene::kTitle;

	// 初期シーンの設定
	titleScene = new TitleScene();
	titleScene->Initialize();

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
			// TitleScene::Update() をここで呼ぶ前に isfinished をチェックするのは不適切。
			// まず更新してから、その結果で終了判定をするべきです。
			// または、Update()内で終了判定を自身で行い、ReadyToChangeScene()のようなフラグを返す。
			// 今回はシンプルに、Update()後にチェックする形で進めます。
			// ※isFinished()とisfinished()のtypoが混在しているので、どちらかに統一してください。
			if (titleScene != nullptr && titleScene->isfinished()) { // タイトルシーンが終了条件を満たしたら
				nextSceneEnum = Scene::kGame;                        // 次のシーンはゲームシーン
			}
			break;
		case Scene::kGame:
			if (gameScene != nullptr && gameScene->isFinished()) { // ゲームシーンが終了条件を満たしたら
				nextSceneEnum = Scene::kTitle;                     // 次のシーンはタイトルシーン
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
			case Scene::kGame:
				delete gameScene;
				gameScene = nullptr;
				break;
			}

			// 次のシーンを初期化
			currentSceneEnum = nextSceneEnum;
			switch (currentSceneEnum) {
			case Scene::kTitle:
				titleScene = new TitleScene();
				titleScene->Initialize();
				break;
			case Scene::kGame:
				gameScene = new GameScene();
				gameScene->Initialize();
				break;
			}
		}

		// ★現在のシーンの更新 (毎フレーム)
		switch (currentSceneEnum) {
		case Scene::kTitle:
			if (titleScene != nullptr) { // 念のためnullptrチェック
				titleScene->Update();
			}
			break;
		case Scene::kGame:
			if (gameScene != nullptr) { // 念のためnullptrチェック
				gameScene->Update();
			}
			break;
		}

		inguiManager->End();

		// 描画開始
		dxCommon->PreDraw();

		// ★現在のシーンの描画 (毎フレーム)
		switch (currentSceneEnum) {
		case Scene::kTitle:
			if (titleScene != nullptr) { // 念のためnullptrチェック
				titleScene->Draw();
			}
			break;
		case Scene::kGame:
			if (gameScene != nullptr) { // 念のためnullptrチェック
				gameScene->Draw();
			}
			break;
		}

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
	case Scene::kGame:
		delete gameScene;
		gameScene = nullptr;
		break;
	}

	// エンジン終了処理
	FilePathSet();
	return 0;
}