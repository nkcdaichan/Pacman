#pragma once

#include "IShader.h"
//PS_OneCBクラスは,ID3D11PixelShader型のシェーダーを扱うIShaderクラスを継承
class PS_OneCB : public IShader<ID3D11PixelShader>
{
public:
  //コンストラクタ。シェーダーファイルのパスとデバイスを受け取り初期化を行う
  PS_OneCB(const std::wstring& file, ID3D11Device1* device);
  //デストラクタ
  ~PS_OneCB() {}

  //IShaderから継承した仮想関数をオーバーライド
  //シェーダーをアクティブにするメソッド。レンダリングパイプラインで使用するシェーダーを設定
  virtual void ActivateShader(ID3D11DeviceContext1* context) override;
  //定数バッファをバインドするメソッド。シェーダーに対して定数バッファを設定
  virtual void BindConstantBuffers(ID3D11DeviceContext1* context, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers) override;
};

