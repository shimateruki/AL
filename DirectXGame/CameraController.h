#pragma once
#include "KamataEngine.h"
#include "math.h"
#include <algorithm>
#include <numbers> // ★ EaseInOutSine で使うため

class Player;

class CameraController {
public:
	// ★ カメラの状態を定義
	enum class Mode {
		kFollowPlayer, // 通常の追従
		kVictoryZoom   // 勝利ズーム演出
	};

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
	void SetMovableSrea(Rect area) { movebleArea_ = area; }

	// 勝利ズーム演出を開始する命令
	void StartVictoryZoom(Player* target);
private:
	// ▼▼▼ 以下を追記 ▼▼▼
	// ズーム演出の更新処理
	void UpdateVictoryZoom();
	// ▲▲▲▲▲▲▲▲▲▲▲▲▲▲

	KamataEngine::Camera* camera_;
	Player* target_ = nullptr;
	KamataEngine::Vector3 targettooffset = {0, 0, -15.0f};
	Rect movebleArea_ = {0, 100, 0, 100};
	KamataEngine::Vector3 destination_;
	static inline const float kInterpolationRate = 0.1f;
	static inline const float kVelocityBias = 10.0f;
	static inline const Rect targetMargin = {-9.0f, 9.0f, -5.0f, 5.0f};

	Math* math_ = nullptr; // ★ 初期化されていないので修正が必要

	Mode mode_ = Mode::kFollowPlayer; // 現在のカメラモード
	float zoomTimer_ = 0.0f;
	float kZoomDuration = 1.5f;                // ズームにかかる時間（秒）
	KamataEngine::Vector3 zoomStartPos_ = {};  // ズーム開始時のカメラ位置
	KamataEngine::Vector3 zoomTargetPos_ = {}; // ズーム目標のカメラ位置
};