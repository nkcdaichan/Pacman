#pragma once

#include "IShader.h"
//ID3D11PixelShader�^��IShader���p�����܂�
class PS_General : public IShader<ID3D11PixelShader>
{
public:
  //�R���X�g���N�^�B�V�F�[�_�[�t�@�C���̃p�X�ƃf�o�C�X�������Ƃ��Ď󂯎��
  PS_General(const std::wstring& file, ID3D11Device1* device);
  //�f�X�g���N�^�B
  ~PS_General() {}

  //IShader�C���^�[�t�F�[�X����̃I�[�o�[���C�h���\�b�h
  //�V�F�[�_�[���A�N�e�B�u�ɂ�,�����Ƃ��ăf�o�C�X�R���e�L�X�g���󂯎��
  virtual void ActivateShader(ID3D11DeviceContext1* context) override;
  //�萔�o�b�t�@���o�C���h��,�����Ƃ��ăf�o�C�X�R���e�L�X�g,�萔�o�b�t�@�̔z��,�萔�o�b�t�@�̐����󂯎��
  virtual void BindConstantBuffers(ID3D11DeviceContext1* context, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers) override;
};
