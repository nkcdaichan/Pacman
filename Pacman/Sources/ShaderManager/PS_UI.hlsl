//テクスチャを定義
Texture2D textureUI : register(t0);
//サンプラーステート定義
SamplerState samplerState : register(s0);
//position:頂点シェーダーから渡されるピクセルの位置(SV_POSITION)
//texCoord:テクスチャ座標 (TexCoord)
//シェーダーの出力 (SV_Target)
float4 main(float4 position : SV_POSITION, float2 texCoord : TexCoord) : SV_Target
{
  //指定されたテクスチャ座標でテクスチャをサンプリング
  return textureUI.Sample(samplerState, texCoord);
}
