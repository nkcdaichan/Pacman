#include "Common.h"

// ���e�s��̒萔�o�b�t�@�i�X���b�gb0�j
cbuffer ProjectionMatrix : register(b0)
{
  // ���e�s��
  matrix projection;
}

// �t���[�����Ƃ̒萔�o�b�t�@�i�X���b�gb1�j
cbuffer FrameConstantBuffer : register(b1)
{
  //X�C���f�b�N�X
  uint spriteX;
  //Y�C���f�b�N�X
  uint spriteY;
  //�V�[�g�̗�
  uint spriteSheetColumns;
  //�V�[�g�̍s��
  uint spriteSheetRows;
  //�r���{�[�h�T�C�Y
  float4 billboardSize_0_0_0;
};
//���C���֐�(�ő咸�_����4�ɐݒ�)
[maxvertexcount(4)]
void main(point VS_Output input[1], inout TriangleStream<GS_Output> outputStream)
{
	//GS_Output�̃C���X�^���X���쐬���A���͂���̒l��ݒ�
  GS_Output a;
  // ���͂̐F��ݒ�
  a.color = input[0].color;
  // ���̖͂@����ݒ�
  a.normal = input[0].normal;
  // ���͂̈ʒu�����[���h�r���[��ԂŐݒ�
  a.pos = input[0].pos_wv;
  // 4�̒��_���쐬
  GS_Output b = a;
  GS_Output c = a;
  GS_Output d = a;

  //�T�C�Y��������
  float billboardSize = 0;
  //�r���{�[�h�T�C�Y��ݒ�
  if (input[0].sizeFlag == 1)
    billboardSize = billboardSize_0_0_0.x;
  else if (input[0].sizeFlag == 2)
    billboardSize = billboardSize_0_0_0.y;

  //4�̒��_�̈ʒu���v�Z
  a.pos.xy += float2(-0.5, -0.5) * billboardSize;
  b.pos.xy += float2(0.5, -0.5) * billboardSize;
  c.pos.xy += float2(-0.5, 0.5) * billboardSize;
  d.pos.xy += float2(0.5, 0.5) * billboardSize;

  //���e�s����g���Ē��_�ʒu��ϊ�
  a.pos = mul(a.pos, projection);
  b.pos = mul(b.pos, projection);
  c.pos = mul(c.pos, projection);
  d.pos = mul(d.pos, projection);

  //�X�v���C�g�V�[�g�̃I�t�Z�b�g���v�Z
  float xOffset = 1.0f / spriteSheetColumns;
  float yOffset = 1.0f / spriteSheetRows;

  //�e�N�X�`�����W��ݒ�
  a.texCoord = float2(spriteX * xOffset, (spriteY + 1) * yOffset);
  b.texCoord = float2((spriteX + 1) * xOffset, (spriteY + 1) * yOffset);
  c.texCoord = float2(spriteX * xOffset, spriteY * yOffset);
  d.texCoord = float2((spriteX + 1) * xOffset, spriteY * yOffset);

  //�v�Z�������_��outputStream�ɒǉ�
  outputStream.Append(a);
  outputStream.Append(b);
  outputStream.Append(c);
  outputStream.Append(d);
}
