#include "pch.h"

#include "ShaderManager.h"

//バーテックスシェーダー
#include "VS_Indexed.h"
#include "VS_Instanced.h"

//ジオメトリシェーダー
#include "GS_General.h"

//ピクセルシェーダー
#include "PS_General.h"
#include "PS_OneCB.h"
//コンストラクタ
ShaderManager::ShaderManager(ID3D11Device1* device, ID3D11DeviceContext1* context) :
  //デバイスの初期化
  m_device(device),
  //コンテキストの初期化
  m_context(context)
{
  //プリデファインされたシェーダーを追加する関数を呼び出す
  AddPredefinedOnes();
}
//デストラクタ
ShaderManager::~ShaderManager()
{
}
//ピクセルシェーダーを設定する
void ShaderManager::SetPixelShader(PixelShader pixelShader)
{
  //ピクセルシェーダーをアクティブに
  m_pixelShaders[static_cast<uint8_t>(pixelShader)]->ActivateShader(m_context);
}
//バーテックスシェーダーに定数バッファをバインドする
void ShaderManager::BindConstantBuffersToVertexShader(VertexShader vertexShader, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers)
{
  //定数バッファをバインド
  m_vertexShaders[static_cast<uint8_t>(vertexShader)]->BindConstantBuffers(m_context, constantBuffers, numberOfConstantBuffers);
}
//ジオメトリシェーダーに定数バッファをバインドする
void ShaderManager::BindConstantBuffersToGeometryShader(GeometryShader geometryShader, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers)
{
  //定数バッファをバインド
  m_geometryShaders[static_cast<uint8_t>(geometryShader)]->BindConstantBuffers(m_context, constantBuffers, numberOfConstantBuffers);
}
//ピクセルシェーダーに定数バッファをバインドする
void ShaderManager::BindConstantBuffersToPixelShader(PixelShader pixelShader, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers)
{
  //定数バッファをバインド
  m_pixelShaders[static_cast<uint8_t>(pixelShader)]->BindConstantBuffers(m_context, constantBuffers, numberOfConstantBuffers);
}
//定数バッファの更新
void ShaderManager::UpdateConstantBuffer(ID3D11Buffer* constantBuffer, void* data, size_t dataLength)
{
  //マップされたサブリソース
  D3D11_MAPPED_SUBRESOURCE resource;
  //バッファをマップ
  m_context->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
  //データをコピーする
  memcpy(resource.pData, data, dataLength);
  //バッファのマップを解除
  m_context->Unmap(constantBuffer, 0);
}
//ジオメトリシェーダーを設定する
void ShaderManager::SetGeometryShader(GeometryShader geometryShader)
{
  //ジオメトリシェーダーをアクティブに
  m_geometryShaders[static_cast<uint8_t>(geometryShader)]->ActivateShader(m_context);
}
//vertexShaderを設定する
void ShaderManager::SetVertexShader(VertexShader vertexShader)
{
  //vertexShaderをアクティブに
  m_vertexShaders[static_cast<uint8_t>(vertexShader)]->ActivateShader(m_context);
}
//パイプラインからシェーダーをクリアする
void ShaderManager::ClearShadersFromThePipeline()
{
  //vertexShaderをクリア
  m_context->VSSetShader(NULL, NULL, 0);
  //ジオメトリシェーダーをクリア
  m_context->GSSetShader(NULL, NULL, 0);
  //ピクセルシェーダーをクリア
  m_context->PSSetShader(NULL, NULL, 0);
}
//プリデファインされたシェーダーを追加する
void ShaderManager::AddPredefinedOnes()
{
  //ピクセルシェーダーの追加
  m_pixelShaders[static_cast<uint8_t>(PixelShader::Color)] = std::make_unique<PS_General>(L"../bin/PS_Color.cso", m_device);
  m_pixelShaders[static_cast<uint8_t>(PixelShader::Texture)] = std::make_unique<PS_General>(L"../bin/PS_Texture.cso", m_device);
  m_pixelShaders[static_cast<uint8_t>(PixelShader::Phong)] = std::make_unique<PS_OneCB>(L"../bin/PS_Phong.cso", m_device);
  m_pixelShaders[static_cast<uint8_t>(PixelShader::UI)] = std::make_unique<PS_General>(L"../bin/PS_UI.cso", m_device);

  //ジオメトリシェーダーの追加
  m_geometryShaders[static_cast<uint8_t>(GeometryShader::Billboard)] = std::make_unique<GS_General>(L"../bin/GS_Billboard.cso", m_device);

  //バーテックスシェーダーの追加
  m_vertexShaders[static_cast<uint8_t>(VertexShader::Indexed)] = std::make_unique<VS_Indexed>(L"../bin/VS_Indexed.cso", m_device);
  m_vertexShaders[static_cast<uint8_t>(VertexShader::Instanced)] = std::make_unique<VS_Instanced>(L"../bin/VS_Instanced.cso", m_device);
  m_vertexShaders[static_cast<uint8_t>(VertexShader::UI)] = std::make_unique<VS_Indexed>(L"../bin/VS_UI.cso", m_device);
}
