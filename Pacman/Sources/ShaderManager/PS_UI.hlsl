//�e�N�X�`�����`
Texture2D textureUI : register(t0);
//�T���v���[�X�e�[�g��`
SamplerState samplerState : register(s0);
//position:���_�V�F�[�_�[����n�����s�N�Z���̈ʒu(SV_POSITION)
//texCoord:�e�N�X�`�����W (TexCoord)
//�V�F�[�_�[�̏o�� (SV_Target)
float4 main(float4 position : SV_POSITION, float2 texCoord : TexCoord) : SV_Target
{
  //�w�肳�ꂽ�e�N�X�`�����W�Ńe�N�X�`�����T���v�����O
  return textureUI.Sample(samplerState, texCoord);
}
