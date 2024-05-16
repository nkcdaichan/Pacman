#include "Common.h"
//�J�����̃r���[�ƃv���W�F�N�V�����s����i�[����萔�o�b�t�@
cbuffer cameraPerFrame : register(b0)
{
  //�r���[�s��
  matrix view;
  //�v���W�F�N�V�����s��
  matrix projection;
};
//�I�u�W�F�N�g�̃��[���h�s����i�[����萔�o�b�t�@
cbuffer cameraPerObject : register(b1)
{
  //���[���h�s��
  matrix world;
};
//���_�V�F�[�_�[�̃��C���֐�
VS_Output main(VS_Input_Indexed input)
{
  VS_Output output;
  //���͒��_�̈ʒu��float4�ɕϊ�(w������1.0�ɐݒ�)
  float4 pos = float4(input.pos, 1.0f);
  //���[���h�s��Œ��_�ʒu��ϊ�
  pos = mul(pos, world);
  //���[���h���W�n�ł̈ʒu
  output.pos_w = pos;
  //�r���[�s��ŕϊ�
  pos = mul(pos, view);
  //�r���[���W�n�ł̈ʒu
  output.pos_wv = pos;
  //�v���W�F�N�V�����s��ŕϊ�
  pos = mul(pos, projection);
  //�N���b�v��Ԃł̈ʒu
  output.pos_wvp = pos;
  //���͒��_�̖@����float4�ɕϊ�(w������0.0�ɐݒ�)
  float4 normal = float4(input.normal, 0.0f);
  //���[���h�s��Ŗ@����ϊ�
  normal = mul(normal, world);
  //�ϊ���̖@��
  output.normal = normal.xyz;
  //���͒��_�̐F�����̂܂܏o�͂ɃR�s�[
  output.color = input.color;
  //�T�C�Y�t���O��������
  output.sizeFlag = 0;
  //�V�F�[�_�[�̏o�͂�Ԃ�
  return output;
}
