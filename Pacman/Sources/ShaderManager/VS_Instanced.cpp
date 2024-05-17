#include "pch.h"

#include "VS_Instanced.h"
// �R���X�g���N�^�F���_�V�F�[�_�[��ǂݍ��݁A�쐬
VS_Instanced::VS_Instanced(const std::wstring & file, ID3D11Device1 * device)
{
  //�V�F�[�_�[�R�[�h���i�[����o�C�i���I�u�W�F�N�g�̃|�C���^
  Microsoft::WRL::ComPtr<ID3DBlob> blob;
  //�t�@�C������V�F�[�_�[�R�[�h��ǂݍ��݁Ablob�Ɋi�[
  D3DReadFileToBlob(file.c_str(), &blob);
  //device���g���Ē��_�V�F�[�_�[���쐬
  device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, GetShaderPP());
  //���̓��C�A�E�g�̒�`
  const D3D11_INPUT_ELEMENT_DESC ied[] =
  {
    { "Position" ,0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
    { "Normal"   ,0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
    { "Color"    ,0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
    { "AdjPos"   ,0,DXGI_FORMAT_R32G32B32_FLOAT,1,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_INSTANCE_DATA,1 },
    { "SizeFlag" ,0,DXGI_FORMAT_R8_UINT,1,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_INSTANCE_DATA,1 }
  };
  //���̓��C�A�E�g���쐬���Am_inputLayout�Ɋi�[
  device->CreateInputLayout(ied, (UINT)std::size(ied), blob->GetBufferPointer(), blob->GetBufferSize(), m_inputLayout.GetAddressOf());
}
//�f�X�g���N�^
VS_Instanced::~VS_Instanced()
{
}
//�V�F�[�_�[���A�N�e�B�u�ɂ���
void VS_Instanced::ActivateShader(ID3D11DeviceContext1* context)
{
  //���̓��C�A�E�g�ݒ�
  context->IASetInputLayout(m_inputLayout.Get());
  //���_�V�F�[�_�[�ݒ�
  context->VSSetShader(GetShader(), NULL, 0);
}
//�萔�o�b�t�@���o�C���h����
void VS_Instanced::BindConstantBuffers(ID3D11DeviceContext1* context, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers)
{
  if (constantBuffers && numberOfConstantBuffers > 0)
	//�萔�o�b�t�@�𒸓_�V�F�[�_�[�Ƀo�C���h
    context->VSSetConstantBuffers(0, numberOfConstantBuffers, constantBuffers);
}
