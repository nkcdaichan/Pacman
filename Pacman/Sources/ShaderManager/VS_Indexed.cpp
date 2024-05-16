#include "pch.h"

#include "VS_Indexed.h"
//�R���X�g���N�^ �t�@�C���p�X�ƃf�o�C�X���󂯎��
VS_Indexed::VS_Indexed(const std::wstring& file, ID3D11Device1* device)
{
  //�R���p�C���ς݃V�F�[�_�[�f�[�^��ێ����邽�߂�blob�I�u�W�F�N�g
  Microsoft::WRL::ComPtr<ID3DBlob> blob;
  //�t�@�C������V�F�[�_�[�o�C�i����ǂݍ����blob�Ɋi�[

  D3DReadFileToBlob(file.c_str(), &blob);
  //�ǂݍ��񂾃V�F�[�_�[�o�C�i�����璸�_�V�F�[�_�[���쐬
  device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, GetShaderPP());

  //���̓��C�A�E�g�̒�`
  const D3D11_INPUT_ELEMENT_DESC ied[] =
  {
    { "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 }, // �ʒu���
    { "Normal"  ,0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 }, // �@���x�N�g��
    { "Color"   ,0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 }, // �F
    { "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT   ,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 }  // �e�N�X�`�����W
  };
  //���̓��C�A�E�g�̍쐬
  device->CreateInputLayout(ied, (UINT)std::size(ied), blob->GetBufferPointer(), blob->GetBufferSize(), m_inputLayout.GetAddressOf());
}
//�f�X�g���N�^
VS_Indexed::~VS_Indexed()
{
}
//�V�F�[�_�[���A�N�e�B�u�ɂ��郁�\�b�h
void VS_Indexed::ActivateShader(ID3D11DeviceContext1* context)
{
  //���̓��C�A�E�g��ݒ�
  context->IASetInputLayout(m_inputLayout.Get());
  //���_�V�F�[�_�[��ݒ�
  context->VSSetShader(GetShader(), NULL, 0);
}
//�R���X�^���g�o�b�t�@���o�C���h���郁�\�b�h
void VS_Indexed::BindConstantBuffers(ID3D11DeviceContext1* context, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers)
{
  //�L���ȃo�b�t�@�ƃo�b�t�@�������邩�m�F
  if (constantBuffers && numberOfConstantBuffers > 0)
	 //�R���X�^���g�o�b�t�@���o�C���h
    context->VSSetConstantBuffers(0, numberOfConstantBuffers, constantBuffers);
}
