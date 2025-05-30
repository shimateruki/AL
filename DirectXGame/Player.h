#pragma once
#include"KamataEngine.h"
#include "math.h"

//前方宣言
class MapChipField;

enum Corner {
	kRightBottom,//右下
	kLeftBottom, //左下
	kRightTop,//右上
	kLeftTop,//左上

	knumCorner// 要素数
};

//マップチップの当たり判定情報
struct CollisionMapInfo {
	KamataEngine::Vector3 isMovement; // プレイヤーの移動量
	bool isHitTop = false;
	bool isHitBottom = false;
	bool isHitLeft = false;
	bool isHitRight = false;
};


enum class LRDirection {
	kRight,
	kLeft
};

class Player
{
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, KamataEngine::Vector3& position);
	void Move();//移動処理
	void Update();
	void Draw();
	void ResolveCollision();
	const KamataEngine::WorldTransform &GetWorldTransformPlayer()const { return worldTransformPlayer_; }
	void SetMapChipField(MapChipField* mapChipField) { mapchipField_ = mapChipField; }
	void MapChipUp(CollisionMapInfo& info);
	void MapChipDown(CollisionMapInfo& info);

	void MapChipLeft(CollisionMapInfo& info);

	void MapChipRight(CollisionMapInfo& info);

	KamataEngine::Vector3 CarnerPosition(const KamataEngine::Vector3& center, Corner cornter);

	void ground(const CollisionMapInfo& info);

	KamataEngine::Vector3 GetVelosity() { return velosity_; }
		;

private:
	KamataEngine::WorldTransform worldTransformPlayer_;
	KamataEngine::WorldTransform playerModel_;
	KamataEngine:: Model*  model_ = nullptr;
	uint32_t textureHandle_ = 0;
	KamataEngine::Camera* camera_ = nullptr;
	
	Math* math;
	KamataEngine::Vector3 velosity_ = {};
	static inline const float kAcceleration = 0.01f;//加速度
	static inline const float kAtteunuation = 0.1f;//速度減少量
	static inline const float kLimitRunSpeed = 5.0f;//最大速度
	LRDirection lrDirection_ = LRDirection::kRight;//方向
	float turnFirstRottationY_ = 0.0f;//旋回開始時の角度
	float turnTimer_ = 0.0f;//旋回タイマー
	static inline const float kTimeTurn = 0.3f;//旋回時間
	bool onGround_ = true;//着地フラグ
	static inline const float kGgravityAcceleration = 9.8f;// 重力加速度
	static inline const float kLimitFallSpeed = 5.0f;//最大落下加速度
	static inline const float kJumpAccleration = 1.0f;     // ジャンプ加速

	// キャラクターの当たり判定
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	static inline const float kBlank = 0.0f;

	
    static inline const float smallnumber = 0.01f;

	
	//マップチップフィールド着地時の速度減少率
	static inline const float kAttenuationLoading = 0.1f;

	MapChipField* mapchipField_ = nullptr;
};
