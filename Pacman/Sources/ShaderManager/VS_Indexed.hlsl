#include "Common.h"
//カメラのビューとプロジェクション行列を格納する定数バッファ
cbuffer cameraPerFrame : register(b0)
{
  //ビュー行列
  matrix view;
  //プロジェクション行列
  matrix projection;
};
//オブジェクトのワールド行列を格納する定数バッファ
cbuffer cameraPerObject : register(b1)
{
  //ワールド行列
  matrix world;
};
//頂点シェーダーのメイン関数
VS_Output main(VS_Input_Indexed input)
{
  VS_Output output;
  //入力頂点の位置をfloat4に変換(w成分を1.0に設定)
  float4 pos = float4(input.pos, 1.0f);
  //ワールド行列で頂点位置を変換
  pos = mul(pos, world);
  //ワールド座標系での位置
  output.pos_w = pos;
  //ビュー行列で変換
  pos = mul(pos, view);
  //ビュー座標系での位置
  output.pos_wv = pos;
  //プロジェクション行列で変換
  pos = mul(pos, projection);
  //クリップ空間での位置
  output.pos_wvp = pos;
  //入力頂点の法線をfloat4に変換(w成分を0.0に設定)
  float4 normal = float4(input.normal, 0.0f);
  //ワールド行列で法線を変換
  normal = mul(normal, world);
  //変換後の法線
  output.normal = normal.xyz;
  //入力頂点の色をそのまま出力にコピー
  output.color = input.color;
  //サイズフラグを初期化
  output.sizeFlag = 0;
  //シェーダーの出力を返す
  return output;
}
