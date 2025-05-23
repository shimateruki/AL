#include "MapChipField.h"
#include <fstream>
#include <sstream>


MapChipType MapChipField::GetMapChipTypeByindex(uint32_t xindex, uint32_t yindex) {
	if (xindex < 0 || kNumBlockHorizonal - 1 < xindex) {
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

	return KamataEngine::Vector3(kBlockWidth * xindex, kBlockHeight * (kNumBlockVirtical - 1 - yindex), 0); };

