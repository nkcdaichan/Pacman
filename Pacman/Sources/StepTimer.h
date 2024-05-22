

#pragma once

#include <cmath>
#include <exception>
#include <stdint.h>

namespace DX
{
  //アニメーションやシミュレーションのタイミングのためのヘルパークラス
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

	  //最大デルタを1/10秒に初期化
      m_qpcMaxDelta = static_cast<uint64_t>(m_qpcFrequency.QuadPart / 10);
    }

	//直前のUpdate呼び出しからの経過時間を取得
    uint64_t GetElapsedTicks() const { return m_elapsedTicks; }
    double GetElapsedSeconds() const { return TicksToSeconds(m_elapsedTicks); }

	//プログラム開始からの合計時間を取得
    uint64_t GetTotalTicks() const { return m_totalTicks; }
    double GetTotalSeconds() const { return TicksToSeconds(m_totalTicks); }

	//プログラム開始からの合計更新回数を取得
    uint32_t GetFrameCount() const { return m_frameCount; }

	//現在のフレームレートを取得
    uint32_t GetFramesPerSecond() const { return m_framesPerSecond; }

	//固定または可変のタイムステップモードを使用するかどうかを設定
    void SetFixedTimeStep(bool isFixedTimestep) { m_isFixedTimeStep = isFixedTimestep; }

	//固定タイムステップモードでのUpdateの呼び出し頻度を設定
    void SetTargetElapsedTicks(uint64_t targetElapsed) { m_targetElapsedTicks = targetElapsed; }
    void SetTargetElapsedSeconds(double targetElapsed) { m_targetElapsedTicks = SecondsToTicks(targetElapsed); }

	//整数形式は1秒あたり10000000ティックを使用して時間を表す
    static const uint64_t TicksPerSecond = 10000000;

    static double TicksToSeconds(uint64_t ticks) { return static_cast<double>(ticks) / TicksPerSecond; }
    static uint64_t SecondsToTicks(double seconds) { return static_cast<uint64_t>(seconds * TicksPerSecond); }

	//故意のタイミングの不連続性（たとえばブロッキングIO操作）の後に、
	//固定タイムステップロジックがキャッチアップのUpdate呼び出しを試みないようにするために
	//これを呼び出す

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

	//タイマーの状態を更新し,指定されたUpdate関数を適切な回数呼び出す
    template<typename TUpdate>
    void Tick(const TUpdate& update)
    {
	  //現在の時刻をクエリする
      LARGE_INTEGER currentTime;

      if (!QueryPerformanceCounter(&currentTime))
      {
        throw std::exception("QueryPerformanceCounter");
      }

      uint64_t timeDelta = currentTime.QuadPart - m_qpcLastTime.QuadPart;

      m_qpcLastTime = currentTime;
      m_qpcSecondCounter += timeDelta;

	  //過剰に大きな時間差をクランプ(デバッガーで一時停止した後など)
      if (timeDelta > m_qpcMaxDelta)
      {
        timeDelta = m_qpcMaxDelta;
      }

	  //QPCユニットを標準的なティック形式に変換 これは前のクランプのためにオーバーフローすることはない
      timeDelta *= TicksPerSecond;
      timeDelta /= m_qpcFrequency.QuadPart;

      uint32_t lastFrameCount = m_frameCount;

      if (m_isFixedTimeStep)
      {
		  //固定タイムステップの更新ロジック

		  //アプリが目標の経過時間と非常に近い間隔で実行されている場合（1/4ミリ秒以内）、クロックを
		  //目標値と完全に一致するようにクランプする。これにより、小さな無視できないエラーが
		  //時間とともに蓄積されるのを防ぐ。このクランプを行わないと、60 fpsの固定更新を要求した
		  //ゲームが、59.94 NTSCディスプレイで垂直同期が有効な状態で実行される場合、
		  //最終的に十分に小さいエラーが蓄積し、フレームが落ちてしまう。

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
		//可変タイムステップの更新ロジック
        m_elapsedTicks = timeDelta;
        m_totalTicks += timeDelta;
        m_leftOverTicks = 0;
        m_frameCount++;

        update();
      }

	  //現在のフレームレートを追跡
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
	//ソースのタイミングデータはQPCユニットを使用
    LARGE_INTEGER m_qpcFrequency;
    LARGE_INTEGER m_qpcLastTime;
    uint64_t m_qpcMaxDelta;

	//派生したタイミングデータは標準的なティック形式を使用
    uint64_t m_elapsedTicks;
    uint64_t m_totalTicks;
    uint64_t m_leftOverTicks;

	//フレームレートを追跡するためのメンバー
    uint32_t m_frameCount;
    uint32_t m_framesPerSecond;
    uint32_t m_framesThisSecond;
    uint64_t m_qpcSecondCounter;

	//固定タイムステップモードを構成するためのメンバー
    bool m_isFixedTimeStep;
    uint64_t m_targetElapsedTicks;
  };
}
