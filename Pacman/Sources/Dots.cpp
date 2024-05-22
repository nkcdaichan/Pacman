#include "pch.h"

#include "Dots.h"
#include "Global.h"
#include "WICTextureLoader.h"

using namespace DirectX;

//�R���X�g���N�^
Dots::Dots() :
  m_dots{
	//2�����z��̏�����: 0=�����Ȃ��A1=���ʂ̃h�b�g�A2=�G�N�X�g���h�b�g
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
  //���[���h�}�g���b�N�X��P�ʍs��ŏ�����
  XMStoreFloat4x4(&m_worldMatrix, DirectX::XMMatrixIdentity());     //�P�ʍs��̓]�u�͕s�v
}
//�f�X�g���N�^
Dots::~Dots()
{
}

void Dots::Draw(ID3D11DeviceContext1* context)
{
  unsigned int strides[2];
  unsigned int offsets[2];

  //�o�b�t�@�̃X�g���C�h��ݒ�
  strides[0] = sizeof(Global::Vertex);
  strides[1] = sizeof(InstanceType);

  //�o�b�t�@�̃I�t�Z�b�g��ݒ�
  offsets[0] = 0;
  offsets[1] = 0;

  //���_�o�b�t�@�ƃC���X�^���X�o�b�t�@�̃|�C���^�z���ݒ�
  ID3D11Buffer* bufferPointers[2];
  bufferPointers[0] = m_vertexBuffer.Get();
  bufferPointers[1] = m_instanceBuffer.Get();

  //�v���~�e�B�u�g�|���W�[���|�C���g���X�g�ɐݒ�
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
  //���_�o�b�t�@��ݒ�
  context->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

  //�s�N�Z���V�F�[�_���\�[�X��ݒ�
  context->PSSetShaderResources(0, 1, m_shaderResourceView.GetAddressOf());
  //�T���v���[�X�e�[�g��ݒ�
  context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

  //���X�^���C�U�[�X�e�[�g��ݒ�
  context->RSSetState(m_cullNone.Get());

  //�u�����h�X�e�[�g��ݒ�
  context->OMSetBlendState(m_blendState.Get(), NULL, 0xffffffff);

  //�C���X�^���X�`������s
  context->DrawInstanced(1, (UINT)m_instances.size(), 0, 0);
}

void Dots::Init(ID3D11Device1* device)
{
  //�e�N�X�`���̓ǂݍ��݂ƍ쐬
  DX::ThrowIfFailed(CreateWICTextureFromFile(device, nullptr, L"Resources/dot.png", m_resource.GetAddressOf(), m_shaderResourceView.GetAddressOf()));

  //���_�f�[�^��ǉ�
  m_vertices.push_back({ {0, 0, 0}, {0.0, 1.0, 0.0}, {0.8, 0.0, 0.0} });

  //Vertexbuffer�̐ݒ�
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

  //���X�^���C�U�[�X�e�[�g�̐ݒ�
  D3D11_RASTERIZER_DESC cmdesc = {};
  cmdesc.FillMode = D3D11_FILL_SOLID;
  cmdesc.FrontCounterClockwise = false;
  cmdesc.CullMode = D3D11_CULL_NONE;

  DX::ThrowIfFailed(device->CreateRasterizerState(&cmdesc, m_cullNone.GetAddressOf()));

  //�T���v���[�̐ݒ�
  D3D11_SAMPLER_DESC sampDesc = {};
  sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
  sampDesc.MinLOD = 0;
  sampDesc.MaxLOD = 0;

  //�T���v���X�e�[�g���쐬
  DX::ThrowIfFailed(device->CreateSamplerState(&sampDesc, m_samplerState.GetAddressOf()));

  //�u�����h�X�e�[�g�̐ݒ�
  D3D11_BLEND_DESC omDesc = {};
  omDesc.RenderTarget[0].BlendEnable = true;
  omDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
  omDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
  omDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
  omDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
  omDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
  omDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
  omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

  //�u�����h�X�e�[�g���쐬
  DX::ThrowIfFailed(device->CreateBlendState(&omDesc, m_blendState.GetAddressOf()));

  //�C���X�^���X�f�[�^�̐ݒ�
  for (unsigned int z = 0; z != Global::worldSize; z++)
    for (unsigned int x = 0; x != Global::worldSize; x++)
      if (m_dots[z][x] > 0)
        m_instances.push_back({DirectX::XMFLOAT3(static_cast<float>(x) + 0.5f, 0.25f, static_cast<float>(z) + 0.5f), m_dots[z][x]});

  //�C���X�^���X�o�b�t�@�̐ݒ�
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

  //�C���X�^���X�o�b�t�@���쐬
  DX::ThrowIfFailed(device->CreateBuffer(&instanceBufferDesc, &instanceData, m_instanceBuffer.GetAddressOf()));
}

void Dots::Update(uint8_t column, uint8_t row, ID3D11DeviceContext1* context, Type& dotEaten)
{
  //�h�b�g�̎�ނɉ�����dotEaten��ݒ�
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

  //�h�b�g�����݂���ꍇ
  if (m_dots[row][column] > 0)
  {
	//�h�b�g������
    m_dots[row][column] = 0;

	//�C���X�^���X�f�[�^���N���A
    m_instances.clear();

	//�C���X�^���X�f�[�^���X�V
    for (unsigned int z = 0; z != Global::worldSize; z++)
      for (unsigned int x = 0; x != Global::worldSize; x++)
        if (m_dots[z][x] > 0)
          m_instances.push_back({DirectX::XMFLOAT3(static_cast<float>(x) + 0.5f, 0.25f, static_cast<float>(z) + 0.5f), m_dots[z][x]});

	//�S�Ẵh�b�g���H�ׂ�ꂽ�ꍇ
    if (m_instances.size() == 0)
    {
	  //�Ō�̃h�b�g���H�ׂ�ꂽ���Ƃ�ʒm
      dotEaten = Type::LastOne;
      return;
    }
    else
    {
	  //�C���X�^���X�o�b�t�@���}�b�s���O���čX�V
      D3D11_MAPPED_SUBRESOURCE resource;
      context->Map(m_instanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
      memcpy(resource.pData, m_instances.data(), m_instances.size() * sizeof(InstanceType));
      context->Unmap(m_instanceBuffer.Get(), 0);
    }
  }
}
