#pragma once

#include <vector>

#include "Global.h"

class Caption
{
public:
  //�R���X�g���N�^
  Caption();
  //�f�X�g���N�^
  ~Caption();
  //�`�揈��
  void Draw(ID3D11DeviceContext1* context);
  //����������
  void Init(ID3D11Device1* device, uint16_t width, uint16_t height);
  //Y���W�𒲐�
  void AdjustY(float value, float minimum);
  //�ʒu��ݒ肷��
  void SetPosition(float x, float y, float z);
  //�����擾
  uint16_t GetWidth() { return m_width; }
  //�������擾
  uint16_t GetHeight() { return m_height; }

  //���[���h�s����擾
  const DirectX::XMFLOAT4X4& GetWorldMatrix() const { return m_worldMatrix; }

private:
  //���[���h�s����X�V
  void UpdateWorldMatrix();
  //���_�o�b�t�@
  Microsoft::WRL::ComPtr<ID3D11Buffer>             m_vertexBuffer;
  //�J�����O�Ȃ��̃��X�^���C�U�[�X�e�[�g
  Microsoft::WRL::ComPtr<ID3D11RasterizerState>    m_cullNone;
  //���\�[�X
  Microsoft::WRL::ComPtr<ID3D11Resource>           m_resource;
  //�V�F�[�_�[���\�[�X�r���[
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
  //�T���v���[�X�e�[�g
  Microsoft::WRL::ComPtr<ID3D11SamplerState>       m_samplerState;
  //�u�����h�X�e�[�g
  Microsoft::WRL::ComPtr<ID3D11BlendState>         m_blendState;
  //�C���f�b�N�X�o�b�t�@
  Microsoft::WRL::ComPtr<ID3D11Buffer>             m_indexBuffer;

  //�ʒu
  DirectX::XMFLOAT3 m_position;
  //���[���h�s��
  DirectX::XMFLOAT4X4 m_worldMatrix;

  //���_�f�[�^
  std::vector<Global::Vertex> m_vertices;
  //�C���f�b�N�X�f�[�^
  std::vector<uint16_t> m_indices;
  //��
  uint16_t m_width;
  //����
  uint16_t m_height;
};
