#pragma once

#include "ShaderManager/ShaderManager.h"

#include "Camera.h"
#include "Caption.h"
#include "Dots.h"
#include "Global.h"
#include "Ghost.h"
#include "Character.h"
#include "Keyboard.h"
#include "StepTimer.h"
#include "World.h"

//クラスの宣言
class Game
{
public:
  //ゲームの状態を表す列挙型
  enum class State
  {
    Intro,   //イントロ画面
    Start,   //ゲーム開始
    Level,   //レベルプレイ中
    Dead     //プレイヤーが死亡
  };
  //ゴーストの種類を表す列挙型
  enum class Ghosts
  {
    Blinky,  //ブリンキー
    Pinky,   //ピンキー
    Inky,    //インキー
    Clyde,   //クライド
    None     //ゴーストなし
  };
  //フェーズの構造体
  struct Phase
  {
    Global::Mode mode;     //モード
    double startingTime;   //開始時間
    double duration;       //継続時間
  };

  Game() noexcept;

  //初期化と管理
  void Initialize(HWND window, uint16_t width, uint16_t height);

  //基本的なゲームループ
  void Tick();

  //メッセージ処理
  void OnActivated();
  void OnDeactivated();
  void OnSuspending();
  void OnResuming();
  void OnWindowSizeChanged(uint16_t width, uint16_t height);

  //プロパティ
  void GetDefaultSize(uint16_t& width, uint16_t& height) const;

private:
  //ワールド描画
  void DrawWorld();
  //イントロ画面描画
  void DrawIntro();
  //スプライト描画
  void DrawSprites();
  //デバッグ描画
  void DrawDebug();

  //各ゴーストの位置更新
  void UpdatePositionOfBlinky();
  void UpdatePositionOfPinky();
  void UpdatePositionOfInky();
  void UpdatePositionOfClyde();
  void Update(const DX::StepTimer& timer);

  //レンダリング処理
  void Render();
  //クリア処理
  void Clear();
  //表示処理
  void Present();

  //デバイス作成
  void CreateDevice();
  //リソース作成
  void CreateResources();

  //デバイスロスト処理
  void OnDeviceLost();

  //キャラクター用のスプライト定数バッファ設定
  void SetSpriteConstantBufferForCharacter(Global::SpriteConstantBuffer& spriteConstantBuffer, const Character& character);
  //ゴーストを特定の位置に移動
  void MoveGhostTowardsPosition(float posX, float posZ, Game::Ghosts ghost);
  //ゴーストをランダムな位置に移動
  void MoveGhostTowardsRandomPosition(Game::Ghosts ghost);
  //ゴーストのデフォルトスプライト設定
  void SetGhostsDefaultSprites();
  //フェーズ作成
  void CreatePhases();
  //衝突処理
  void HandleCollisions();
  //スタートアニメーション用のカメラ更新
  void UpdateCameraForStartAnimation();
  //新しいゲームの初期化
  void NewGameInitialization();

  //移動が逆または同じかどうかを判定
  bool AreMovementsOppositeOrSame(Character::Movement m1, Character::Movement m2);

  //キャラクター間の距離を計算
  float DistanceBetweenCharacters(const Character& ch1, const Character& ch2);

  //Direct3D11のデバイスとコンテキスト
  D3D_FEATURE_LEVEL                                 m_featureLevel;
  Microsoft::WRL::ComPtr<ID3D11Device1>             m_d3dDevice;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext1>      m_d3dContext;
  Microsoft::WRL::ComPtr<IDXGISwapChain1>           m_swapChain;
  Microsoft::WRL::ComPtr<ID3D11RenderTargetView>    m_renderTargetView;
  Microsoft::WRL::ComPtr<ID3D11DepthStencilView>    m_depthStencilView;
  Microsoft::WRL::ComPtr<ID3D11Buffer>              m_projectionMatrixConstantBuffer;
  Microsoft::WRL::ComPtr<ID3D11Buffer>              m_frameBuffer;
  Microsoft::WRL::ComPtr<ID3D11Buffer>              m_cameraPerFrame;
  Microsoft::WRL::ComPtr<ID3D11Buffer>              m_cameraPerObject;
  Microsoft::WRL::ComPtr<ID3D11Buffer>              m_light;
  Microsoft::WRL::ComPtr<ID3D11Resource>            m_pacManResource;
  Microsoft::WRL::ComPtr<ID3D11Resource>            m_ghostsResource;
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  m_pacManShaderResourceView;
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  m_ghostsShaderResourceView;

  //ウィンドウハンドル
  HWND m_window;
  //ステップタイマー
  DX::StepTimer m_timer;
  //パックマンの移動要求
  Character::Movement m_pacmanMovementRequest;

  //カメラ
  Camera m_camera;
  //ワールド
  World m_world;

  //ユニークポインタによるリソース管理
  std::unique_ptr<Dots> m_dots;
  std::unique_ptr<Caption> m_caption;
  std::unique_ptr<ShaderManager> m_shaderManager;
  std::unique_ptr<DirectX::Keyboard> m_keyboard;

  //デバッグポイントのベクトル
  std::vector<Global::Vertex> m_debugPoints;
  //ゴーストの配列
  std::array<std::unique_ptr<Ghost>, Global::numGhosts> m_ghosts;
  //パックマンのユニークポインタ
  std::unique_ptr<Character> m_pacman;

  //レベル1のフェーズ配列
  std::array<Phase, Global::phasesNum> m_phasesLevel1; 

  //現在のフェーズインデックス
  uint8_t m_currentPhaseIndex;
  //前のフェーズインデックス
  uint8_t m_previousPhaseIndex;
  //現在のゴーストカウンター
  Ghosts m_currentGhostCounter;

  //デバッグ描画フラグ
  bool m_debugDraw;
  //おびえ状態遷移フラグ
  bool m_frightenedTransition;
  //ゲーム一時停止フラグ
  bool m_gamePaused;

  //出力の幅と高さ
  uint16_t m_outputWidth;
  uint16_t m_outputHeight;
  //ゲームの状態
  State m_gameState;
};
