#pragma once

#include "IShader.h"
//VS_Indexed�N���X��,�e���v���[�g�p�����[�^�Ƃ���ID3D11VertexShader���g�p����IShader����p��
class VS_Indexed : public IShader<ID3D11VertexShader>
{
public:
  //�t�@�C���p�X��Direct3D�f�o�C�X�̃|�C���^���󂯎��R���X�g���N�^
  VS_Indexed(const std::wstring& file, ID3D11Device1* device);
  //�f�X�g���N�^
  ~VS_Indexed();

  //IShader����I�[�o�[���C�h���ꂽ�V�F�[�_�[���A�N�e�B�x�[�g����
  virtual void ActivateShader(ID3D11DeviceContext1* context) override;
  //IShader����I�[�o�[���C�h���ꂽ�萔�o�b�t�@���o�C���h����
  virtual void BindConstantBuffers(ID3D11DeviceContext1* context, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers) override;

private:
  //���_�V�F�[�_�[�̓��̓��C�A�E�g�C���^�[�t�F�[�X���Ǘ�����X�}�[�g�|�C���^
  Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
};
