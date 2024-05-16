#pragma once

#include "IShader.h"
//ID3D11PixelShader型のIShaderを継承します
class PS_General : public IShader<ID3D11PixelShader>
{
public:
  //コンストラクタ。シェーダーファイルのパスとデバイスを引数として受け取る
  PS_General(const std::wstring& file, ID3D11Device1* device);
  //デストラクタ。
  ~PS_General() {}

  //IShaderインターフェースからのオーバーライドメソッド
  //シェーダーをアクティブにし,引数としてデバイスコンテキストを受け取る
  virtual void ActivateShader(ID3D11DeviceContext1* context) override;
  //定数バッファをバインドし,引数としてデバイスコンテキスト,定数バッファの配列,定数バッファの数を受け取る
  virtual void BindConstantBuffers(ID3D11DeviceContext1* context, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers) override;
};
