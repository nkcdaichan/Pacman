#include "pch.h"

#include "PS_OneCB.h"
//コンストラクタ　シェーダーファイルを読み込み,ピクセルシェーダーを作成
PS_OneCB::PS_OneCB(const std::wstring& file, ID3D11Device1* device)
{
  //シェーダーデータを格納するためのID3DBlobインターフェイスへのスマートポインタ
  Microsoft::WRL::ComPtr<ID3DBlob> blob;
  //シェーダーファイルを読み込んでblobに格納
  D3DReadFileToBlob(file.c_str(), &blob);
  //デバイスを使用してピクセルシェーダーを作成
  device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, GetShaderPP());
}
//ピクセルシェーダーをコンテキストに設定してアクティブに
void PS_OneCB::ActivateShader(ID3D11DeviceContext1 * context)
{
  //ピクセルシェーダーを設定する
  context->PSSetShader(GetShader(), NULL, 0);
}
//定数バッファのバインドする
void PS_OneCB::BindConstantBuffers(ID3D11DeviceContext1* context, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers)
{
  //定数バッファが有効かつバッファの数が0より多い場合にバインドを行う
  if (constantBuffers && numberOfConstantBuffers > 0)
	//定数バッファを設定する
    context->PSSetConstantBuffers(0, numberOfConstantBuffers, constantBuffers);
}
