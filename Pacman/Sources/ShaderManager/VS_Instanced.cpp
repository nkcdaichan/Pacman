#include "pch.h"

#include "VS_Instanced.h"
// コンストラクタ：頂点シェーダーを読み込み、作成
VS_Instanced::VS_Instanced(const std::wstring & file, ID3D11Device1 * device)
{
  //シェーダーコードを格納するバイナリオブジェクトのポインタ
  Microsoft::WRL::ComPtr<ID3DBlob> blob;
  //ファイルからシェーダーコードを読み込み、blobに格納
  D3DReadFileToBlob(file.c_str(), &blob);
  //deviceを使って頂点シェーダーを作成
  device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, GetShaderPP());
  //入力レイアウトの定義
  const D3D11_INPUT_ELEMENT_DESC ied[] =
  {
    { "Position" ,0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
    { "Normal"   ,0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
    { "Color"    ,0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
    { "AdjPos"   ,0,DXGI_FORMAT_R32G32B32_FLOAT,1,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_INSTANCE_DATA,1 },
    { "SizeFlag" ,0,DXGI_FORMAT_R8_UINT,1,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_INSTANCE_DATA,1 }
  };
  //入力レイアウトを作成し、m_inputLayoutに格納
  device->CreateInputLayout(ied, (UINT)std::size(ied), blob->GetBufferPointer(), blob->GetBufferSize(), m_inputLayout.GetAddressOf());
}
//デストラクタ
VS_Instanced::~VS_Instanced()
{
}
//シェーダーをアクティブにする
void VS_Instanced::ActivateShader(ID3D11DeviceContext1* context)
{
  //入力レイアウト設定
  context->IASetInputLayout(m_inputLayout.Get());
  //頂点シェーダー設定
  context->VSSetShader(GetShader(), NULL, 0);
}
//定数バッファをバインドする
void VS_Instanced::BindConstantBuffers(ID3D11DeviceContext1* context, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers)
{
  if (constantBuffers && numberOfConstantBuffers > 0)
	//定数バッファを頂点シェーダーにバインド
    context->VSSetConstantBuffers(0, numberOfConstantBuffers, constantBuffers);
}
