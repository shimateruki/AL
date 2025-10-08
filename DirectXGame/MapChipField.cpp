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
	return mapChipData_.data[yindex][xindex];
}

/// -------------------------------------------
/// マップチップデータを全て空に初期化する
/// -------------------------------------------
void MapChipField::ResetMapChipData() {
	mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockVirtical);
	for (std::vector<MapChipType>& mapChipDataLine : mapChipData_.data) {
		mapChipDataLine.resize(kNumBlockHorizonal);
	}
}

/// -------------------------------------------
/// CSVファイルからマップチップデータを読み込む
/// -------------------------------------------
void MapChipField::LoadMapChipCsv(const std::string& filePath) {
	// マップチップデータのリセット
	ResetMapChipData();

	// ファイルを開く
	std::ifstream file(filePath);
#ifdef _DEBUG
	assert(file.is_open());
#endif

	std::string line;
	uint32_t yIndex = 0;

	while (std::getline(file, line) && yIndex < kNumBlockVirtical) {
		std::istringstream lineStream(line);
		std::string word;
		uint32_t xIndex = 0;

		while (std::getline(lineStream, word, ',') && xIndex < kNumBlockHorizonal) {
			// 前後の全角・半角空白、改行、BOMを削除
			word.erase(0, word.find_first_not_of(" \t\r\n\xEF\xBB\xBF"));
			word.erase(word.find_last_not_of(" \t\r\n\xEF\xBB\xBF") + 1);

			// 数値として読み取る（全角数字や空白付きもOK）
			int num = 0;
			try {
				num = std::stoi(word);
			} catch (...) {
				num = 0; // 読み取れない場合は空白扱い
			}

			// enum変換（CSVの数字とMapChipTypeの順番を一致させている前提）
			if (num >= 0 && num <= static_cast<int>(MapChipType::kIceFloor_)) {
					mapChipData_.data[yIndex][xIndex] = static_cast<MapChipType>(num);
				} else {
					mapChipData_.data[yIndex][xIndex] = MapChipType::kBlank_;
				}

			xIndex++;
		}
		yIndex++;
	}

	file.close();

	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizonal; ++j) {
	
		}
		std::cout << "\n";
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

