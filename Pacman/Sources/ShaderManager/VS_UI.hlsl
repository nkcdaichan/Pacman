#include "Common.h"
//�J�����Ɋ֘A����s��f�[�^��ێ�����
cbuffer cameraPerFrame : register(b0)
{
  //�r���[�s��
  matrix view;
  //�v���W�F�N�V�����s��
  matrix projection;
};
//�I�u�W�F�N�g�Ɋ֘A����s��f�[�^��ێ�����
cbuffer cameraPerObject : register(b1)
{
  //���[���h�s��
  matrix world;
};
//���_�V�F�[�_�[�̏o�͍\����
struct VOut
{
  //���_�̕ϊ���̈ʒu
  float4 position : SV_POSITION;
  //���_�̃e�N�X�`�����W
  float2 texCoord : TexCoord;
};
//���_�V�F�[�_�[�̃��C���֐�
VOut main(VS_Input_Indexed input)
{
  //�o�͍\���̂̃C���X�^���X
  VOut output;
  //���͒��_�̈ʒu��float4�ɕϊ�
  output.position = float4(input.pos, 1.0f);
  //���[���h�s��Œ��_�ʒu��ϊ�
  output.position = mul(output.position, world);
  //�r���[�s��Œ��_�ʒu��ϊ�
  output.position = mul(output.position, view);
  //�v���W�F�N�V�����s��Œ��_�ʒu��ϊ�
  output.position = mul(output.position, projection);
  //�e�N�X�`�����W���o�͂ɐݒ�
  output.texCoord = input.texCoord;

  return output;
}
