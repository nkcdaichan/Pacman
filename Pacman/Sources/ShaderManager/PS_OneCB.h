#pragma once

#include "IShader.h"
//PS_OneCB�N���X��,ID3D11PixelShader�^�̃V�F�[�_�[������IShader�N���X���p��
class PS_OneCB : public IShader<ID3D11PixelShader>
{
public:
  //�R���X�g���N�^�B�V�F�[�_�[�t�@�C���̃p�X�ƃf�o�C�X���󂯎�菉�������s��
  PS_OneCB(const std::wstring& file, ID3D11Device1* device);
  //�f�X�g���N�^
  ~PS_OneCB() {}

  //IShader����p���������z�֐����I�[�o�[���C�h
  //�V�F�[�_�[���A�N�e�B�u�ɂ��郁�\�b�h�B�����_�����O�p�C�v���C���Ŏg�p����V�F�[�_�[��ݒ�
  virtual void ActivateShader(ID3D11DeviceContext1* context) override;
  //�萔�o�b�t�@���o�C���h���郁�\�b�h�B�V�F�[�_�[�ɑ΂��Ē萔�o�b�t�@��ݒ�
  virtual void BindConstantBuffers(ID3D11DeviceContext1* context, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers) override;
};

