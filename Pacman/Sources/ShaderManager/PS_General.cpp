#include "pch.h"

#include "PS_General.h"
//PS_General クラスのコンストラクタ
PS_General::PS_General(const std::wstring& file, ID3D11Device1* device)
{
  //シェーダーファイルの内容を読み込むためのバッファ
  Microsoft::WRL::ComPtr<ID3DBlob> blob;
  //ファイルを読み込み、blobに格納
  D3DReadFileToBlob(file.c_str(), &blob);
  //デバイスを使ってピクセルシェーダーを作成
  device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, GetShaderPP());
}
//シェーダーを有効化
void PS_General::ActivateShader(ID3D11DeviceContext1* context)
{
  //コンテキストにピクセルシェーダーをセット
  context->PSSetShader(GetShader(), NULL, 0);
}
//コンスタントバッファをバインドする
void PS_General::BindConstantBuffers(ID3D11DeviceContext1* context, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers)
{
   //未使用引数を明示的に無視するためのキャスト
  (void)context;
  (void)constantBuffers;
  (void)numberOfConstantBuffers;
}
