

#pragma once

#include <cmath>
#include <exception>
#include <stdint.h>

namespace DX
{
  //�A�j���[�V������V�~�����[�V�����̃^�C�~���O�̂��߂̃w���p�[�N���X
  class StepTimer
  {
  public:
    StepTimer() noexcept(false) :
      m_elapsedTicks(0),
      m_totalTicks(0),
      m_leftOverTicks(0),
      m_frameCount(0),
      m_framesPerSecond(0),
      m_framesThisSecond(0),
      m_qpcSecondCounter(0),
      m_isFixedTimeStep(false),
      m_targetElapsedTicks(TicksPerSecond / 60)
    {
      if (!QueryPerformanceFrequency(&m_qpcFrequency))
      {
        throw std::exception("QueryPerformanceFrequency");
      }

      if (!QueryPerformanceCounter(&m_qpcLastTime))
      {
        throw std::exception("QueryPerformanceCounter");
      }

	  //�ő�f���^��1/10�b�ɏ�����
      m_qpcMaxDelta = static_cast<uint64_t>(m_qpcFrequency.QuadPart / 10);
    }

	//���O��Update�Ăяo������̌o�ߎ��Ԃ��擾
    uint64_t GetElapsedTicks() const { return m_elapsedTicks; }
    double GetElapsedSeconds() const { return TicksToSeconds(m_elapsedTicks); }

	//�v���O�����J�n����̍��v���Ԃ��擾
    uint64_t GetTotalTicks() const { return m_totalTicks; }
    double GetTotalSeconds() const { return TicksToSeconds(m_totalTicks); }

	//�v���O�����J�n����̍��v�X�V�񐔂��擾
    uint32_t GetFrameCount() const { return m_frameCount; }

	//���݂̃t���[�����[�g���擾
    uint32_t GetFramesPerSecond() const { return m_framesPerSecond; }

	//�Œ�܂��͉ς̃^�C���X�e�b�v���[�h���g�p���邩�ǂ�����ݒ�
    void SetFixedTimeStep(bool isFixedTimestep) { m_isFixedTimeStep = isFixedTimestep; }

	//�Œ�^�C���X�e�b�v���[�h�ł�Update�̌Ăяo���p�x��ݒ�
    void SetTargetElapsedTicks(uint64_t targetElapsed) { m_targetElapsedTicks = targetElapsed; }
    void SetTargetElapsedSeconds(double targetElapsed) { m_targetElapsedTicks = SecondsToTicks(targetElapsed); }

	//�����`����1�b������10000000�e�B�b�N���g�p���Ď��Ԃ�\��
    static const uint64_t TicksPerSecond = 10000000;

    static double TicksToSeconds(uint64_t ticks) { return static_cast<double>(ticks) / TicksPerSecond; }
    static uint64_t SecondsToTicks(double seconds) { return static_cast<uint64_t>(seconds * TicksPerSecond); }

	//�̈ӂ̃^�C�~���O�̕s�A�����i���Ƃ��΃u���b�L���OIO����j�̌�ɁA
	//�Œ�^�C���X�e�b�v���W�b�N���L���b�`�A�b�v��Update�Ăяo�������݂Ȃ��悤�ɂ��邽�߂�
	//������Ăяo��

    void ResetElapsedTime()
    {
      if (!QueryPerformanceCounter(&m_qpcLastTime))
      {
        throw std::exception("QueryPerformanceCounter");
      }

      m_leftOverTicks = 0;
      m_framesPerSecond = 0;
      m_framesThisSecond = 0;
      m_qpcSecondCounter = 0;
    }

