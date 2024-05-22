#include "pch.h"

#include "Global.h"
#include "Ghost.h"
//�R���X�g���N�^
Ghost::Ghost():
  m_dotLimit(0),                                      //�H�ׂ��h�b�g�̐��̐�����������
  m_eatenDots(0),                                     //�H�ׂ��h�b�g�̐���������
  m_mode(Global::Mode::Scatter)                       //�������[�h���u�U��΂�v�ɐݒ�
{
  SetMovement(Character::Movement::Stop);             //�ړ����~��Ԃɐݒ�
  SetColumnsAndRowsOfAssociatedSpriteSheet(8, 7);     //�X�v���C�g�V�[�g�̗�ƍs��ݒ�
  SetSpriteScaleFactor(Global::ghostSize);            //�X�v���C�g�̃X�P�[����ݒ�
  SetFramesPerState(2);                               //��Ԃ��Ƃ̃t���[������ݒ�
}
