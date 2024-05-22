#pragma once

#include <vector>

#include "Global.h"

class Character
{
public:
  //enum class����enum���g�p(�C���f�b�N�X�Ƃ��Ďg�p���邽��)
  enum Direction
  {
    Up,         //�����
    Right,      //�E����
    Down,       //������
    Left,       //������
    _Count      //�����̑���
  };

  enum class Movement
  {
    Up,         //������Ɉړ�
    Right,      //�E�����Ɉړ�
    Down,       //�������Ɉړ�
    Left,       //�������Ɉړ�
    Stop,       //��~
    InHouse     //�Ƃ̒�
  };

  struct InstanceType
  {
    DirectX::XMFLOAT3 position;     //�ʒu
    uint8_t sizeFlag;               //�T�C�Y�t���O
  };
  //�R���X�g���N�^
  Character();
  //�f�X�g���N�^
  ~Character();
  //�ʒu�𒲐�
  void AdjustPosition(float x, float y, float z);
  //�t���[�����X�V����
  void UpdateFrame(double elapsedTime);
  //����������(RGB�l���w��j
  void Init(ID3D11Device1* device, float r, float g, float b);
  //������
  void Init(ID3D11Device1* device);
  //�`�揈��
  void Draw(ID3D11DeviceContext1* context);
  //�}�b�v�ɍ��킹�Ĉʒu�𒲐�
  void AlignToMap();

  //�t���[���J�E���^�𑝉�
  void IncreaseFrameCounter();
  //�t���[���J�E���^�����Z�b�g
  void ResetFrameCounter();
  //�ړ������𔽓]
  void ReverseMovementDirection();
  //�A�j���[�V�������ĊJ�n
  void Restart() { m_totalElapsed = 0; m_isAnimationDone = false; }

  //�ʒu��ݒ�
  void SetPosition(float x, float y, float z);
  //�ړ���ݒ�
  void SetMovement(Movement movement);
  //�X�v���C�g��Y���W��ݒ�
  void SetSpriteY(uint8_t spriteY) { m_spriteY = spriteY; }
  //�X�v���C�g��X���W��ݒ�
  void SetSpriteX(uint8_t spriteX) { m_currentFrame = spriteX; }
  //�X�v���C�g�V�[�g�̗񐔂ƍs����ݒ�
  void SetColumnsAndRowsOfAssociatedSpriteSheet(uint8_t columns, uint8_t rows);
  //�X�v���C�g�̃X�P�[���t�@�N�^�[��ݒ�
  void SetSpriteScaleFactor(float scale) { m_spriteScaleFactor = scale; }
  //�X�v���C�g��X���W�̒ǉ��l��ݒ�
  void SetSpriteXAddition(uint8_t xAddition) { m_spriteXAddition = xAddition; }
  //��Ԃ��Ƃ̃t���[������ݒ�
  void SetFramesPerState(uint8_t frames) { m_framesPerState = frames; }
  //������̎��Ԃ�ݒ�
  void SetOneCycle(float oneCycle) { m_oneCycle = oneCycle; }
  //���S�t���O��ݒ�
  void SetDead(bool dead) { m_isDead = dead; }

  //���[���h�s����擾
  const DirectX::XMFLOAT4X4& GetWorldMatrix() const { return m_worldMatrix; }
  //�ʒu���擾
  const DirectX::XMFLOAT3& GetPosition() const { return m_position; }

  //�����Ă���������擾
  Direction GetFacingDirection() const { return m_facingDirection; }
  //�ړ����擾
  Movement GetMovement() const { return m_movement; }

  //�X�v���C�g�̃X�P�[���t�@�N�^�[���擾
  float GetSpriteScaleFactor() const { return m_spriteScaleFactor; }

  //�t���[�������擾
  uint8_t GetNumberOfFrames() const { return m_frameCounter; }
  //�X�v���C�g��Y���W���擾
  uint8_t GetSpriteY() const { return m_spriteY; }
  //�X�v���C�g��X���W���擾
  uint8_t GetSpriteX() const { return m_currentFrame; }
  //�X�v���C�g�V�[�g�̗񐔂��擾
  uint8_t GetSpriteSheetColumns() const { return m_spriteSheetColumns; }
  //�X�v���C�g�V�[�g�̍s�����擾
  uint8_t GetSpriteSheetRows() const { return m_spriteSheetRows; }

  //�A�j���[�V�������������������擾
  bool IsAnimationDone() { return m_isAnimationDone; }
  //���S���������擾
  bool IsDead() { return m_isDead; }
  //�������Ă��邩���擾
  bool IsAlive() { return !m_isDead; }

private:
  //���[���h�s����X�V
  void UpdateWorldMatrix();

  //DirectX�֘A�̃����o�ϐ�
  Microsoft::WRL::ComPtr<ID3D11Buffer>          m_vertexBuffer;
  Microsoft::WRL::ComPtr<ID3D11SamplerState>    m_samplerState;
  Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_cullNone;
  Microsoft::WRL::ComPtr<ID3D11Buffer>          m_instanceBuffer;

  //���[���h�s��
  DirectX::XMFLOAT4X4 m_worldMatrix;
  //�ʒu
  DirectX::XMFLOAT3 m_position;

  //�C���X�^���X�̃��X�g
  std::vector<InstanceType> m_instances;
  //���_�̃��X�g
  std::vector<Global::Vertex> m_vertices;

  //���݂̈ړ�
  Movement m_movement;
  //�����Ă������
  Direction m_facingDirection;

  uint8_t m_currentFrame;            //���݂̃t���[��
  uint8_t m_spriteY;                 //�X�v���C�g��Y���W
  uint8_t m_frameCounter;            //�t���[���J�E���^
  uint8_t m_spriteSheetColumns;      //�X�v���C�g�V�[�g�̗�
  uint8_t m_spriteSheetRows;         //�X�v���C�g�V�[�g�̍s��
  uint8_t m_spriteXAddition;         //�X�v���C�g��X���W�̒ǉ��l
  uint8_t m_framesPerState;          //��Ԃ��Ƃ̃t���[����

  float m_spriteScaleFactor;         //�X�v���C�g�̃X�P�[���t�@�N�^�[

  double m_totalElapsed;             //���v�o�ߎ���
  double m_timePerFrame;             //�t���[�����Ƃ̎���

  bool m_oneCycle;                   //������̃t���O
  bool m_isAnimationDone;            //�A�j���[�V���������t���O
  bool m_isDead;                     //���S�t���O
};

