#include "Common.h"
//�J�����Ɋւ���萔(�t���[�����ƂɍX�V�j
cbuffer cameraPerFrame : register(b0)
{
  // �r���[�s�� �J�����̎��_
  matrix view;
  //�v���W�F�N�V�����s�� �J�����̓��e�ϊ�
  matrix projection;
};
//�J�����Ɋւ���萔(�I�u�W�F�N�g���ƂɍX�V�j
cbuffer cameraPerObject : register(b1)
{
  //���[���h�s�� �I�u�W�F�N�g�̕ϊ�
  matrix world;
};
//���_�V�F�[�_�[�̃G���g���|�C���g
VS_Output main(VS_Input_Instanced input)
{
  //�o�͍\����
  VS_Output output;
  //���_�ʒu��float4�^�ɕϊ��Aw������1.0f�ɐݒ�
  float4 pos = float4(input.adjPos, 1.0f);
  //���[���h�s��Œ��_�ʒu��ϊ�
  pos = mul(pos, world);
  //���[���h�ϊ���̒��_�ʒu���o��
  output.pos_w = pos;
  //�v���W�F�N�V�����s��Œ��_�ʒu��ϊ�
  pos = mul(pos, view);
  //���[���h�r���[�ˉe�ϊ���̒��_�ʒu���o��
  output.pos_wv = pos;
  //�v���W�F�N�V�����s��Œ��_�ʒu��ϊ�
  pos = mul(pos, projection);
  //���[���h�r���[�ˉe�ϊ���̒��_�ʒu���o��
  output.pos_wvp = pos;

  //���̑��̒��_�������o�͂ɃR�s�[
  //���_�J���[
  output.color = input.color;
  //���_�@��
  output.normal = input.normal;
  //�T�C�Y�t���O
  output.sizeFlag = input.sizeFlag;
  //�ϊ����ꂽ���_�f�[�^��Ԃ�
  return output;
}
