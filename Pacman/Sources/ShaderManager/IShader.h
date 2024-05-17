#pragma once

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <string>
#include <wrl.h>
//�e���v���[�g�N���XIShader�̒�`�B�V�F�[�_�[�̊��C���^�[�t�F�[�X���
template <typename T>
class IShader
{
public:
  //���z�f�X�g���N�^�B�h���N���X�œK�؂ɃN���[���A�b�v���s����悤��
  virtual ~IShader() {}
  //�V�F�[�_�[�ւ̃|�C���^���擾����
  T*  GetShader()   { return m_shader.Get(); }
  //�V�F�[�_�[�ւ̃|�C���^�̃A�h���X���擾����
  T** GetShaderPP() { return m_shader.GetAddressOf(); }
  //�������z�֐��Ȃ̂ŃV�F�[�_�[���A�N�e�B�u�ɂ��邽�߂ɔh���N���X�Ŏ�������K�v������
  //�������z�֐������N���X�͒��ۃN���X�ƂȂ�,���̃N���X�̃C���X�^���X�𒼐ڐ������邱�Ƃ͂ł��Ȃ�
  //�������z�֐���,�h���N���X�ŕK���I�[�o�[���C�h����邱�Ƃ�O��
  virtual void ActivateShader(ID3D11DeviceContext1* context) = 0;
  //�萔�o�b�t�@���o�C���h���邽�߂ɔh���N���X�Ŏ�������K�v������B������������z�֐�
  virtual void BindConstantBuffers(ID3D11DeviceContext1* context, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers) = 0;

private:
  //Microsoft::WRL::ComPtr��COM�I�u�W�F�N�g�̃X�}�[�g�|�C���^�B�e���v���[�g����T�^�̃V�F�[�_�[���Ǘ�
  Microsoft::WRL::ComPtr<T> m_shader;
};