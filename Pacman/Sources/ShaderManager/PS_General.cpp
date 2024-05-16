#include "pch.h"

#include "PS_General.h"
//PS_General �N���X�̃R���X�g���N�^
PS_General::PS_General(const std::wstring& file, ID3D11Device1* device)
{
  //�V�F�[�_�[�t�@�C���̓��e��ǂݍ��ނ��߂̃o�b�t�@
  Microsoft::WRL::ComPtr<ID3DBlob> blob;
  //�t�@�C����ǂݍ��݁Ablob�Ɋi�[
  D3DReadFileToBlob(file.c_str(), &blob);
  //�f�o�C�X���g���ăs�N�Z���V�F�[�_�[���쐬
  device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, GetShaderPP());
}
//�V�F�[�_�[��L����
void PS_General::ActivateShader(ID3D11DeviceContext1* context)
{
  //�R���e�L�X�g�Ƀs�N�Z���V�F�[�_�[���Z�b�g
  context->PSSetShader(GetShader(), NULL, 0);
}
//�R���X�^���g�o�b�t�@���o�C���h����
void PS_General::BindConstantBuffers(ID3D11DeviceContext1* context, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers)
{
   //���g�p�����𖾎��I�ɖ������邽�߂̃L���X�g
  (void)context;
  (void)constantBuffers;
  (void)numberOfConstantBuffers;
}
