#pragma once

#include "IShader.h"
//頂点シェーダークラス
class VS_Instanced : public IShader<ID3D11VertexShader>
{
public:
  //コンストラクタ
  //ファイル名とデバイスを受け取り、頂点シェーダーを初期化する
  VS_Instanced(const std::wstring& file, ID3D11Device1* device);
  //デストラクタ
  ~VS_Instanced();

  //IShaderインターフェースからのオーバーライド
  //シェーダーをアクティブに
  virtual void ActivateShader(ID3D11DeviceContext1* context) override;
  //定数バッファをバインドする
  //引数の numberOfConstantBuffersは0から255の範囲で指定
  //uint8_tはメモリの効率的な使用、範囲の適切さ、パフォーマンスの向上のため使用
  virtual void BindConstantBuffers(ID3D11DeviceContext1* context, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers) override;

private:
  //入力レイアウトを保持する変数
  Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
};
