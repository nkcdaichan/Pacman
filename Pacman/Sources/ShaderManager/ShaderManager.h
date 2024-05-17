#pragma once
//std::arrayを使用するため
#include <array>
//std::vectorを使用するため
#include <vector>

#include "IShader.h"
//クラスの定義
class ShaderManager
{
public:
  //VertexShaderの種類を列挙する
  enum class VertexShader
  {
    Indexed,      //インデックス付き頂点シェーダー
    Instanced,    //インスタンシング用頂点シェーダー
    UI,           //UI用頂点シェーダー
    _Count        //頂点シェーダーの数を表す
  };
  //GeometryShaderの種類を列挙する
  enum class GeometryShader
  {
    Billboard,    //ビルボード用ジオメトリシェーダー
    _Count        //ジオメトリシェーダーの数を表す
  };
  //PixelShaderの種類を列挙する
  enum class PixelShader
  {
    Color,        //カラー用ピクセルシェーダー
    Texture,      //テクスチャ用ピクセルシェーダー
    Phong,        //フォンシェーディング用ピクセルシェーダー
    UI,           //UI用ピクセルシェーダー
    _Count        //ピクセルシェーダーの数を表す
  };
  //デバイスとコンテキストを初期化
  ShaderManager(ID3D11Device1* device, ID3D11DeviceContext1* context);
  //デストラクタ
  ~ShaderManager();
  //指定された頂点シェーダーをセット
  void SetVertexShader(VertexShader vertexShader);
  //指定されたジオメトリシェーダーをセット
  void SetGeometryShader(GeometryShader geometryShader);
  //指定されたピクセルシェーダーをセット
  void SetPixelShader(PixelShader pixelShader);
  //頂点シェーダーにバインドする定数バッファを設定
  void BindConstantBuffersToVertexShader(VertexShader vertexShader, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers);
  //ジオメトリシェーダーにバインドする定数バッファを設定
  void BindConstantBuffersToGeometryShader(GeometryShader geometryShader, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers);
  //ピクセルシェーダーにバインドする定数バッファを設定
  void BindConstantBuffersToPixelShader(PixelShader pixelShader, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers);
  //指定された定数バッファの内容を更新
  void UpdateConstantBuffer(ID3D11Buffer* constantBuffer, void* data, size_t dataLength);
  //パイプラインからすべてのシェーダーをクリア
  void ClearShadersFromThePipeline();

private:
  //事前定義されたシェーダーを追加
  void AddPredefinedOnes();
  //ピクセルシェーダーを格納する配列
  std::array<std::unique_ptr<IShader<ID3D11PixelShader>>, static_cast<uint8_t>(PixelShader::_Count)>        m_pixelShaders;
  //ジオメトリシェーダーを格納する配列
  std::array<std::unique_ptr<IShader<ID3D11GeometryShader>>, static_cast<uint8_t>(GeometryShader::_Count)>  m_geometryShaders;
  //頂点シェーダーを格納する配列
  std::array<std::unique_ptr<IShader<ID3D11VertexShader>>, static_cast<uint8_t>(VertexShader::_Count)>      m_vertexShaders;
  //デバイスインターフェース
  ID3D11Device1*        m_device;
  //デバイスコンテキストインターフェース
  ID3D11DeviceContext1* m_context;
};

