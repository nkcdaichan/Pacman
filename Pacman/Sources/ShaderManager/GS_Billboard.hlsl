#include "Common.h"

// 投影行列の定数バッファ（スロットb0）
cbuffer ProjectionMatrix : register(b0)
{
  // 投影行列
  matrix projection;
}

// フレームごとの定数バッファ（スロットb1）
cbuffer FrameConstantBuffer : register(b1)
{
  //Xインデックス
  uint spriteX;
  //Yインデックス
  uint spriteY;
  //シートの列数
  uint spriteSheetColumns;
  //シートの行数
  uint spriteSheetRows;
  //ビルボードサイズ
  float4 billboardSize_0_0_0;
};
//メイン関数(最大頂点数を4に設定)
[maxvertexcount(4)]
void main(point VS_Output input[1], inout TriangleStream<GS_Output> outputStream)
{
	//GS_Outputのインスタンスを作成し、入力からの値を設定
  GS_Output a;
  // 入力の色を設定
  a.color = input[0].color;
  // 入力の法線を設定
  a.normal = input[0].normal;
  // 入力の位置をワールドビュー空間で設定
  a.pos = input[0].pos_wv;
  // 4つの頂点を作成
  GS_Output b = a;
  GS_Output c = a;
  GS_Output d = a;

  //サイズを初期化
  float billboardSize = 0;
  //ビルボードサイズを設定
  if (input[0].sizeFlag == 1)
    billboardSize = billboardSize_0_0_0.x;
  else if (input[0].sizeFlag == 2)
    billboardSize = billboardSize_0_0_0.y;

  //4つの頂点の位置を計算
  a.pos.xy += float2(-0.5, -0.5) * billboardSize;
  b.pos.xy += float2(0.5, -0.5) * billboardSize;
  c.pos.xy += float2(-0.5, 0.5) * billboardSize;
  d.pos.xy += float2(0.5, 0.5) * billboardSize;

  //投影行列を使って頂点位置を変換
  a.pos = mul(a.pos, projection);
  b.pos = mul(b.pos, projection);
  c.pos = mul(c.pos, projection);
  d.pos = mul(d.pos, projection);

  //スプライトシートのオフセットを計算
  float xOffset = 1.0f / spriteSheetColumns;
  float yOffset = 1.0f / spriteSheetRows;

  //テクスチャ座標を設定
  a.texCoord = float2(spriteX * xOffset, (spriteY + 1) * yOffset);
  b.texCoord = float2((spriteX + 1) * xOffset, (spriteY + 1) * yOffset);
  c.texCoord = float2(spriteX * xOffset, spriteY * yOffset);
  d.texCoord = float2((spriteX + 1) * xOffset, spriteY * yOffset);

  //計算した頂点をoutputStreamに追加
  outputStream.Append(a);
  outputStream.Append(b);
  outputStream.Append(c);
  outputStream.Append(d);
}
