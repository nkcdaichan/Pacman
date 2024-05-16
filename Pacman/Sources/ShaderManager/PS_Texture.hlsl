#include "Common.h"
//2Dテクスチャを定義し,シェーダーリソースビュー(t0)にバインド
Texture2D     dotTexture    : register(t0);
//サンプラーステートを定義し,サンプラーステートビュー(s0)にバインド
SamplerState  samplerState  : register(s0);

//入力: Geometry Shader(GS)からの出力を受け取る
//出力: ピクセルカラー(SV_Target)を返す
float4 main(GS_Output input) : SV_Target
{ 
  // テクスチャ座標でテクスチャをサンプリングし,得られた色を返す
  return dotTexture.Sample(samplerState, input.texCoord);
}