	//�^�C�}�[�̏�Ԃ��X�V��,�w�肳�ꂽUpdate�֐���K�؂ȉ񐔌Ăяo��
    template<typename TUpdate>
    void Tick(const TUpdate& update)
    {
	  //���݂̎������N�G������
      LARGE_INTEGER currentTime;

      if (!QueryPerformanceCounter(&currentTime))
      {
        throw std::exception("QueryPerformanceCounter");
      }

      uint64_t timeDelta = currentTime.QuadPart - m_qpcLastTime.QuadPart;

      m_qpcLastTime = currentTime;
      m_qpcSecondCounter += timeDelta;

	  //�ߏ�ɑ傫�Ȏ��ԍ����N�����v(�f�o�b�K�[�ňꎞ��~������Ȃ�)
      if (timeDelta > m_qpcMaxDelta)
      {
        timeDelta = m_qpcMaxDelta;
      }

	  //QPC���j�b�g��W���I�ȃe�B�b�N�`���ɕϊ� ����͑O�̃N�����v�̂��߂ɃI�[�o�[�t���[���邱�Ƃ͂Ȃ�
      timeDelta *= TicksPerSecond;
      timeDelta /= m_qpcFrequency.QuadPart;

      uint32_t lastFrameCount = m_frameCount;

      if (m_isFixedTimeStep)
      {
		  //�Œ�^�C���X�e�b�v�̍X�V���W�b�N

		  //�A�v�����ڕW�̌o�ߎ��ԂƔ��ɋ߂��Ԋu�Ŏ��s����Ă���ꍇ�i1/4�~���b�ȓ��j�A�N���b�N��
		  //�ڕW�l�Ɗ��S�Ɉ�v����悤�ɃN�����v����B����ɂ��A�����Ȗ����ł��Ȃ��G���[��
		  //���ԂƂƂ��ɒ~�ς����̂�h���B���̃N�����v���s��Ȃ��ƁA60 fps�̌Œ�X�V��v������
		  //�Q�[�����A59.94 NTSC�f�B�X�v���C�Ő����������L���ȏ�ԂŎ��s�����ꍇ�A
		  //�ŏI�I�ɏ\���ɏ������G���[���~�ς��A�t���[���������Ă��܂��B

        if (static_cast<uint64_t>(std::abs(static_cast<int64_t>(timeDelta - m_targetElapsedTicks))) < TicksPerSecond / 4000)
        {
          timeDelta = m_targetElapsedTicks;
        }

        m_leftOverTicks += timeDelta;

        while (m_leftOverTicks >= m_targetElapsedTicks)
        {
          m_elapsedTicks = m_targetElapsedTicks;
          m_totalTicks += m_targetElapsedTicks;
          m_leftOverTicks -= m_targetElapsedTicks;
          m_frameCount++;

          update();
        }
      }
      else
      {
		//�σ^�C���X�e�b�v�̍X�V���W�b�N
        m_elapsedTicks = timeDelta;
        m_totalTicks += timeDelta;
        m_leftOverTicks = 0;
        m_frameCount++;

        update();
      }

	  //���݂̃t���[�����[�g��ǐ�
      if (m_frameCount != lastFrameCount)
      {
        m_framesThisSecond++;
      }

      if (m_qpcSecondCounter >= static_cast<uint64_t>(m_qpcFrequency.QuadPart))
      {
        m_framesPerSecond = m_framesThisSecond;
        m_framesThisSecond = 0;
        m_qpcSecondCounter %= m_qpcFrequency.QuadPart;
      }
    }

  private:
	//�\�[�X�̃^�C�~���O�f�[�^��QPC���j�b�g���g�p
    LARGE_INTEGER m_qpcFrequency;
    LARGE_INTEGER m_qpcLastTime;
    uint64_t m_qpcMaxDelta;

	//�h�������^�C�~���O�f�[�^�͕W���I�ȃe�B�b�N�`�����g�p
    uint64_t m_elapsedTicks;
    uint64_t m_totalTicks;
    uint64_t m_leftOverTicks;

	//�t���[�����[�g��ǐՂ��邽�߂̃����o�[
    uint32_t m_frameCount;
    uint32_t m_framesPerSecond;
    uint32_t m_framesThisSecond;
    uint64_t m_qpcSecondCounter;

	//�Œ�^�C���X�e�b�v���[�h���\�����邽�߂̃����o�[
    bool m_isFixedTimeStep;
    uint64_t m_targetElapsedTicks;
  };
}
