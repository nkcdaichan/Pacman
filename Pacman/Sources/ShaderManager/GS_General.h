#pragma once

#include "IShader.h"
//GS_Generalクラスの定義。このクラスは IShader<ID3D11GeometryShader>を継承
//IShaderはDirect3D11のシェーダーを扱うための抽象基底クラスであり、ID3D11GeometryShaderはジオメトリシェーダーのインターフェース
class GS_General : public IShader<ID3D11GeometryShader>
{
public:
  //コンストラクタ。シェーダーファイルのパスとデバイスを引数に
  GS_General(const std::wstring& file, ID3D11Device1* device);
  //デストラクタ。
  ~GS_General();

  //IShader インターフェースからのオーバーライド。シェーダーをアクティブ化
  virtual void ActivateShader(ID3D11DeviceContext1* context) override;
  //IShader インターフェースからのオーバーライド。定数バッファをバインド
  virtual void BindConstantBuffers(ID3D11DeviceContext1* context, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers) override;
};
