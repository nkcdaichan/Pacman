#include "pch.h"

#include "GS_General.h"
//GS_Generalクラスのコンストラクタ
GS_General::GS_General(const std::wstring & file, ID3D11Device1* device)
{
  //シェーダーバイナリを格納するためのID3DBlobインターフェースのポインタを作成
  Microsoft::WRL::ComPtr<ID3DBlob> blob;
  //指定されたファイルからシェーダーバイナリを読み込んでblobに格納
  D3DReadFileToBlob(file.c_str(), &blob);
  //デバイスを使用してジオメトリシェーダーを作成し、メンバ変数に保存
  device->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, GetShaderPP());
}
//GS_Generalクラスのデストラクタ
GS_General::~GS_General()
{
}
//ジオメトリシェーダーをアクティブにするメソッド
void GS_General::ActivateShader(ID3D11DeviceContext1* context)
{
  //コンテキストを使用してジオメトリシェーダーを設定
  context->GSSetShader(GetShader(), NULL, 0);
}
//コンスタントバッファをバインドするメソッド
void GS_General::BindConstantBuffers(ID3D11DeviceContext1* context, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers)
{
  //コンスタントバッファが存在し、かつその数が0より大きい場合にのみ実行
  if (constantBuffers && numberOfConstantBuffers > 0)
	// コンテキストを使用してジオメトリシェーダーのコンスタントバッファを設定
    context->GSSetConstantBuffers(0, numberOfConstantBuffers, constantBuffers);
}
