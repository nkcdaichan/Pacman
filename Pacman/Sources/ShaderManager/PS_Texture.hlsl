#include "Common.h"
//2D�e�N�X�`�����`��,�V�F�[�_�[���\�[�X�r���[(t0)�Ƀo�C���h
Texture2D     dotTexture    : register(t0);
//�T���v���[�X�e�[�g���`��,�T���v���[�X�e�[�g�r���[(s0)�Ƀo�C���h
SamplerState  samplerState  : register(s0);

//����: Geometry Shader(GS)����̏o�͂��󂯎��
//�o��: �s�N�Z���J���[(SV_Target)��Ԃ�
float4 main(GS_Output input) : SV_Target
{ 
  // �e�N�X�`�����W�Ńe�N�X�`�����T���v�����O��,����ꂽ�F��Ԃ�
  return dotTexture.Sample(samplerState, input.texCoord);
}