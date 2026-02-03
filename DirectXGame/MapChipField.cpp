#include "MapChipField.h"
#include <fstream>
#include <iostream>
#include <sstream>
using namespace KamataEngine;

/// -------------------------------------------
/// 指定されたインデックスのマップチップの種類を返す
/// 範囲外の場合は空白チップを返す
/// -------------------------------------------
MapChipType MapChipField::GetMapChipTypeByindex(uint32_t xindex, uint32_t yindex) {
	if (xindex >= kNumBlockHorizonal || yindex >= kNumBlockVirtical) {
		return MapChipType::kBlank_;
	}

	if (xindex >= mapChipData_.data[yindex].size()) {
		return MapChipType::kBlank_;
	}
	return mapChipData_.data[yindex][xindex];
}

/// -------------------------------------------
/// マップチップデータを全て空に初期化する
/// -------------------------------------------
void MapChipField::ResetMapChipData() {
	mapChipData_.data.clear();
}

/// -------------------------------------------
/// CSVファイルからマップチップデータを読み込む
/// -------------------------------------------
void MapChipField::LoadMapChipCsv(const std::string& filePath) {
	// データを一度クリア
	mapChipData_.data.clear();

	// ファイルを開く
	std::ifstream file(filePath);
	if (!file.is_open()) {
		return;
	}

	std::string line;

	// ★ 読み込みながら、動的にデータを追加していく
	while (std::getline(file, line)) {
		std::stringstream line_stream(line);
		std::string word;
		std::vector<MapChipType> row;

		while (std::getline(line_stream, word, ',')) {
			MapChipType type = MapChipType::kBlank_;
			if (mapChipTable.contains(word)) {
				type = mapChipTable[word];
			}
			row.push_back(type);
		}
		// 1行分のデータを追加
		mapChipData_.data.push_back(row);
	}

	// ★ 読み込んだデータのサイズに合わせて、変数を更新！
	kNumBlockVirtical = static_cast<uint32_t>(mapChipData_.data.size());
	if (kNumBlockVirtical > 0) {
		kNumBlockHorizonal = static_cast<uint32_t>(mapChipData_.data[0].size());
	} else {
		kNumBlockHorizonal = 0;
	}
}
/// -------------------------------------------
/// チップインデックスからワールド座標を取得
/// -------------------------------------------
KamataEngine::Vector3 MapChipField::GetChipPositionIndex(uint32_t xIndex, uint32_t yIndex) { return Vector3(kBlockWidth * xIndex, kBlockHeight * (kNumBlockVirtical - 1 - yIndex), 0); }

void MapChipField::SetMapChipType(uint32_t xIndex, uint32_t yIndex, MapChipType type) 
{
	if (yIndex < mapChipData_.data.size() && xIndex < mapChipData_.data[yIndex].size()) {
		mapChipData_.data[yIndex][xIndex] = type;
	}
}

/// -------------------------------------------
/// ワールド座標からチップのインデックスを逆算して取得
/// -------------------------------------------
MapChipField::IndexSet MapChipField::GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position) {
	IndexSet indexSet = {};
	indexSet.xIndex = static_cast<uint32_t>((position.x + kBlockWidth / 2.0f) / kBlockWidth);
	indexSet.yIndex = kNumBlockVirtical - 1 - static_cast<uint32_t>((position.y + kBlockHeight / 2.0f) / kBlockHeight);
	return indexSet;
}

/// -------------------------------------------
/// インデックスからUVの矩形（左・右・上・下）を取得
/// 描画などで使用される
/// -------------------------------------------
MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) {
	// 中心座標を取得
	Vector3 center = GetChipPositionIndex(xIndex, yIndex);

	// チップのサイズから矩形を設定
	Rect rect;
	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f;
	rect.top = center.y + kBlockHeight / 2.0f;
	rect.bottom = center.y - kBlockHeight / 2.0f;

	return rect;
}

