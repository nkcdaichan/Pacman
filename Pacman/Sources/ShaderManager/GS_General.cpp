#include "pch.h"

#include "GS_General.h"
//GS_General�N���X�̃R���X�g���N�^
GS_General::GS_General(const std::wstring & file, ID3D11Device1* device)
{
  //�V�F�[�_�[�o�C�i�����i�[���邽�߂�ID3DBlob�C���^�[�t�F�[�X�̃|�C���^���쐬
  Microsoft::WRL::ComPtr<ID3DBlob> blob;
  //�w�肳�ꂽ�t�@�C������V�F�[�_�[�o�C�i����ǂݍ����blob�Ɋi�[
  D3DReadFileToBlob(file.c_str(), &blob);
  //�f�o�C�X���g�p���ăW�I���g���V�F�[�_�[���쐬���A�����o�ϐ��ɕۑ�
  device->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, GetShaderPP());
}
//GS_General�N���X�̃f�X�g���N�^
GS_General::~GS_General()
{
}
//�W�I���g���V�F�[�_�[���A�N�e�B�u�ɂ��郁�\�b�h
void GS_General::ActivateShader(ID3D11DeviceContext1* context)
{
  //�R���e�L�X�g���g�p���ăW�I���g���V�F�[�_�[��ݒ�
  context->GSSetShader(GetShader(), NULL, 0);
}
//�R���X�^���g�o�b�t�@���o�C���h���郁�\�b�h
void GS_General::BindConstantBuffers(ID3D11DeviceContext1* context, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers)
{
  //�R���X�^���g�o�b�t�@�����݂��A�����̐���0���傫���ꍇ�ɂ̂ݎ��s
  if (constantBuffers && numberOfConstantBuffers > 0)
	// �R���e�L�X�g���g�p���ăW�I���g���V�F�[�_�[�̃R���X�^���g�o�b�t�@��ݒ�
    context->GSSetConstantBuffers(0, numberOfConstantBuffers, constantBuffers);
}
