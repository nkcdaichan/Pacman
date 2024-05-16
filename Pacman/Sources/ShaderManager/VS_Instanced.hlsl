#include "Common.h"
//カメラに関する定数(フレームごとに更新）
cbuffer cameraPerFrame : register(b0)
{
  // ビュー行列 カメラの視点
  matrix view;
  //プロジェクション行列 カメラの投影変換
  matrix projection;
};
//カメラに関する定数(オブジェクトごとに更新）
cbuffer cameraPerObject : register(b1)
{
  //ワールド行列 オブジェクトの変換
  matrix world;
};
//頂点シェーダーのエントリポイント
VS_Output main(VS_Input_Instanced input)
{
  //出力構造体
  VS_Output output;
  //頂点位置をfloat4型に変換、w成分は1.0fに設定
  float4 pos = float4(input.adjPos, 1.0f);
  //ワールド行列で頂点位置を変換
  pos = mul(pos, world);
  //ワールド変換後の頂点位置を出力
  output.pos_w = pos;
  //プロジェクション行列で頂点位置を変換
  pos = mul(pos, view);
  //ワールドビュー射影変換後の頂点位置を出力
  output.pos_wv = pos;
  //プロジェクション行列で頂点位置を変換
  pos = mul(pos, projection);
  //ワールドビュー射影変換後の頂点位置を出力
  output.pos_wvp = pos;

  //その他の頂点属性を出力にコピー
  //頂点カラー
  output.color = input.color;
  //頂点法線
  output.normal = input.normal;
  //サイズフラグ
  output.sizeFlag = input.sizeFlag;
  //変換された頂点データを返す
  return output;
}
