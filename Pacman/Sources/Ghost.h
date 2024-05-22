#pragma once

#include "Global.h"
#include "Character.h"

class Ghost : public Character
{
public:
  //コンストラクタ
  Ghost();
  //デストラクタ
  ~Ghost() {};
  //モード設定
  void SetMode(Global::Mode mode) { m_mode = mode; }
  //ドット制限数の設定
  void SetDotLimit(uint8_t dotLimit) { m_dotLimit = dotLimit; }
  //食べたドット数をインクリメント
  void IncrementEatenDots() { m_eatenDots++; }
  //食べたドット数をリセット
  void ResetEatenDots() { m_eatenDots = 0; }

  //ゴーストがハウスを出る準備ができているか判定
  bool ReadyToLeaveHouse() { return m_eatenDots >= m_dotLimit; }
  //現在のモードを取得
  Global::Mode GetMode() { return m_mode; }

private:
  //ドット制限数
  uint8_t m_dotLimit;
  //食べたドット数
  uint8_t m_eatenDots;
  //現在のモード
  Global::Mode m_mode;
};

