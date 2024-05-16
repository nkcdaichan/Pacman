#pragma once

#include "IShader.h"
//GS_General�N���X�̒�`�B���̃N���X�� IShader<ID3D11GeometryShader>���p��
//IShader��Direct3D11�̃V�F�[�_�[���������߂̒��ۊ��N���X�ł���AID3D11GeometryShader�̓W�I���g���V�F�[�_�[�̃C���^�[�t�F�[�X
class GS_General : public IShader<ID3D11GeometryShader>
{
public:
  //�R���X�g���N�^�B�V�F�[�_�[�t�@�C���̃p�X�ƃf�o�C�X��������
  GS_General(const std::wstring& file, ID3D11Device1* device);
  //�f�X�g���N�^�B
  ~GS_General();

  //IShader �C���^�[�t�F�[�X����̃I�[�o�[���C�h�B�V�F�[�_�[���A�N�e�B�u��
  virtual void ActivateShader(ID3D11DeviceContext1* context) override;
  //IShader �C���^�[�t�F�[�X����̃I�[�o�[���C�h�B�萔�o�b�t�@���o�C���h
  virtual void BindConstantBuffers(ID3D11DeviceContext1* context, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers) override;
};
