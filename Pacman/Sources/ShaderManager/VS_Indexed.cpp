#include "pch.h"

#include "VS_Indexed.h"
//コンストラクタ ファイルパスとデバイスを受け取る
VS_Indexed::VS_Indexed(const std::wstring& file, ID3D11Device1* device)
{
  //コンパイル済みシェーダーデータを保持するためのblobオブジェクト
  Microsoft::WRL::ComPtr<ID3DBlob> blob;
  //ファイルからシェーダーバイナリを読み込んでblobに格納

  D3DReadFileToBlob(file.c_str(), &blob);
  //読み込んだシェーダーバイナリから頂点シェーダーを作成
  device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, GetShaderPP());

  //入力レイアウトの定義
  const D3D11_INPUT_ELEMENT_DESC ied[] =
  {
    { "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 }, // 位置情報
    { "Normal"  ,0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 }, // 法線ベクトル
    { "Color"   ,0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 }, // 色
    { "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT   ,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 }  // テクスチャ座標
  };
  //入力レイアウトの作成
  device->CreateInputLayout(ied, (UINT)std::size(ied), blob->GetBufferPointer(), blob->GetBufferSize(), m_inputLayout.GetAddressOf());
}
//デストラクタ
VS_Indexed::~VS_Indexed()
{
}
//シェーダーをアクティブにするメソッド
void VS_Indexed::ActivateShader(ID3D11DeviceContext1* context)
{
  //入力レイアウトを設定
  context->IASetInputLayout(m_inputLayout.Get());
  //頂点シェーダーを設定
  context->VSSetShader(GetShader(), NULL, 0);
}
//コンスタントバッファをバインドするメソッド
void VS_Indexed::BindConstantBuffers(ID3D11DeviceContext1* context, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers)
{
  //有効なバッファとバッファ数があるか確認
  if (constantBuffers && numberOfConstantBuffers > 0)
	 //コンスタントバッファをバインド
    context->VSSetConstantBuffers(0, numberOfConstantBuffers, constantBuffers);
}
