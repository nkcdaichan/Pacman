#pragma once

#include <vector>

#include "Global.h"

class Character
{
public:
  //enum classよりもenumを使用(インデックスとして使用するため)
  enum Direction
  {
    Up,         //上方向
    Right,      //右方向
    Down,       //下方向
    Left,       //左方向
    _Count      //方向の総数
  };

  enum class Movement
  {
    Up,         //上方向に移動
    Right,      //右方向に移動
    Down,       //下方向に移動
    Left,       //左方向に移動
    Stop,       //停止
    InHouse     //家の中
  };

  struct InstanceType
  {
    DirectX::XMFLOAT3 position;     //位置
    uint8_t sizeFlag;               //サイズフラグ
  };
  //コンストラクタ
  Character();
  //デストラクタ
  ~Character();
  //位置を調整
  void AdjustPosition(float x, float y, float z);
  //フレームを更新する
  void UpdateFrame(double elapsedTime);
  //初期化する(RGB値を指定）
  void Init(ID3D11Device1* device, float r, float g, float b);
  //初期化
  void Init(ID3D11Device1* device);
  //描画処理
  void Draw(ID3D11DeviceContext1* context);
  //マップに合わせて位置を調整
  void AlignToMap();

  //フレームカウンタを増加
  void IncreaseFrameCounter();
  //フレームカウンタをリセット
  void ResetFrameCounter();
  //移動方向を反転
  void ReverseMovementDirection();
  //アニメーションを再開始
  void Restart() { m_totalElapsed = 0; m_isAnimationDone = false; }

  //位置を設定
  void SetPosition(float x, float y, float z);
  //移動を設定
  void SetMovement(Movement movement);
  //スプライトのY座標を設定
  void SetSpriteY(uint8_t spriteY) { m_spriteY = spriteY; }
  //スプライトのX座標を設定
  void SetSpriteX(uint8_t spriteX) { m_currentFrame = spriteX; }
  //スプライトシートの列数と行数を設定
  void SetColumnsAndRowsOfAssociatedSpriteSheet(uint8_t columns, uint8_t rows);
  //スプライトのスケールファクターを設定
  void SetSpriteScaleFactor(float scale) { m_spriteScaleFactor = scale; }
  //スプライトのX座標の追加値を設定
  void SetSpriteXAddition(uint8_t xAddition) { m_spriteXAddition = xAddition; }
  //状態ごとのフレーム数を設定
  void SetFramesPerState(uint8_t frames) { m_framesPerState = frames; }
  //一周期の時間を設定
  void SetOneCycle(float oneCycle) { m_oneCycle = oneCycle; }
  //死亡フラグを設定
  void SetDead(bool dead) { m_isDead = dead; }

  //ワールド行列を取得
  const DirectX::XMFLOAT4X4& GetWorldMatrix() const { return m_worldMatrix; }
  //位置を取得
  const DirectX::XMFLOAT3& GetPosition() const { return m_position; }

  //向いている方向を取得
  Direction GetFacingDirection() const { return m_facingDirection; }
  //移動を取得
  Movement GetMovement() const { return m_movement; }

  //スプライトのスケールファクターを取得
  float GetSpriteScaleFactor() const { return m_spriteScaleFactor; }

  //フレーム数を取得
  uint8_t GetNumberOfFrames() const { return m_frameCounter; }
  //スプライトのY座標を取得
  uint8_t GetSpriteY() const { return m_spriteY; }
  //スプライトのX座標を取得
  uint8_t GetSpriteX() const { return m_currentFrame; }
  //スプライトシートの列数を取得
  uint8_t GetSpriteSheetColumns() const { return m_spriteSheetColumns; }
  //スプライトシートの行数を取得
  uint8_t GetSpriteSheetRows() const { return m_spriteSheetRows; }

  //アニメーションが完了したかを取得
  bool IsAnimationDone() { return m_isAnimationDone; }
  //死亡したかを取得
  bool IsDead() { return m_isDead; }
  //生存しているかを取得
  bool IsAlive() { return !m_isDead; }

private:
  //ワールド行列を更新
  void UpdateWorldMatrix();

  //DirectX関連のメンバ変数
  Microsoft::WRL::ComPtr<ID3D11Buffer>          m_vertexBuffer;
  Microsoft::WRL::ComPtr<ID3D11SamplerState>    m_samplerState;
  Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_cullNone;
  Microsoft::WRL::ComPtr<ID3D11Buffer>          m_instanceBuffer;

  //ワールド行列
  DirectX::XMFLOAT4X4 m_worldMatrix;
  //位置
  DirectX::XMFLOAT3 m_position;

  //インスタンスのリスト
  std::vector<InstanceType> m_instances;
  //頂点のリスト
  std::vector<Global::Vertex> m_vertices;

  //現在の移動
  Movement m_movement;
  //向いている方向
  Direction m_facingDirection;

  uint8_t m_currentFrame;            //現在のフレーム
  uint8_t m_spriteY;                 //スプライトのY座標
  uint8_t m_frameCounter;            //フレームカウンタ
  uint8_t m_spriteSheetColumns;      //スプライトシートの列数
  uint8_t m_spriteSheetRows;         //スプライトシートの行数
  uint8_t m_spriteXAddition;         //スプライトのX座標の追加値
  uint8_t m_framesPerState;          //状態ごとのフレーム数

  float m_spriteScaleFactor;         //スプライトのスケールファクター

  double m_totalElapsed;             //合計経過時間
  double m_timePerFrame;             //フレームごとの時間

  bool m_oneCycle;                   //一周期のフラグ
  bool m_isAnimationDone;            //アニメーション完了フラグ
  bool m_isDead;                     //死亡フラグ
};

