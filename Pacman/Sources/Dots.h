#pragma once

#include <vector>

#include "Global.h"

class Dots
{
public:
  enum class Type
  {
    Nothing,      //�����Ȃ����
    Normal,       //���ʂ̃h�b�g
    Extra,        //���ʂȃh�b�g
    LastOne       //�Ō�̈��
  };

  struct InstanceType
  {
    DirectX::XMFLOAT3 position;      //���W
    uint8_t sizeFlag;                //�T�C�Y�t���O
  };

  //�R���X�g���N�^
  explicit Dots();
  //�f�X�g���N�^
  ~Dots();

  //�`��
  void Draw(ID3D11DeviceContext1* context);
  //������
  void Init(ID3D11Device1* device);

  //�X�V
  void Update(uint8_t column, uint8_t row, ID3D11DeviceContext1* context, Type& dotEaten);

  //���[���h�s��̎擾
  const DirectX::XMFLOAT4X4& GetWorldMatrix() const { return m_worldMatrix; }

private:
  Microsoft::WRL::ComPtr<ID3D11Buffer>             m_vertexBuffer;          //���_�o�b�t�@
  Microsoft::WRL::ComPtr<ID3D11RasterizerState>    m_cullNone;              //�J�����O�Ȃ��̃��X�^���C�U�X�e�[�g
  Microsoft::WRL::ComPtr<ID3D11Resource>           m_resource;              //���\�[�X
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;    //�V�F�[�_�[���\�[�X�r���[
  Microsoft::WRL::ComPtr<ID3D11SamplerState>       m_samplerState;          //�T���v���X�e�[�g
  Microsoft::WRL::ComPtr<ID3D11BlendState>         m_blendState;            //�u�����h�X�e�[�g
  Microsoft::WRL::ComPtr<ID3D11Buffer>             m_instanceBuffer;        //�C���X�^���X�o�b�t�@

  //���[���h�s��
  DirectX::XMFLOAT4X4 m_worldMatrix;

  //�h�b�g�̔z��
  uint8_t m_dots[Global::worldSize][Global::worldSize];

  //�h�b�g�̐�
  uint16_t m_numberOfDots;

  //���_�̔z��
  std::vector<Global::Vertex> m_vertices;
  //�C���X�^���X�̔z��
  std::vector<InstanceType> m_instances;
};

