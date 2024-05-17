#pragma once

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <string>
#include <wrl.h>
//テンプレートクラスIShaderの定義。シェーダーの基底インターフェースを提供
template <typename T>
class IShader
{
public:
  //仮想デストラクタ。派生クラスで適切にクリーンアップが行われるように
  virtual ~IShader() {}
  //シェーダーへのポインタを取得する
  T*  GetShader()   { return m_shader.Get(); }
  //シェーダーへのポインタのアドレスを取得する
  T** GetShaderPP() { return m_shader.GetAddressOf(); }
  //純粋仮想関数なのでシェーダーをアクティブにするために派生クラスで実装する必要がある
  //純粋仮想関数を持つクラスは抽象クラスとなり,そのクラスのインスタンスを直接生成することはできない
  //純粋仮想関数は,派生クラスで必ずオーバーライドされることを前提
  virtual void ActivateShader(ID3D11DeviceContext1* context) = 0;
  //定数バッファをバインドするために派生クラスで実装する必要がある。これも純粋仮想関数
  virtual void BindConstantBuffers(ID3D11DeviceContext1* context, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers) = 0;

private:
  //Microsoft::WRL::ComPtrはCOMオブジェクトのスマートポインタ。テンプレート引数T型のシェーダーを管理
  Microsoft::WRL::ComPtr<T> m_shader;
};