#pragma once
#include <map>
#include "KamataEngine.h"
using namespace KamataEngine;

class GameStateManager 
{
private:

	    int currentStageID_ = 0; // 現在のステージIDを保存する変数
	// シングルトン化のためのコンストラクタ
	GameStateManager() = default;
	~GameStateManager() = default;
	GameStateManager(const GameStateManager&) = delete;
	GameStateManager& operator=(const GameStateManager&) = delete;


public:
	// 唯一のインスタンスを取得する静的メソッド
	static GameStateManager* GetInstance();

	// ステージクリア情報を設定・取得するメソッド
	void SetStageClear(int stageID, bool isClear);
	bool IsStageClear(int stageID) const;

	void SetCurrentStageID(int stageID) { currentStageID_ = stageID; }
	int GetCurrentStageID() const { return currentStageID_; }

	// プレイヤーの次の初期位置を設定・取得する
	void SetPlayerStartPosition(const KamataEngine::Vector3& position);
	const KamataEngine::Vector3& GetPlayerStartPosition() const;
	// スターコインの記録を保存する（多かったら更新）
	void SaveStarCoinRecord(int stageID, int count);

	// ：そのステージのコイン記録を取得する
	int GetStarCoinRecord(int stageID) const;

private:
	// ステージIDとクリア状態を管理するマップ
	std::map<int, bool> stageClearStatus_;

	    // プレイヤーの次のステージでの初期位置
	KamataEngine::Vector3 playerStartPosition_ = {};

	std::map<int, int> starCoinRecords_;

};
