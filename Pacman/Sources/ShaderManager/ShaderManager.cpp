#include "pch.h"

#include "ShaderManager.h"

//�o�[�e�b�N�X�V�F�[�_�[
#include "VS_Indexed.h"
#include "VS_Instanced.h"

//�W�I���g���V�F�[�_�[
#include "GS_General.h"

//�s�N�Z���V�F�[�_�[
#include "PS_General.h"
#include "PS_OneCB.h"
//�R���X�g���N�^
ShaderManager::ShaderManager(ID3D11Device1* device, ID3D11DeviceContext1* context) :
  //�f�o�C�X�̏�����
  m_device(device),
  //�R���e�L�X�g�̏�����
  m_context(context)
{
  //�v���f�t�@�C�����ꂽ�V�F�[�_�[��ǉ�����֐����Ăяo��
  AddPredefinedOnes();
}
//�f�X�g���N�^
ShaderManager::~ShaderManager()
{
}
//�s�N�Z���V�F�[�_�[��ݒ肷��
void ShaderManager::SetPixelShader(PixelShader pixelShader)
{
  //�s�N�Z���V�F�[�_�[���A�N�e�B�u��
  m_pixelShaders[static_cast<uint8_t>(pixelShader)]->ActivateShader(m_context);
}
//�o�[�e�b�N�X�V�F�[�_�[�ɒ萔�o�b�t�@���o�C���h����
void ShaderManager::BindConstantBuffersToVertexShader(VertexShader vertexShader, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers)
{
  //�萔�o�b�t�@���o�C���h
  m_vertexShaders[static_cast<uint8_t>(vertexShader)]->BindConstantBuffers(m_context, constantBuffers, numberOfConstantBuffers);
}
//�W�I���g���V�F�[�_�[�ɒ萔�o�b�t�@���o�C���h����
void ShaderManager::BindConstantBuffersToGeometryShader(GeometryShader geometryShader, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers)
{
  //�萔�o�b�t�@���o�C���h
  m_geometryShaders[static_cast<uint8_t>(geometryShader)]->BindConstantBuffers(m_context, constantBuffers, numberOfConstantBuffers);
}
//�s�N�Z���V�F�[�_�[�ɒ萔�o�b�t�@���o�C���h����
void ShaderManager::BindConstantBuffersToPixelShader(PixelShader pixelShader, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers)
{
  //�萔�o�b�t�@���o�C���h
  m_pixelShaders[static_cast<uint8_t>(pixelShader)]->BindConstantBuffers(m_context, constantBuffers, numberOfConstantBuffers);
}
//�萔�o�b�t�@�̍X�V
void ShaderManager::UpdateConstantBuffer(ID3D11Buffer* constantBuffer, void* data, size_t dataLength)
{
  //�}�b�v���ꂽ�T�u���\�[�X
  D3D11_MAPPED_SUBRESOURCE resource;
  //�o�b�t�@���}�b�v
  m_context->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
  //�f�[�^���R�s�[����
  memcpy(resource.pData, data, dataLength);
  //�o�b�t�@�̃}�b�v������
  m_context->Unmap(constantBuffer, 0);
}
//�W�I���g���V�F�[�_�[��ݒ肷��
void ShaderManager::SetGeometryShader(GeometryShader geometryShader)
{
  //�W�I���g���V�F�[�_�[���A�N�e�B�u��
  m_geometryShaders[static_cast<uint8_t>(geometryShader)]->ActivateShader(m_context);
}
//vertexShader��ݒ肷��
void ShaderManager::SetVertexShader(VertexShader vertexShader)
{
  //vertexShader���A�N�e�B�u��
  m_vertexShaders[static_cast<uint8_t>(vertexShader)]->ActivateShader(m_context);
}
//�p�C�v���C������V�F�[�_�[���N���A����
void ShaderManager::ClearShadersFromThePipeline()
{
  //vertexShader���N���A
  m_context->VSSetShader(NULL, NULL, 0);
  //�W�I���g���V�F�[�_�[���N���A
  m_context->GSSetShader(NULL, NULL, 0);
  //�s�N�Z���V�F�[�_�[���N���A
  m_context->PSSetShader(NULL, NULL, 0);
}
//�v���f�t�@�C�����ꂽ�V�F�[�_�[��ǉ�����
void ShaderManager::AddPredefinedOnes()
{
  //�s�N�Z���V�F�[�_�[�̒ǉ�
  m_pixelShaders[static_cast<uint8_t>(PixelShader::Color)] = std::make_unique<PS_General>(L"../bin/PS_Color.cso", m_device);
  m_pixelShaders[static_cast<uint8_t>(PixelShader::Texture)] = std::make_unique<PS_General>(L"../bin/PS_Texture.cso", m_device);
  m_pixelShaders[static_cast<uint8_t>(PixelShader::Phong)] = std::make_unique<PS_OneCB>(L"../bin/PS_Phong.cso", m_device);
  m_pixelShaders[static_cast<uint8_t>(PixelShader::UI)] = std::make_unique<PS_General>(L"../bin/PS_UI.cso", m_device);

  //�W�I���g���V�F�[�_�[�̒ǉ�
  m_geometryShaders[static_cast<uint8_t>(GeometryShader::Billboard)] = std::make_unique<GS_General>(L"../bin/GS_Billboard.cso", m_device);

  //�o�[�e�b�N�X�V�F�[�_�[�̒ǉ�
  m_vertexShaders[static_cast<uint8_t>(VertexShader::Indexed)] = std::make_unique<VS_Indexed>(L"../bin/VS_Indexed.cso", m_device);
  m_vertexShaders[static_cast<uint8_t>(VertexShader::Instanced)] = std::make_unique<VS_Instanced>(L"../bin/VS_Instanced.cso", m_device);
  m_vertexShaders[static_cast<uint8_t>(VertexShader::UI)] = std::make_unique<VS_Indexed>(L"../bin/VS_UI.cso", m_device);
}
