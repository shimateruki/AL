#pragma once  
#include "KamataEngine.h"  
#include "input/Input.h"  
#include "imgui.h"


class ViewProjection; // 前方宣言  

class Player {  
public:  
   // 初期化  
   void Initialize(KamataEngine::Model* model, uint32_t textureHandle);  
   // 更新  
   void Update();  
   // 描画  
   void Draw(ViewProjection& viewProjection);  

private:  
   uint32_t textureHandle_ = 0;                  // テクスチャハンドル  
   KamataEngine::WorldTransform worldTransform_; // ワールド変換（おそら  
   KamataEngine::Model* model_ = nullptr;  
   KamataEngine::Input* input_ = KamataEngine::Input::GetInstance(); // 名前空間を明示して曖昧さを解消  
};
