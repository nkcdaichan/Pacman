#include "pch.h"

#include "Global.h"
#include "Ghost.h"
//コンストラクタ
Ghost::Ghost():
  m_dotLimit(0),                                      //食べたドットの数の制限を初期化
  m_eatenDots(0),                                     //食べたドットの数を初期化
  m_mode(Global::Mode::Scatter)                       //初期モードを「散らばり」に設定
{
  SetMovement(Character::Movement::Stop);             //移動を停止状態に設定
  SetColumnsAndRowsOfAssociatedSpriteSheet(8, 7);     //スプライトシートの列と行を設定
  SetSpriteScaleFactor(Global::ghostSize);            //スプライトのスケールを設定
  SetFramesPerState(2);                               //状態ごとのフレーム数を設定
}
