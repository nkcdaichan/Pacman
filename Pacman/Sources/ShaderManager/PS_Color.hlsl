#include "Common.h"

//ピクセルシェーダーのエントリーポイント
//GS_Output型のinputを引数に取り、float4型を返す
//float4型は、4つの浮動小数点数を含むベクトルを表す。
float4 main(GS_Output input) : SV_Target
{
  //input.colorのRGB値にアルファ値1.0fを追加してfloat4型の値を返す。
  //不透明な色がピクセルシェーダーへ
  return float4(input.color, 1.0f);
}