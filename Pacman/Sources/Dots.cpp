#include "pch.h"

#include "Dots.h"
#include "Global.h"
#include "WICTextureLoader.h"

using namespace DirectX;

//コンストラクタ
Dots::Dots() :
  m_dots{
	//2次元配列の初期化: 0=何もない、1=普通のドット、2=エクストラドット
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0},
    {0, 0, 2, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 2, 0, 0},
    {0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0},
    {0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 2, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 2, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
{
  //ワールドマトリックスを単位行列で初期化
  XMStoreFloat4x4(&m_worldMatrix, DirectX::XMMatrixIdentity());     //単位行列の転置は不要
}
//デストラクタ
Dots::~Dots()
{
}

void Dots::Draw(ID3D11DeviceContext1* context)
{
  unsigned int strides[2];
  unsigned int offsets[2];

  //バッファのストライドを設定
  strides[0] = sizeof(Global::Vertex);
  strides[1] = sizeof(InstanceType);

  //バッファのオフセットを設定
  offsets[0] = 0;
  offsets[1] = 0;

  //頂点バッファとインスタンスバッファのポインタ配列を設定
  ID3D11Buffer* bufferPointers[2];
  bufferPointers[0] = m_vertexBuffer.Get();
  bufferPointers[1] = m_instanceBuffer.Get();

  //プリミティブトポロジーをポイントリストに設定
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
  //頂点バッファを設定
  context->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

  //ピクセルシェーダリソースを設定
  context->PSSetShaderResources(0, 1, m_shaderResourceView.GetAddressOf());
  //サンプラーステートを設定
  context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

  //ラスタライザーステートを設定
  context->RSSetState(m_cullNone.Get());

  //ブレンドステートを設定
  context->OMSetBlendState(m_blendState.Get(), NULL, 0xffffffff);

  //インスタンス描画を実行
  context->DrawInstanced(1, (UINT)m_instances.size(), 0, 0);
}

void Dots::Init(ID3D11Device1* device)
{
  //テクスチャの読み込みと作成
  DX::ThrowIfFailed(CreateWICTextureFromFile(device, nullptr, L"Resources/dot.png", m_resource.GetAddressOf(), m_shaderResourceView.GetAddressOf()));

  //頂点データを追加
  m_vertices.push_back({ {0, 0, 0}, {0.0, 1.0, 0.0}, {0.8, 0.0, 0.0} });

  //Vertexbufferの設定
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

  //ラスタライザーステートの設定
  D3D11_RASTERIZER_DESC cmdesc = {};
  cmdesc.FillMode = D3D11_FILL_SOLID;
  cmdesc.FrontCounterClockwise = false;
  cmdesc.CullMode = D3D11_CULL_NONE;

  DX::ThrowIfFailed(device->CreateRasterizerState(&cmdesc, m_cullNone.GetAddressOf()));

  //サンプラーの設定
  D3D11_SAMPLER_DESC sampDesc = {};
  sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
  sampDesc.MinLOD = 0;
  sampDesc.MaxLOD = 0;

  //サンプルステートを作成
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

  //ブレンドステートを作成
  DX::ThrowIfFailed(device->CreateBlendState(&omDesc, m_blendState.GetAddressOf()));

  //インスタンスデータの設定
  for (unsigned int z = 0; z != Global::worldSize; z++)
    for (unsigned int x = 0; x != Global::worldSize; x++)
      if (m_dots[z][x] > 0)
        m_instances.push_back({DirectX::XMFLOAT3(static_cast<float>(x) + 0.5f, 0.25f, static_cast<float>(z) + 0.5f), m_dots[z][x]});

  //インスタンスバッファの設定
  D3D11_BUFFER_DESC instanceBufferDesc = {};
  instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  instanceBufferDesc.MiscFlags = 0;
  instanceBufferDesc.ByteWidth = sizeof(InstanceType) * (UINT)m_instances.size();
  instanceBufferDesc.StructureByteStride = 0;

  D3D11_SUBRESOURCE_DATA instanceData = {};
  instanceData.pSysMem = m_instances.data();
  instanceData.SysMemPitch = 0;
  instanceData.SysMemSlicePitch = 0;

  //インスタンスバッファを作成
  DX::ThrowIfFailed(device->CreateBuffer(&instanceBufferDesc, &instanceData, m_instanceBuffer.GetAddressOf()));
}

void Dots::Update(uint8_t column, uint8_t row, ID3D11DeviceContext1* context, Type& dotEaten)
{
  //ドットの種類に応じてdotEatenを設定
  switch (m_dots[row][column])
  {
    case 0:
      dotEaten = Type::Nothing;
      break;
    case 1:
      dotEaten = Type::Normal;
      break;
    case 2:
      dotEaten = Type::Extra;
      break;
  }

  //ドットが存在する場合
  if (m_dots[row][column] > 0)
  {
	//ドットを消す
    m_dots[row][column] = 0;

	//インスタンスデータをクリア
    m_instances.clear();

	//インスタンスデータを更新
    for (unsigned int z = 0; z != Global::worldSize; z++)
      for (unsigned int x = 0; x != Global::worldSize; x++)
        if (m_dots[z][x] > 0)
          m_instances.push_back({DirectX::XMFLOAT3(static_cast<float>(x) + 0.5f, 0.25f, static_cast<float>(z) + 0.5f), m_dots[z][x]});

	//全てのドットが食べられた場合
    if (m_instances.size() == 0)
    {
	  //最後のドットが食べられたことを通知
      dotEaten = Type::LastOne;
      return;
    }
    else
    {
	  //インスタンスバッファをマッピングして更新
      D3D11_MAPPED_SUBRESOURCE resource;
      context->Map(m_instanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
      memcpy(resource.pData, m_instances.data(), m_instances.size() * sizeof(InstanceType));
      context->Unmap(m_instanceBuffer.Get(), 0);
    }
  }
}
