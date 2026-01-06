#include "GameStateManager.h"


GameStateManager* GameStateManager::GetInstance() {
	// 静的変数としてインスタンスを生成し、唯一のインスタンスを保証する
	static GameStateManager instance;
	return &instance;
}

void GameStateManager::SetStageClear(int stageID, bool isClear) { stageClearStatus_[stageID] = isClear; }

bool GameStateManager::IsStageClear(int stageID) const {
	// 指定されたステージIDがマップに存在するかを確認
	auto it = stageClearStatus_.find(stageID);
	if (it != stageClearStatus_.end()) {
		return it->second;
	}
	// 存在しない場合は、未クリアとして扱う
	return false;
}

void GameStateManager::SetPlayerStartPosition(const KamataEngine::Vector3& position) { playerStartPosition_ = position; }

const KamataEngine::Vector3& GameStateManager::GetPlayerStartPosition() const { return playerStartPosition_; }

void GameStateManager::SaveStarCoinRecord(int stageID, int count) {
	// 今までの記録より多かったら更新する
	if (count > starCoinRecords_[stageID]) {
		starCoinRecords_[stageID] = count;
	}
}

int GameStateManager::GetStarCoinRecord(int stageID) const {
	// 記録があれば返す、なければ0
	auto it = starCoinRecords_.find(stageID);
	if (it != starCoinRecords_.end()) {
		return it->second;
	}
	return 0;
}