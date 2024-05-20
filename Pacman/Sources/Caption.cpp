#include "pch.h"

#include "Caption.h"
#include "WICTextureLoader.h"

using namespace DirectX;

//コンストラクタ
Caption::Caption() :
  m_position(0, 0, 0),
  m_height(0),
  m_width(0)
{
  //ワールドマトリックスの更新
  UpdateWorldMatrix();
}
//デストラクタ
Caption::~Caption()
{
}

//描画メソッド
void Caption::Draw(ID3D11DeviceContext1 * context)
{
  unsigned int stride = sizeof(Global::Vertex);
  unsigned int offset = 0;
  //プリミティブトポロジを設定
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  //VertexBufferを設定
  context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
  //インデックスバッファを設定
  context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
  //ピクセルシェーダリソースを設定
  context->PSSetShaderResources(0, 1, m_shaderResourceView.GetAddressOf());
  //サンプラーステートを設定
  context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
  //ラスタライザーステートを設定
  context->RSSetState(m_cullNone.Get());
  //ブレンドステートを設定
  context->OMSetBlendState(m_blendState.Get(), NULL, 0xffffffff);
  //インデックスを使って描画
  context->DrawIndexed(6, 0, 0);
}

//初期化メソッド
void Caption::Init(ID3D11Device1* device, uint16_t width, uint16_t height)
{
  m_width = width;
  m_height = height;
  //テクスチャを読み込む
  DX::ThrowIfFailed(CreateWICTextureFromFile(device, nullptr, L"Resources/caption.png", m_resource.GetAddressOf(), m_shaderResourceView.GetAddressOf()));

  // Vertex buffer
  /*
  m_vertices.push_back({ {-0.8f, -0.2f, 0.0f}, {0.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, {0, 1} });
  m_vertices.push_back({ {-0.8f,  0.2f, 0.0f}, {0.0, 1.0, 0.0}, {0.0, 0.0, 0.0}, {0, 0} });
  m_vertices.push_back({ { 0.8f, -0.2f, 0.0f}, {0.0, 1.0, 0.0}, {1.0, 1.0, 0.0}, {1, 1} });
  m_vertices.push_back({ { 0.8f,  0.2f, 0.0f}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0}, {1, 0} });
  */
  //頂点データを設定
  m_vertices.push_back({ { 0, static_cast<float>(height), 0.0f}, {0.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, {0, 1} });
  m_vertices.push_back({ { 0, 0, 0.0f}, {0.0, 1.0, 0.0}, {0.0, 0.0, 0.0}, {0, 0} });
  m_vertices.push_back({ { static_cast<float>(width), static_cast<float>(height), 0.0f}, {0.0, 1.0, 0.0}, {1.0, 1.0, 0.0}, {1, 1} });
  m_vertices.push_back({ { static_cast<float>(width), 0, 0.0f}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0}, {1, 0} });

  //頂点バッファの設定
  D3D11_BUFFER_DESC bd = {};
  bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  bd.Usage = D3D11_USAGE_DEFAULT;
  bd.CPUAccessFlags = 0;
  bd.MiscFlags = 0;
  bd.ByteWidth = UINT(sizeof(Global::Vertex) * m_vertices.size());
  bd.StructureByteStride = sizeof(Global::Vertex);

  D3D11_SUBRESOURCE_DATA sd = {};
  sd.pSysMem = m_vertices.data();
  device->CreateBuffer(&bd, &sd, &m_vertexBuffer);

  //インデックスバッファ
  m_indices.push_back(0);
  m_indices.push_back(1);
  m_indices.push_back(2);
  m_indices.push_back(2);
  m_indices.push_back(1);
  m_indices.push_back(3);

  D3D11_BUFFER_DESC ibd = {};
  ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
  ibd.Usage = D3D11_USAGE_DEFAULT;
  ibd.CPUAccessFlags = 0;
  ibd.MiscFlags = 0;
  ibd.ByteWidth = UINT(m_indices.size() * sizeof(uint16_t));
  ibd.StructureByteStride = sizeof(uint16_t);

  D3D11_SUBRESOURCE_DATA isd = {};
  isd.pSysMem = m_indices.data();
  DX::ThrowIfFailed(device->CreateBuffer(&ibd, &isd, &m_indexBuffer));

  //ラスタライザーステートの設定
  D3D11_RASTERIZER_DESC cmdesc = {};
  cmdesc.FillMode = D3D11_FILL_SOLID;
  cmdesc.FrontCounterClockwise = false;
  cmdesc.CullMode = D3D11_CULL_NONE;

  DX::ThrowIfFailed(device->CreateRasterizerState(&cmdesc, m_cullNone.GetAddressOf()));

  //サンプラーステートの設定
  D3D11_SAMPLER_DESC sampDesc = {};
  sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
  sampDesc.MinLOD = 0;
  sampDesc.MaxLOD = 0;

  //サンプラーステートを作成
  DX::ThrowIfFailed(device->CreateSamplerState(&sampDesc, m_samplerState.GetAddressOf()));

  //ブレンドステートの設定
  D3D11_BLEND_DESC omDesc = {};
  omDesc.RenderTarget[0].BlendEnable = true;
  omDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
  omDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
  omDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
  omDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
  omDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
  omDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
  omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

  DX::ThrowIfFailed(device->CreateBlendState(&omDesc, m_blendState.GetAddressOf()));
}
//Y座標の調整メソッド
void Caption::AdjustY(float value, float minimum)
{
  m_position.y += value;
  m_position.y = std::min(m_position.y, minimum);
  //ワールドマトリックスの更新
  UpdateWorldMatrix();
}

//位置の設定メソッド
void Caption::SetPosition(float x, float y, float z)
{
  m_position.x = x;
  m_position.y = y;
  m_position.z = z;
  //ワールドマトリックスの更新
  UpdateWorldMatrix();
}
//更新メソッド
void Caption::UpdateWorldMatrix()
{
  XMStoreFloat4x4(&m_worldMatrix, XMMatrixTranspose(XMMatrixTranslation(m_position.x, m_position.y, m_position.z)));
}
