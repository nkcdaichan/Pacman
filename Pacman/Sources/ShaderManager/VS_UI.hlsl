#include "Common.h"
//カメラに関連する行列データを保持する
cbuffer cameraPerFrame : register(b0)
{
  //ビュー行列
  matrix view;
  //プロジェクション行列
  matrix projection;
};
//オブジェクトに関連する行列データを保持する
cbuffer cameraPerObject : register(b1)
{
  //ワールド行列
  matrix world;
};
//頂点シェーダーの出力構造体
struct VOut
{
  //頂点の変換後の位置
  float4 position : SV_POSITION;
  //頂点のテクスチャ座標
  float2 texCoord : TexCoord;
};
//頂点シェーダーのメイン関数
VOut main(VS_Input_Indexed input)
{
  //出力構造体のインスタンス
  VOut output;
  //入力頂点の位置をfloat4に変換
  output.position = float4(input.pos, 1.0f);
  //ワールド行列で頂点位置を変換
  output.position = mul(output.position, world);
  //ビュー行列で頂点位置を変換
  output.position = mul(output.position, view);
  //プロジェクション行列で頂点位置を変換
  output.position = mul(output.position, projection);
  //テクスチャ座標を出力に設定
  output.texCoord = input.texCoord;

  return output;
}
