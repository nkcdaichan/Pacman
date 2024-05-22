#pragma once

namespace Global
{

//�L�����N�^�[�̏�Ԃ�\���񋓌^
enum class Mode
{
  Chase,          //�ǂ��������[�h
  Scatter,        //�U��΂胂�[�h
  Frightened      //���т����[�h
};
//�J�����̈ʒu�萔
constexpr DirectX::XMFLOAT3 frontCamera {10.5f, 5.0f, -2.5f};
constexpr DirectX::XMFLOAT3 upCamera {10.5f, 15.0f, 10.5f};
//�Q�[���ݒ�萔
constexpr uint8_t worldSize = 21;                            //���[���h�̃T�C�Y
constexpr uint8_t minFramesPerDirection = 3;                 //�������Ƃ̍ŏ��t���[����
constexpr uint8_t phasesNum = 9;                             //�t�F�[�Y�̐�
 
constexpr uint8_t rowBlinky = 0;                             //�u�����L�[�̍s
constexpr uint8_t rowPinky = 1;                              //�s���L�[�̍s
constexpr uint8_t rowInky = 2;                               //�C���L�[�̍s
constexpr uint8_t rowClyde = 3;                              //�N���C�h�̍s
constexpr uint8_t rowFrightened = 4;                         //���т����[�h�̍s
constexpr uint8_t rowTransition = 5;                         //�J�ڃ��[�h�̍s
constexpr uint8_t rowDead = 6;                               //���S���[�h�̍s
constexpr uint8_t numGhosts = 4;                             //�S�[�X�g�̐�

//�L�����N�^�[�̑��x�萔
constexpr float pacManSpeed = 0.1f;                           //�p�b�N�}���̑��x
constexpr float ghostSpeed = 0.08f;                           //�S�[�X�g�̑��x
constexpr float pacManSize = 1.0f;                            //�p�b�N�}���̃T�C�Y
constexpr float ghostSize = 1.0f;                             //�S�[�X�g�̃T�C�Y
constexpr float pacManHalfSize = pacManSize / 2.0f;           //�p�b�N�}���̔����̃T�C�Y

//���_�f�[�^�\����
struct Vertex
{
  DirectX::XMFLOAT3 position;                                 //�ʒu                     
  DirectX::XMFLOAT3 normal;                                   //�@��
  DirectX::XMFLOAT3 color;                                    //�F
  DirectX::XMFLOAT2 texCoord;                                 //�e�N�X�`�����W
};
//�t���[�����Ƃ̃J�����f�[�^�\����
struct CameraPerFrame
{
  DirectX::XMFLOAT4X4 view;                                    //�r���[�s��
  DirectX::XMFLOAT4X4 projection;                              //�ˉe�s��
};
//�A���C�������g�̊m�F
static_assert((sizeof(CameraPerFrame) % 16) == 0, "CameraPerFrame is not aligned!");

//�I�u�W�F�N�g���Ƃ̃J�����f�[�^�\����
struct CameraPerObject
{
  //���[���h�s��
  DirectX::XMFLOAT4X4 world;
};

//�A���C�������g�̊m�F
static_assert((sizeof(CameraPerObject) % 16) == 0, "CameraPerObject is not aligned!");

//�X�v���C�g�萔�o�b�t�@�\����
struct SpriteConstantBuffer
{
  uint32_t spriteX;                                             //�X�v���C�g��X���W
  uint32_t spriteY;                                             //�X�v���C�g��Y���W
  uint32_t spriteSheetColumns;                                  //�X�v���C�g�V�[�g�̗�
  uint32_t spriteSheetRows;                                     //�X�v���C�g�V�[�g�̍s��

  DirectX::XMFLOAT4 billboardSize_0_0_0;                        //�r���{�[�h�T�C�Y
};
//�A���C�������g�̊m�F
static_assert((sizeof(SpriteConstantBuffer) % 16) == 0, "SpriteConstantBuffer is not aligned!");

//�����萔�o�b�t�@�\����
struct LightConstantBuffer
{
  //�����̒l
  DirectX::XMFLOAT4 values;
};
//�A���C�������g�̊m�F
static_assert((sizeof(LightConstantBuffer) % 16) == 0, "LightConstantBuffer is not aligned!");
}
