#pragma once
#include "KamataEngine.h"
#include "MapChipField.h"
#include "math.h"

using namespace KamataEngine;
class BreakableFloor {
public:
	void Initialize(const KamataEngine::Vector3& pos, uint32_t x, uint32_t y, MapChipField* field);

	void OnStepped();                                                    // 踏まれたとき呼ばれる
	void Update();                                                       // タイマー進行＆消滅処理
	void Draw(KamataEngine::Model* model, KamataEngine::Camera* camera); // 描画用

	AABB GetAABB() const; // 当たり判定用

	// 外から行列参照できるようにする
	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }
	KamataEngine::WorldTransform& GetWorldTransform() { return worldTransform_; }
	bool IsBroken() const { return isBroken_; }

	bool IsAlive() const { return !isBreaking_ || timer_ < kBreakTime; } // 消滅チェック用
	uint32_t GetIndexX() const {
		
		return xIndex_;
	}
	uint32_t GetIndexY() const { return yIndex_; }

private:
	KamataEngine::WorldTransform worldTransform_; // ← 追加！
	uint32_t xIndex_ = 0;
	uint32_t yIndex_ = 0;

	bool isBreaking_ = false;
	int timer_ = 0;

	MapChipField* mapChipField_ = nullptr;
	Math* math = nullptr; // 数学ユーティリティ

	static inline const int kBreakTime = 90; // 60フレームで壊れる
	static inline const float kWidth = 1.0f;
	static inline const float kHeight = 1.0f;
	KamataEngine::ObjectColor color_;

	bool isBroken_ = false; // 完全に壊れたかどうか 
};