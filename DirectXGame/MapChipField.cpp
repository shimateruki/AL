#include "MapChipField.h"
#include <fstream>
#include <sstream>
using namespace KamataEngine;

MapChipType MapChipField::GetMapChipTypeByindex(uint32_t xindex, uint32_t yindex) {
	if (xindex >= kNumBlockHorizonal || yindex >= kNumBlockVirtical){
		return MapChipType::kBlank_;
	}
	if (yindex < 0 || kNumBlockHorizonal - 1 < yindex) {
		return MapChipType::kBlank_;
	}
	return mapChipData_.data[yindex][xindex];
}

void MapChipField::ResetMapChipData() { mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockVirtical);
	for ( std::vector<MapChipType>& mapChipDataLine: mapChipData_.data) 
	{
		mapChipDataLine.resize(kNumBlockHorizonal);
	}
}

void MapChipField::LoadMapChipCsv(const std::string& filePath) {
	//マップチップデータのリセット
	ResetMapChipData();
	//ファイルを開く
	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());
	//マップチップcsv
	std::stringstream mapChipCsv;
	mapChipCsv << file.rdbuf();
	file.close();
	//csvからマップチップを読み込む
	for (int i = 0; i < kNumBlockVirtical; ++i) {
		std::string line;
		getline(mapChipCsv, line);
		std::istringstream line_stram(line);
		for (uint32_t j = 0; j < kNumBlockHorizonal; j++) {
			std::string word;
			getline(line_stram, word, ',');

			if (mapChipTable.contains(word)) {
				mapChipData_.data[i][j] = mapChipTable[word];
			}
		}
	}


}

KamataEngine::Vector3 MapChipField::GetChipPositionIndex(uint32_t xindex, uint32_t yindex) {

	return KamataEngine::Vector3(kBlockWidth * xindex, kBlockHeight * (kNumBlockVirtical - 1 - yindex), 0); }
IndexSet MapChipField::GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position) 
{
	IndexSet indexSet = {};
	indexSet.xIndex = static_cast<uint32_t>(position.x / kBlockWidth);
	indexSet.yIndex = static_cast<uint32_t>(kNumBlockVirtical - 1 - static_cast<int>(position.y / kBlockHeight));
	return indexSet;
};


Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) {
	// 中心座標を取得
	Vector3 center = GetChipPositionIndex(xIndex, yIndex);

	Rect rect;
	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f;
	rect.top = center.y - kBlockHeight / 2.0f;
	rect.bottom = center.y + kBlockHeight / 2.0f;

	return rect;
}

