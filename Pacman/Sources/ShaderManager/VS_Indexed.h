#pragma once

#include "IShader.h"
//VS_Indexedクラスは,テンプレートパラメータとしてID3D11VertexShaderを使用してIShaderから継承
class VS_Indexed : public IShader<ID3D11VertexShader>
{
public:
  //ファイルパスとDirect3Dデバイスのポインタを受け取るコンストラクタ
  VS_Indexed(const std::wstring& file, ID3D11Device1* device);
  //デストラクタ
  ~VS_Indexed();

  //IShaderからオーバーライドされたシェーダーをアクティベートする
  virtual void ActivateShader(ID3D11DeviceContext1* context) override;
  //IShaderからオーバーライドされた定数バッファをバインドする
  virtual void BindConstantBuffers(ID3D11DeviceContext1* context, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers) override;

private:
  //頂点シェーダーの入力レイアウトインターフェースを管理するスマートポインタ
  Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
};
