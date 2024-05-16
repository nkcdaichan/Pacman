#include "Common.h"

//定数として定義されたマテリアルカラー
static const float3 materialColor = { 0.7f, 0.7f, 0.9f };
//環境光の色
static const float3 ambient = { 0.15f, 0.15f, 0.15f };
//拡散反射光の色
static const float3 diffuseColor = { 1.0f, 1.0f, 1.0f };
//拡散反射光の強度
static const float diffuseIntensity = 1.0f;
//ライトの減衰係数
//定数減衰
static const float attConst = 1.0f;
//線形減衰
static const float attLin = 0.045f;
//二次減衰
static const float attQuad = 0.0075f;
//ライトのパラメータを格納する定数バッファ、レジスタb0に配置
cbuffer lightParams : register(b0)
{
	//ライトの位置（w成分は使われない）
  float4 lightPos;
};
//ピクセルシェーダーのエントリーポイント
float4 main(VS_Output input) : SV_Target
{
	//フラグメント(ピクセル)からライトへのベクトル
  const float3 vToL = lightPos.xyz - input.pos_w.xyz;
    //フラグメントからライトまでの距離
  const float distToL = length(vToL);
    //フラグメントからライトへの単位ベクトル
  const float3 dirToL = vToL / distToL;

  //減衰係数の計算
  const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));

  //拡散反射の強度計算
  const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f,dot(dirToL, input.normal));

  //最終的な色の計算(環境光と拡散反射を合計し、マテリアルカラーを掛け合わせる）
  return float4(saturate((diffuse + ambient) * materialColor),1.0f);
}
