#pragma once  
#include "KamataEngine.h"  
#include "Player.h" 
#include "Math.h" 
#include "ViewProjection.h"
#include "input/Input.h"
//ゲームシーン  
class GameScene  
{  
public:  

//初期化  
void Initialize();  

//更新  
void Update();  

//描画  
void Draw();  
~GameScene();  
private:  
uint32_t textureHandel_ = 0;                  // テクスチャハンドル  
KamataEngine::WorldTransform worldTransform_; // ワールド変換（おそらく単一のオブジェクト用、現状未使用の可能性あり）  
Player player_; // プレイヤーオブジェクト 
KamataEngine::Model* model_ = nullptr; // モデル
//ビュープロジェクション  
ViewProjection viewProjection_; // ビュープロジェクション変換  
bool isDebugCameraActive_ = false;          // デバッグモードフラグ（未使用）
KamataEngine::DebugCamera* debaucamera_ = nullptr; // デバッグカメラのインスタンス

};