#pragma once

#include "Global.h"
#include "Character.h"

class Ghost : public Character
{
public:
  //�R���X�g���N�^
  Ghost();
  //�f�X�g���N�^
  ~Ghost() {};
  //���[�h�ݒ�
  void SetMode(Global::Mode mode) { m_mode = mode; }
  //�h�b�g�������̐ݒ�
  void SetDotLimit(uint8_t dotLimit) { m_dotLimit = dotLimit; }
  //�H�ׂ��h�b�g�����C���N�������g
  void IncrementEatenDots() { m_eatenDots++; }
  //�H�ׂ��h�b�g�������Z�b�g
  void ResetEatenDots() { m_eatenDots = 0; }

  //�S�[�X�g���n�E�X���o�鏀�����ł��Ă��邩����
  bool ReadyToLeaveHouse() { return m_eatenDots >= m_dotLimit; }
  //���݂̃��[�h���擾
  Global::Mode GetMode() { return m_mode; }

private:
  //�h�b�g������
  uint8_t m_dotLimit;
  //�H�ׂ��h�b�g��
  uint8_t m_eatenDots;
  //���݂̃��[�h
  Global::Mode m_mode;
};

