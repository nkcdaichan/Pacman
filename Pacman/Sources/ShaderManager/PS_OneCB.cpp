#include "pch.h"

#include "PS_OneCB.h"
//�R���X�g���N�^�@�V�F�[�_�[�t�@�C����ǂݍ���,�s�N�Z���V�F�[�_�[���쐬
PS_OneCB::PS_OneCB(const std::wstring& file, ID3D11Device1* device)
{
  //�V�F�[�_�[�f�[�^���i�[���邽�߂�ID3DBlob�C���^�[�t�F�C�X�ւ̃X�}�[�g�|�C���^
  Microsoft::WRL::ComPtr<ID3DBlob> blob;
  //�V�F�[�_�[�t�@�C����ǂݍ����blob�Ɋi�[
  D3DReadFileToBlob(file.c_str(), &blob);
  //�f�o�C�X���g�p���ăs�N�Z���V�F�[�_�[���쐬
  device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, GetShaderPP());
}
//�s�N�Z���V�F�[�_�[���R���e�L�X�g�ɐݒ肵�ăA�N�e�B�u��
void PS_OneCB::ActivateShader(ID3D11DeviceContext1 * context)
{
  //�s�N�Z���V�F�[�_�[��ݒ肷��
  context->PSSetShader(GetShader(), NULL, 0);
}
//�萔�o�b�t�@�̃o�C���h����
void PS_OneCB::BindConstantBuffers(ID3D11DeviceContext1* context, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers)
{
  //�萔�o�b�t�@���L�����o�b�t�@�̐���0��葽���ꍇ�Ƀo�C���h���s��
  if (constantBuffers && numberOfConstantBuffers > 0)
	//�萔�o�b�t�@��ݒ肷��
    context->PSSetConstantBuffers(0, numberOfConstantBuffers, constantBuffers);
}
