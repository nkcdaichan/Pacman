#pragma once

#include "IShader.h"
//���_�V�F�[�_�[�N���X
class VS_Instanced : public IShader<ID3D11VertexShader>
{
public:
  //�R���X�g���N�^
  //�t�@�C�����ƃf�o�C�X���󂯎��A���_�V�F�[�_�[������������
  VS_Instanced(const std::wstring& file, ID3D11Device1* device);
  //�f�X�g���N�^
  ~VS_Instanced();

  //IShader�C���^�[�t�F�[�X����̃I�[�o�[���C�h
  //�V�F�[�_�[���A�N�e�B�u��
  virtual void ActivateShader(ID3D11DeviceContext1* context) override;
  //�萔�o�b�t�@���o�C���h����
  //������ numberOfConstantBuffers��0����255�͈̔͂Ŏw��
  //uint8_t�̓������̌����I�Ȏg�p�A�͈͂̓K�؂��A�p�t�H�[�}���X�̌���̂��ߎg�p
  virtual void BindConstantBuffers(ID3D11DeviceContext1* context, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers) override;

private:
  //���̓��C�A�E�g��ێ�����ϐ�
  Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
};
