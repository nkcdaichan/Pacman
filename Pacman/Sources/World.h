#pragma once

#include <vector>

#include "Global.h"

class World
{
public:
  //�R���X�g���N�^
  explicit World();
  //�f�X�g���N�^
  ~World();

  //���[���h�̏�����
  void Init(ID3D11Device1* device);
  //���[���h�̕`��
  void Draw(ID3D11DeviceContext1* context);
  //�ʍs�\���ǂ����𔻒�
  bool IsPassable(uint8_t column, uint8_t row, bool canGoHome);
  //�w�肵���ʒu�̃^�C�����擾
  uint8_t GetTile(uint8_t column, uint8_t row) { return m_map[row][column]; }
  //���[���h�ϊ��s����擾
  const DirectX::XMFLOAT4X4& GetWorldMatrix() const { return m_worldMatrix; }

private:
  //�u���b�N��ǉ�
  void AddBlock(float x, float z, float depth, bool north, bool west, bool south, bool east);
  //���[���h�̐���
  void Generate(ID3D11Device1* device);

  //���[���h�̃}�b�v�f�[�^
  const uint8_t m_map[Global::worldSize][Global::worldSize];

  //���_�o�b�t�@
  Microsoft::WRL::ComPtr<ID3D11Buffer>           m_vertexBuffer;
  //�C���f�b�N�X�o�b�t�@
  Microsoft::WRL::ComPtr<ID3D11Buffer>           m_indexBuffer;
  //���X�^���C�U�X�e�[�g
  Microsoft::WRL::ComPtr<ID3D11RasterizerState>  m_cullCW;

  //���[���h�ϊ��s��
  DirectX::XMFLOAT4X4 m_worldMatrix;

  //���_���
  std::vector<Global::Vertex> m_vertices;
  //�C���f�b�N�X���
  std::vector<uint16_t> m_indices;
};

