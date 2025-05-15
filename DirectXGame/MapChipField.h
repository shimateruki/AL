#pragma once
#include "KamataEngine.h"
#include<map>

enum class MapChipType {
	kBlank_,kBlock_
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};
namespace {
std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank_},
    {"1", MapChipType::kBlock_},
};
}

class MapChipField
{
public:
	MapChipType GetMapChipTypeByindex(uint32_t xindex, uint32_t yIndex);
	void ResetMapChipData();
	void LoadMapChipCsv(const std::string& filePath);
	KamataEngine::Vector3 GetChipPositionIndex(uint32_t xindex, uint32_t yindex);

	private:
	MapChipData mapChipData_;
	static inline const float kBlockWidth = 1.0f;
	    static inline const float kBlockHeight = 1.0f;
	static inline const uint32_t kNumBlockVirtical = 30;
	    static inline const uint32_t kNumBlockHorizonal = 100;

};
