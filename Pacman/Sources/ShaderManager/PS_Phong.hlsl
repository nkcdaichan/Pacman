#include "Common.h"

//�萔�Ƃ��Ē�`���ꂽ�}�e���A���J���[
static const float3 materialColor = { 0.7f, 0.7f, 0.9f };
//�����̐F
static const float3 ambient = { 0.15f, 0.15f, 0.15f };
//�g�U���ˌ��̐F
static const float3 diffuseColor = { 1.0f, 1.0f, 1.0f };
//�g�U���ˌ��̋��x
static const float diffuseIntensity = 1.0f;
//���C�g�̌����W��
//�萔����
static const float attConst = 1.0f;
//���`����
static const float attLin = 0.045f;
//�񎟌���
static const float attQuad = 0.0075f;
//���C�g�̃p�����[�^���i�[����萔�o�b�t�@�A���W�X�^b0�ɔz�u
cbuffer lightParams : register(b0)
{
	//���C�g�̈ʒu�iw�����͎g���Ȃ��j
  float4 lightPos;
};
//�s�N�Z���V�F�[�_�[�̃G���g���[�|�C���g
float4 main(VS_Output input) : SV_Target
{
	//�t���O�����g(�s�N�Z��)���烉�C�g�ւ̃x�N�g��
  const float3 vToL = lightPos.xyz - input.pos_w.xyz;
    //�t���O�����g���烉�C�g�܂ł̋���
  const float distToL = length(vToL);
    //�t���O�����g���烉�C�g�ւ̒P�ʃx�N�g��
  const float3 dirToL = vToL / distToL;

  //�����W���̌v�Z
  const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));

  //�g�U���˂̋��x�v�Z
  const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f,dot(dirToL, input.normal));

  //�ŏI�I�ȐF�̌v�Z(�����Ɗg�U���˂����v���A�}�e���A���J���[���|�����킹��j
  return float4(saturate((diffuse + ambient) * materialColor),1.0f);
}
