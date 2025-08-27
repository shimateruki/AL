#pragma once

#include "KamataEngine.h"
#include <map>
#include <string>
#include <vector>

/// ---------------------------
/// マップチップの種類定義
/// ---------------------------
enum class MapChipType {
	kBlank_=0, // 何もない
	kDirt_ =1,  // 土ブロック
	kGrass_=2, // 草ブロック
	kGoal_=3,  // ゴール地点
	kSpike_=5, // トゲ（当たると死亡）
	kBreakable_=4, // ★ 追加: 壊れる床
	kJumpPad_= 6   // ★ キノコジャンプ台
};

/// ---------------------------
/// マップチップ配置データ構造
/// ---------------------------
struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

/// ---------------------------
/// マップチップCSV読み込み用の変換テーブル
/// ---------------------------
namespace {
std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank_},
    {"1", MapChipType::kDirt_ },
    {"2", MapChipType::kGrass_},
    {"3", MapChipType::kGoal_ },
    {"5", MapChipType::kSpike_},
	{"4", MapChipType::kBreakable_}, // ★ 追加: 壊れる床のマップチップ
    {"6", MapChipType::kJumpPad_} // ★ 追加: ジャンプ台のマップチップ
};
}

/// ---------------------------
/// マップチップ管理クラス
/// ---------------------------
class MapChipField {
public:
	/// マップ上のインデックス（X,Y）
	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	/// テクスチャUV矩形
	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};

	/// 指定インデックスのマップチップ種別を取得
	MapChipType GetMapChipTypeByindex(uint32_t xindex, uint32_t yIndex);

	/// マップデータのリセット
	void ResetMapChipData();

	/// CSVファイルからマップチップデータを読み込む
	void LoadMapChipCsv(const std::string& filePath);

	/// 指定インデックスのワールド座標を取得
	KamataEngine::Vector3 GetChipPositionIndex(uint32_t xindex, uint32_t yindex);

	/// マップの縦横チップ数を取得
	uint32_t GetNumBlockVirtcal() { return kNumBlockVirtical; }
	uint32_t GetNumBlockHorizonal() { return kNumBlockHorizonal; }
	float GetBlockWidth() { return kBlockWidth; }
	float GetBlockHeight() { return kBlockHeight; }/// 指定インデックスのマップチップを書き換える
	void SetMapChipType(uint32_t xIndex, uint32_t yIndex, MapChipType type);

	/// 座標からマップチップのインデックスを逆算して取得
	IndexSet GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position);

	/// インデックスから対応するUV矩形を取得
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

private:
	/// マップチップの配置データ本体
	MapChipData mapChipData_;

	/// ブロック1つのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;

	/// マップチップの縦横サイズ（チップ数）
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizonal = 100;
};