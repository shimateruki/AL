#pragma once
#include "KamataEngine.h"
#include<map>

enum class MapChipType {
	kBlank_,kBlock_
};

struct IndexSet
{
	uint32_t xIndex;
	uint32_t yIndex;
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

struct Rect {

	float left = 0.0f;
	float right = 1.0f;
	float bottom = 0.0f;
	float top = 1.0f;
};

class MapChipField
{
public:
	MapChipType GetMapChipTypeByindex(uint32_t xindex, uint32_t yIndex);
	void ResetMapChipData();
	void LoadMapChipCsv(const std::string& filePath);
	KamataEngine::Vector3 GetChipPositionIndex(uint32_t xindex, uint32_t yindex);

	//Get関数
	uint32_t GetNumBlockVirtcal() { return kNumBlockVirtical; }
	uint32_t GetNumBlockHorizonal( ){ return kNumBlockHorizonal; }
	IndexSet GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position);

	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

	private:
	MapChipData mapChipData_;
	static inline const float kBlockWidth = 1.0f;
	    static inline const float kBlockHeight = 1.0f;
	static inline const uint32_t kNumBlockVirtical = 20;
	    static inline const uint32_t kNumBlockHorizonal = 100;

};
