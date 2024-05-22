#pragma once

namespace Global
{

//キャラクターの状態を表す列挙型
enum class Mode
{
  Chase,          //追いかけモード
  Scatter,        //散らばりモード
  Frightened      //おびえモード
};
//カメラの位置定数
constexpr DirectX::XMFLOAT3 frontCamera {10.5f, 5.0f, -2.5f};
constexpr DirectX::XMFLOAT3 upCamera {10.5f, 15.0f, 10.5f};
//ゲーム設定定数
constexpr uint8_t worldSize = 21;                            //ワールドのサイズ
constexpr uint8_t minFramesPerDirection = 3;                 //方向ごとの最小フレーム数
constexpr uint8_t phasesNum = 9;                             //フェーズの数
 
constexpr uint8_t rowBlinky = 0;                             //ブリンキーの行
constexpr uint8_t rowPinky = 1;                              //ピンキーの行
constexpr uint8_t rowInky = 2;                               //インキーの行
constexpr uint8_t rowClyde = 3;                              //クライドの行
constexpr uint8_t rowFrightened = 4;                         //おびえモードの行
constexpr uint8_t rowTransition = 5;                         //遷移モードの行
constexpr uint8_t rowDead = 6;                               //死亡モードの行
constexpr uint8_t numGhosts = 4;                             //ゴーストの数

//キャラクターの速度定数
constexpr float pacManSpeed = 0.1f;                           //パックマンの速度
constexpr float ghostSpeed = 0.08f;                           //ゴーストの速度
constexpr float pacManSize = 1.0f;                            //パックマンのサイズ
constexpr float ghostSize = 1.0f;                             //ゴーストのサイズ
constexpr float pacManHalfSize = pacManSize / 2.0f;           //パックマンの半分のサイズ

//頂点データ構造体
struct Vertex
{
  DirectX::XMFLOAT3 position;                                 //位置                     
  DirectX::XMFLOAT3 normal;                                   //法線
  DirectX::XMFLOAT3 color;                                    //色
  DirectX::XMFLOAT2 texCoord;                                 //テクスチャ座標
};
//フレームごとのカメラデータ構造体
struct CameraPerFrame
{
  DirectX::XMFLOAT4X4 view;                                    //ビュー行列
  DirectX::XMFLOAT4X4 projection;                              //射影行列
};
//アラインメントの確認
static_assert((sizeof(CameraPerFrame) % 16) == 0, "CameraPerFrame is not aligned!");

//オブジェクトごとのカメラデータ構造体
struct CameraPerObject
{
  //ワールド行列
  DirectX::XMFLOAT4X4 world;
};

//アラインメントの確認
static_assert((sizeof(CameraPerObject) % 16) == 0, "CameraPerObject is not aligned!");

//スプライト定数バッファ構造体
struct SpriteConstantBuffer
{
  uint32_t spriteX;                                             //スプライトのX座標
  uint32_t spriteY;                                             //スプライトのY座標
  uint32_t spriteSheetColumns;                                  //スプライトシートの列数
  uint32_t spriteSheetRows;                                     //スプライトシートの行数

  DirectX::XMFLOAT4 billboardSize_0_0_0;                        //ビルボードサイズ
};
//アラインメントの確認
static_assert((sizeof(SpriteConstantBuffer) % 16) == 0, "SpriteConstantBuffer is not aligned!");

//光源定数バッファ構造体
struct LightConstantBuffer
{
  //光源の値
  DirectX::XMFLOAT4 values;
};
//アラインメントの確認
static_assert((sizeof(LightConstantBuffer) % 16) == 0, "LightConstantBuffer is not aligned!");
}
