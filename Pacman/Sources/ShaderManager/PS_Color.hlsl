#include "Common.h"

//�s�N�Z���V�F�[�_�[�̃G���g���[�|�C���g
//GS_Output�^��input�������Ɏ��Afloat4�^��Ԃ�
//float4�^�́A4�̕��������_�����܂ރx�N�g����\���B
float4 main(GS_Output input) : SV_Target
{
  //input.color��RGB�l�ɃA���t�@�l1.0f��ǉ�����float4�^�̒l��Ԃ��B
  //�s�����ȐF���s�N�Z���V�F�[�_�[��
  return float4(input.color, 1.0f);
}