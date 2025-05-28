#pragma once
#include "KamataEngine.h"
#include "math.h"
#include <algorithm>
class Player;




class CameraController 
{
public:
	struct Rect {

		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};
	void Initialize(KamataEngine::Camera* camera);

	void Update();

	void SetTarget(Player* target) { target_ = target; }

	void Reset();

	void SetMovableSrea(Rect area) { movebleArea_ =  area; }

private:

	KamataEngine::Camera* camera_;
	Player* target_ = nullptr;
	//追従対象とカメラの座標カメラの差
	KamataEngine::Vector3 targettooffset = {0, 0, -15.0f};
	Rect movebleArea_ = {0, 100, 0, 100};
	//目標座標
	KamataEngine::Vector3 destination_;
	//  座標補間割合
	static inline const float kInterpolationRate = 0.1f;
	//  速度掛け率
	static inline const float kVelocityBias = 30.0f;
	//  追従対象の各方向へのカメラ移動範囲
	static inline const Rect targetMargin = {-9.0f, 9.0f, -5.0f, 5.0f};

Math* math_;
	
};
