#include "pch.h"

#include <cmath>

#include "Game.h"
#include "WICTextureLoader.h"

extern void ExitGame();           //外部で定義されているゲーム終了関数を宣言

using namespace DirectX;

using Microsoft::WRL::ComPtr;

//マクロ定義
#define CURRENT_PHASE m_phasesLevel1[m_currentPhaseIndex]            //現在のフェーズを取得するマクロ

#define BLINKY m_ghosts[static_cast<uint8_t>(Game::Ghosts::Blinky)]  //Blinkyを取得するマクロ
#define PINKY m_ghosts[static_cast<uint8_t>(Game::Ghosts::Pinky)]    //Pinkyを取得するマクロ
#define INKY m_ghosts[static_cast<uint8_t>(Game::Ghosts::Inky)]      //Inkyを取得するマクロ
#define CLYDE m_ghosts[static_cast<uint8_t>(Game::Ghosts::Clyde)]    //Clydeを取得するマクロ
#define PACMAN m_pacman                                              //Pacmanを取得するマクロ

//コンストラクタ
Game::Game() noexcept :
  m_window(nullptr),                                                 //ウィンドウハンドルの初期化
  m_outputWidth(800),                                                //出力幅の初期化
  m_outputHeight(600),                                               //出力高さの初期化
  m_featureLevel(D3D_FEATURE_LEVEL_9_1),                             //DirectXの機能レベルの初期化
  m_pacmanMovementRequest(Character::Movement::Stop),                //Pacmanの初期移動要求を停止に設定
  m_debugDraw(false),                                                //デバッグ描画のフラグを初期化
  m_currentPhaseIndex(1),                                            //現在のフェーズインデックスを初期化
  m_previousPhaseIndex(1),                                           //前のフェーズインデックスを初期化
  m_frightenedTransition(false),                                     //恐怖モード遷移のフラグを初期化
  m_currentGhostCounter(Ghosts::Pinky),                              //現在のゴーストカウンターを初期化
  m_gameState(State::Intro),                                         //ゲーム状態をイントロに設定
  m_gamePaused(true)                                                 //ゲームの一時停止フラグを初期化
{
  CreatePhases();                                                    //フェーズを作成
}
//ゲームの初期化
void Game::Initialize(HWND window, uint16_t width, uint16_t height)
{
  assert(width > 1 && height > 1);                        //幅と高さが1より大きいことを確認

  m_window = window;                                      //ウィンドウハンドルを設定
  m_outputWidth = width;                                  //出力幅を設定
  m_outputHeight = height;                                //出力高さを設定

  CreateDevice();                                         //デバイスを作成

  m_caption = std::make_unique<Caption>();                //キャプションを作成
  m_caption->Init(m_d3dDevice.Get(), 400, 100);           //キャプションを初期化

  CreateResources();                                      //リソースを作成

  m_world.Init(m_d3dDevice.Get());                        //ワールドを初期化

  for (auto& ghost : m_ghosts)                            //ゴースト配列を初期化
  {
    ghost = std::make_unique<Ghost>();                    //ゴーストを作成
    ghost->Init(m_d3dDevice.Get());                       //ゴーストを初期化
  }

  PACMAN = std::make_unique<Character>();                 //Pacmanを作成
  PACMAN->Init(m_d3dDevice.Get());                        //Pacmanを初期化

  //Pacmanのテクスチャを読み込み
  DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), nullptr, L"Resources/pacman.png", m_pacManResource.GetAddressOf(), m_pacManShaderResourceView.GetAddressOf()));
  //ゴーストのテクスチャを読み込み
  DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), nullptr, L"Resources/ghosts.png", m_ghostsResource.GetAddressOf(), m_ghostsShaderResourceView.GetAddressOf()));

  //キーボードを作成
  m_keyboard = std::make_unique<Keyboard>();              

  //ライト定数バッファを作成
  Global::LightConstantBuffer lightCB;  
  //ライトの値を設定
  lightCB.values = XMFLOAT4(10.5f, 2.5f, 11.5f, 1.0f);

  //定数バッファのディスクリプタを設定
  D3D11_BUFFER_DESC cbd = {};
  cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  cbd.Usage = D3D11_USAGE_DYNAMIC;
  cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  cbd.MiscFlags = 0;
  cbd.ByteWidth = sizeof(Global::LightConstantBuffer);
  cbd.StructureByteStride = 0;

  //サブリソースデータを設定
  D3D11_SUBRESOURCE_DATA csd = {};
  csd.pSysMem = &lightCB;

  //ライトバッファを作成
  DX::ThrowIfFailed(m_d3dDevice->CreateBuffer(&cbd, &csd, &m_light));

  //ピクセルシェーダに定数バッファをバインド
  m_shaderManager->BindConstantBuffersToPixelShader(ShaderManager::PixelShader::Phong, m_light.GetAddressOf(), 1);

  //固定タイムステップを設定
  m_timer.SetFixedTimeStep(true);
  //目標経過秒を設定(60fps）
  m_timer.SetTargetElapsedSeconds(1.f / 60.f);
}

//ゲームのティック処理
void Game::Tick()
{
  //タイマーをティックし、アップデート関数を呼び出す
  m_timer.Tick([&]()
  {
    Update(m_timer);
  });

  //レンダリング
  Render();
}

//ゲームの更新処理
void Game::Update(const DX::StepTimer& timer)
{

  //キーボードの状態を取得
  const auto& kb = m_keyboard->GetState();

  //エスケープキーが押されたらゲーム終了
  if (kb.Escape)
    ExitGame();
  //スペースキーが押されたら...
  if (kb.Space)
  {
	//ゲーム状態がイントロならスタートに変更
    if (m_gameState == Game::State::Intro)
      m_gameState = Game::State::Start;

	//ゲーム状態がレベルなら
    if (m_gameState == Game::State::Level)
    {
	  //ゲームを再開
      m_gamePaused = false;
	  //Pacmanを左に移動
      m_pacmanMovementRequest = Character::Movement::Left;
    }
  }

  //Pacmanが死んでいてアニメーションが終了している場合
  if (PACMAN->IsDead() && PACMAN->IsAnimationDone() && m_gameState == Game::State::Level)
	//ゲーム状態をデッドに変更
    m_gameState = Game::State::Dead;

  //ゲーム状態による処理
  switch (m_gameState)
  {
  //イントロ状態
  case Game::State::Intro:

	//キャプションのY座標を調整
    m_caption->AdjustY(5, 80);
	//カメラのリープをリセット
    m_camera.ResetLerp();
    break;
  //スタート状態
  case Game::State::Start:
	//カメラをリープ
    m_camera.LerpBetweenCameraPositions(0.04f);

	//カメラのリープが完了したら
    if (m_camera.IsCameraLerpDone())
    {
	  //新しいゲームを初期化
      NewGameInitialization();
	  //ゲーム状態をレベルに変更
      m_gameState = Game::State::Level;
	  //現在のフェーズの開始時間を設定
      CURRENT_PHASE.startingTime = timer.GetTotalSeconds();
    }
    break;
  //レベル状態
  case Game::State::Level:
	//カメラのリープをリセット
    m_camera.ResetLerp();
    break;
  //デッド状態
  case Game::State::Dead:
	//カメラを逆リープ
    m_camera.InverseLerpBetweenCameraPositions(0.04f);

	// カメラのリープが完了したらゲーム状態をイントロに戻す
    if (m_camera.IsCameraLerpDone())
      m_gameState = Game::State::Intro;
    break;
  }

  //ゲームが開始されていない場合や,一時停止中の場合はシミュレーションをスキップし,フェーズのタイマーを進めない
  if (m_gameState != Game::State::Level || m_gamePaused)
  {
	//フェーズの開始時間を更新
    CURRENT_PHASE.startingTime = timer.GetTotalSeconds();
	//処理を中断
    return;
  }

  //フレームを更新
  //Pacmanのフレームを更新
  PACMAN->UpdateFrame(m_timer.GetElapsedSeconds());  
  //全てのゴーストのフレームを更新
  std::for_each(m_ghosts.begin(), m_ghosts.end(), [&](auto& ghost) { ghost->UpdateFrame(m_timer.GetElapsedSeconds()); });

  //フェーズの期間が経過し,かつ現在のフェーズインデックスが最大値未満の場合
  if (timer.GetTotalSeconds() >= (CURRENT_PHASE.startingTime + CURRENT_PHASE.duration) && m_currentPhaseIndex < Global::phasesNum - 1)
  {
	//フェーズインデックスが0の場合
    if (m_currentPhaseIndex == 0)
    {
	  //前のフェーズインデックスに戻す
      m_currentPhaseIndex = m_previousPhaseIndex;
	  //ゴーストのスプライトをデフォルトに設定
      SetGhostsDefaultSprites();
    }
    else
    {
	  //フェーズインデックスをインクリメント
      m_currentPhaseIndex++;
	  //全てのゴーストの移動方向を反転
      std::for_each(m_ghosts.begin(), m_ghosts.end(), [](auto& ghost) { ghost->ReverseMovementDirection(); });
    }
	//フェーズの開始時間を更新
    CURRENT_PHASE.startingTime = timer.GetTotalSeconds();

	//現在のモードをゴーストに強制的に適用
    std::for_each(m_ghosts.begin(), m_ghosts.end(), [&](auto& ghost)
    {
	  //ゴーストが生きている場合
      if (ghost->IsAlive())
		//ゴーストのモードを設定
        ghost->SetMode(CURRENT_PHASE.mode);
    });

	//恐怖モード遷移フラグをリセット
    m_frightenedTransition = false;
  }
  else if (CURRENT_PHASE.mode == Global::Mode::Frightened && (timer.GetTotalSeconds() >= (CURRENT_PHASE.startingTime + CURRENT_PHASE.duration / 2.0)) && !m_frightenedTransition)
  {
    std::for_each(m_ghosts.begin(), m_ghosts.end(), [](auto& ghost)
    {
	  //ゴーストが恐怖モードで生きている場合
      if (ghost->GetMode() == Global::Mode::Frightened && ghost->IsAlive())
		//ゴーストのスプライトY座標を変更
        ghost->SetSpriteY(Global::rowTransition);
    });
	//恐怖モード遷移フラグを設定
    m_frightenedTransition = true;
  }
  //現在のゴーストカウンターに応じた処理を
  switch (m_currentGhostCounter)
  {
  case Ghosts::Pinky:
	//次のゴーストにカウンターを進める
    m_currentGhostCounter = Ghosts::Inky;
	//Pinkyを家の中に移動
    PINKY->SetMovement(Character::Movement::InHouse);
    break;
  //Inkyが家を出る準備ができた場合
  case Ghosts::Inky:
    if (INKY->ReadyToLeaveHouse())
    {
	  //次のゴーストにカウンターを進める
      m_currentGhostCounter = Ghosts::Clyde;
	  //Inkyを家の中に移動
      INKY->SetMovement(Character::Movement::InHouse);
    }
    break;
  //Clydeが家を出る準備ができた場合
  case Ghosts::Clyde:
    if (CLYDE->ReadyToLeaveHouse())
    {
	  //ゴーストカウンターをリセット
      m_currentGhostCounter = Ghosts::None;
	  //Clydeを家の中に移動
      CLYDE->SetMovement(Character::Movement::InHouse);
    }
    break;
  }
  //Pacmanの現在の移動方向を取得
  const Character::Movement pacmanMovement = PACMAN->GetMovement();
  //Pacmanの現在の位置を取得
  const DirectX::XMFLOAT3& pacmanPosCurrent = PACMAN->GetPosition();

  //テレポート処理(反対側から出てくる処理)
  if (pacmanMovement == Character::Movement::Left && (pacmanPosCurrent.x - 0.5f) < Global::pacManSpeed)
  {
	//Pacmanを右端にテレポート
    PACMAN->SetPosition(20.5f, pacmanPosCurrent.y, pacmanPosCurrent.z);
	//処理を中断
    return;
  }
  else if (pacmanMovement == Character::Movement::Right && (20.5f - pacmanPosCurrent.x) < Global::pacManSpeed)
  {
	//Pacmanを左端にテレポート
    PACMAN->SetPosition(0.5f, pacmanPosCurrent.y, pacmanPosCurrent.z);
	//処理を中断
    return;
  }

  //Pacmanが死んでいる場合の処理
  if (PACMAN->IsDead())
	//処理を中断
    return;

  //水平および垂直位置合わせの判定
  bool isHorizontallyAligned = (fmod(pacmanPosCurrent.x - 0.5f, 1.0f) < Global::pacManSpeed);      //水平方向に位置合わせされているか
  bool isVerticallyAligned = (fmod(pacmanPosCurrent.z - 0.5f, 1.0f) < Global::pacManSpeed);        //垂直方向に位置合わせされているか

  bool moves[Character::Direction::_Count] = { false, false, false, false };                       //各方向への移動可能性を初期化

  //各方向の移動可能性を判定
  moves[Character::Direction::Up] = m_world.IsPassable(static_cast<uint8_t>(pacmanPosCurrent.x), static_cast<uint8_t>(pacmanPosCurrent.z + 1.0f), false);
  moves[Character::Direction::Right] = m_world.IsPassable(static_cast<uint8_t>(pacmanPosCurrent.x + 1.0f), static_cast<uint8_t>(pacmanPosCurrent.z), false);
  moves[Character::Direction::Down] = m_world.IsPassable(static_cast<uint8_t>(pacmanPosCurrent.x), static_cast<uint8_t>(pacmanPosCurrent.z - 1.0f), false);
  moves[Character::Direction::Left] = m_world.IsPassable(static_cast<uint8_t>(pacmanPosCurrent.x - 1.0f), static_cast<uint8_t>(pacmanPosCurrent.z), false);

  //キーボード入力に応じてPacmanの移動要求を設定
  if (isVerticallyAligned)
  {
    if (kb.Right && moves[Character::Direction::Right])
	  //右移動要求を設定
      m_pacmanMovementRequest = Character::Movement::Right;
    else if (kb.Left && moves[Character::Direction::Left])
	  //左移動要求を設定
      m_pacmanMovementRequest = Character::Movement::Left;
  }

  if (isHorizontallyAligned)
  {
    if (kb.Up && moves[Character::Direction::Up])
	  //上移動要求を設定
      m_pacmanMovementRequest = Character::Movement::Up;
    else if (kb.Down && moves[Character::Direction::Down])
	  //下移動要求を設定
      m_pacmanMovementRequest = Character::Movement::Down;
  }
  //Pacmanの移動処理
  auto pacmanHandleMovement = [&](bool isPassable, bool alignment)
  {
	//移動可能な場合
    if (isPassable)
    {
	  //移動方向が反対や同じでない場合
      if (!AreMovementsOppositeOrSame(m_pacmanMovementRequest, pacmanMovement))
		//Pacmanをマップに位置合わせ
        PACMAN->AlignToMap();
	  //移動要求が現在の移動方向と異なる場合
      if (m_pacmanMovementRequest != pacmanMovement)
		//移動方向を設定
        PACMAN->SetMovement(m_pacmanMovementRequest);
    }
	//位置合わせが必要な場合
    else if (alignment)
    {
	  //移動要求が現在の移動方向と同じ場合
      if (m_pacmanMovementRequest == pacmanMovement)
      {
		//Pacmanをマップに位置合わせ
        PACMAN->AlignToMap();
		//移動を停止
        PACMAN->SetMovement(Character::Movement::Stop);
      }
      else
		//移動要求を現在の移動方向に設定
        m_pacmanMovementRequest = pacmanMovement;
    }
  };
  //Pacmanの移動要求に応じた処理
  switch (m_pacmanMovementRequest)
  {
  case Character::Movement::Right:
	//右移動処理
    pacmanHandleMovement(moves[Character::Direction::Right], isHorizontallyAligned);
    break;
  case Character::Movement::Left:
	//左移動処理
    pacmanHandleMovement(moves[Character::Direction::Left], isHorizontallyAligned);
    break;
  case Character::Movement::Up:
	//上移動処理
    pacmanHandleMovement(moves[Character::Direction::Up], isVerticallyAligned);
    break;
  case Character::Movement::Down:
	//下移動処理
    pacmanHandleMovement(moves[Character::Direction::Down], isVerticallyAligned);
    break;
  default:
	//何もしない
    break;
  }

  switch (PACMAN->GetMovement())
  {
  case Character::Movement::Left:
    PACMAN->AdjustPosition(-Global::pacManSpeed, 0, 0);
    break;
  case Character::Movement::Right:
    PACMAN->AdjustPosition(Global::pacManSpeed, 0, 0);
    break;
  case Character::Movement::Up:
    PACMAN->AdjustPosition(0, 0, Global::pacManSpeed);
    break;
  case Character::Movement::Down:
    PACMAN->AdjustPosition(0, 0, -Global::pacManSpeed);
    break;
  default:
    // Nothing
    break;
  }

  //食べたドットの種類を取得
  Dots::Type dotEaten;
  m_dots->Update(static_cast<uint8_t>(pacmanPosCurrent.x), static_cast<uint8_t>(pacmanPosCurrent.z), m_d3dContext.Get(), dotEaten);

  //最後のドットを食べた場合,新しいゲームを初期化し,ゲーム状態をレベルに設定
  if (dotEaten == Dots::Type::LastOne)
  {
    NewGameInitialization();
    m_gameState = Game::State::Level;
  }
  //ドットを食べた場合,かつゴーストカウンターがリセットされていない場合,ゴーストの食べたドット数をインクリメント
  if (dotEaten != Dots::Type::Nothing && m_currentGhostCounter != Ghosts::None)
    m_ghosts[static_cast<uint8_t>(m_currentGhostCounter)]->IncrementEatenDots();
  //特殊ドットを食べた場合
  if (dotEaten == Dots::Type::Extra)
  {
    m_previousPhaseIndex = m_currentPhaseIndex == 0 ? m_previousPhaseIndex : m_currentPhaseIndex;    //フェーズインデックスの更新
	m_frightenedTransition = false;
    m_frightenedTransition = false;
    m_currentPhaseIndex = 0;                                                                         //恐怖モードを強制

    CURRENT_PHASE.startingTime = timer.GetTotalSeconds();                                            //フェーズの開始時間を更新

	//全てのゴーストに恐怖モードを適用
    std::for_each(m_ghosts.begin(), m_ghosts.end(), [](auto& ghost)
    {
      if (ghost->IsAlive())
      {
        ghost->SetMode(Global::Mode::Frightened);     //ゴーストのモードを恐怖に設定
        ghost->SetSpriteY(Global::rowFrightened);     //ゴーストのスプライトY座標を恐怖モードに変更
        ghost->ReverseMovementDirection();            //ゴーストの移動方向を反転
      }
    });
  }

  //各ゴーストの位置を更新
  UpdatePositionOfBlinky();
  UpdatePositionOfPinky();
  UpdatePositionOfInky();
  UpdatePositionOfClyde();

  //衝突処理をハンドリング
  HandleCollisions();
}

void Game::Render()
{
  //最初の更新前に何もレンダリングしない
  if (m_timer.GetFrameCount() == 0)
    return;

  Clear();
  //ゲームの状態に応じて描画
  switch (m_gameState)
  {
  case Game::State::Intro:
    DrawWorld();
    DrawIntro();
    break;
  case Game::State::Start:
    DrawWorld();
    break;
  case Game::State::Level:
    DrawWorld();
    DrawSprites();
    DrawDebug();
    break;
  case Game::State::Dead:
    DrawWorld();
    break;
  }

  Present();
}

//バックバッファをクリアするヘルパーメソッド
void Game::Clear()
{
  //ビューをクリア
  m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::Black);
  m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

  m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

  //ビューポートを設定
  CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));
  m_d3dContext->RSSetViewports(1, &viewport);

  m_shaderManager->ClearShadersFromThePipeline();
}

//バックバッファの内容を画面に表示.
void Game::Present()
{
   //第一引数はVSyncまでブロックするようにDXGIに指示し,
   //次のVSyncまでアプリケーションをスリープさせる。
   //これにより,表示されないフレームをレンダリングする無駄を防ぐ。
  HRESULT hr = m_swapChain->Present(1, 0);

  //デバイスがリセットされた場合,レンダラーを完全に再初期化する必要がある。
  if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    OnDeviceLost();
  else
    DX::ThrowIfFailed(hr);
}

//メッセージハンドラ
void Game::OnActivated()
{
}

void Game::OnDeactivated()
{
}

void Game::OnSuspending()
{
}

void Game::OnResuming()
{
  //レジューム時に経過時間をリセット
  m_timer.ResetElapsedTime();
}

void Game::OnWindowSizeChanged(uint16_t width, uint16_t height)
{
  assert(width > 1 && height > 1);

  m_outputWidth = width;
  m_outputHeight = height;

  //リソースを再作成
  CreateResources();
}

void Game::GetDefaultSize(uint16_t& width, uint16_t& height) const
{
  width = 800;
  height = 600;
}
//ゲームワールドを描画
void Game::DrawWorld()
{
  m_shaderManager->SetVertexShader(ShaderManager::VertexShader::Indexed);
  m_shaderManager->SetPixelShader(ShaderManager::PixelShader::Phong);

  Global::CameraPerObject cameraPerObjectConstantBuffer;
  cameraPerObjectConstantBuffer.world = m_world.GetWorldMatrix();

  m_shaderManager->UpdateConstantBuffer(m_cameraPerObject.Get(), &cameraPerObjectConstantBuffer, sizeof(cameraPerObjectConstantBuffer));

  //カメラ
  Global::CameraPerFrame cameraConstantBufferPerFrame = {};
  cameraConstantBufferPerFrame.view = m_camera.GetViewMatrix();
  cameraConstantBufferPerFrame.projection = m_camera.GetProjectionMatrix();

  m_shaderManager->UpdateConstantBuffer(m_cameraPerFrame.Get(), &cameraConstantBufferPerFrame, sizeof(cameraConstantBufferPerFrame));

  m_world.Draw(m_d3dContext.Get());
}
//イントロ画面を描画
void Game::DrawIntro()
{
  m_shaderManager->SetVertexShader(ShaderManager::VertexShader::UI);
  m_shaderManager->SetPixelShader(ShaderManager::PixelShader::UI);

  Global::CameraPerFrame cameraConstantBufferPerFrame = {};
  XMStoreFloat4x4(&cameraConstantBufferPerFrame.view, DirectX::XMMatrixIdentity());
  cameraConstantBufferPerFrame.projection = m_camera.GetOrthographicMatrix();

  m_shaderManager->UpdateConstantBuffer(m_cameraPerFrame.Get(), &cameraConstantBufferPerFrame, sizeof(cameraConstantBufferPerFrame));

  Global::CameraPerObject cameraPerObjectConstantBuffer = {};
  cameraPerObjectConstantBuffer.world = m_caption->GetWorldMatrix();

  m_shaderManager->UpdateConstantBuffer(m_cameraPerObject.Get(), &cameraPerObjectConstantBuffer, sizeof(cameraPerObjectConstantBuffer));

  m_caption->Draw(m_d3dContext.Get());
}

void Game::DrawSprites()
{
  //ドットを描画
  m_shaderManager->SetVertexShader(ShaderManager::VertexShader::Instanced);        //インスタンス化された頂点シェーダーを設定
  m_shaderManager->SetGeometryShader(ShaderManager::GeometryShader::Billboard);    //ビルボードのジオメトリシェーダーを設定
  m_shaderManager->SetPixelShader(ShaderManager::PixelShader::Texture);            //テクスチャのピクセルシェーダーを設定

  //スプライトの定数バッファの設定
  Global::SpriteConstantBuffer spriteConstantBuffer = { 0, 0, 1, 1, DirectX::XMFLOAT4(0.2f, 0.4f, 0, 0) };
  //フレームの定数バッファを更新
  m_shaderManager->UpdateConstantBuffer(m_frameBuffer.Get(), &spriteConstantBuffer, sizeof(spriteConstantBuffer));

  //カメラの定数バッファを初期化
  Global::CameraPerObject cameraPerObjectConstantBuffer = {};
  //カメラの定数バッファを初期化
  cameraPerObjectConstantBuffer.world = m_dots->GetWorldMatrix();
  //カメラの定数バッファを更新
  m_shaderManager->UpdateConstantBuffer(m_cameraPerObject.Get(), &cameraPerObjectConstantBuffer, sizeof(cameraPerObjectConstantBuffer));
  //ドットを描画
  m_dots->Draw(m_d3dContext.Get());

  //パックマンを描画
  m_d3dContext->PSSetShaderResources(0, 1, m_pacManShaderResourceView.GetAddressOf());

  //パックマンのスプライト定数バッファを設定
  SetSpriteConstantBufferForCharacter(spriteConstantBuffer, *PACMAN);
  //フレームの定数バッファを更新
  m_shaderManager->UpdateConstantBuffer(m_frameBuffer.Get(), &spriteConstantBuffer, sizeof(spriteConstantBuffer));

  //パックマンのワールド行列を設定
  cameraPerObjectConstantBuffer.world = PACMAN->GetWorldMatrix();

  //カメラの定数バッファを更新
  m_shaderManager->UpdateConstantBuffer(m_cameraPerObject.Get(), &cameraPerObjectConstantBuffer, sizeof(cameraPerObjectConstantBuffer));

  //パックマンを描画
  PACMAN->Draw(m_d3dContext.Get());

  //ゴーストを描画
  //パックマンが生きている場合のみ描画
  if (PACMAN->IsAlive())
  {
	//ゴーストのシェーダーリソースビューを設定
    m_d3dContext->PSSetShaderResources(0, 1, m_ghostsShaderResourceView.GetAddressOf());

    std::for_each(m_ghosts.begin(), m_ghosts.end(), [&](auto& ghost)
    {
	  //ゴーストのスプライト定数バッファを設定
      SetSpriteConstantBufferForCharacter(spriteConstantBuffer, *ghost);
	  //フレームの定数バッファを更新
      m_shaderManager->UpdateConstantBuffer(m_frameBuffer.Get(), &spriteConstantBuffer, sizeof(spriteConstantBuffer));

	  //ゴーストのワールド行列を設定
      cameraPerObjectConstantBuffer.world = ghost->GetWorldMatrix();
	  //カメラの定数バッファを更新
      m_shaderManager->UpdateConstantBuffer(m_cameraPerObject.Get(), &cameraPerObjectConstantBuffer, sizeof(cameraPerObjectConstantBuffer));
	  //ゴーストを描画
      ghost->Draw(m_d3dContext.Get());
    });
  }
}

void Game::DrawDebug()
{
  if (!m_debugDraw)
    return;

  for (auto& debugPoint : m_debugPoints)
  {
	//デバッグ用のポイントを表すダミーキャラクターを作成
    Character dummy;
	//ダミーキャラクターの初期化
    dummy.Init(m_d3dDevice.Get(), debugPoint.color.x, debugPoint.color.y, debugPoint.color.z);
	//ダミーキャラクターの位置を設定
    dummy.SetPosition(debugPoint.position.x, debugPoint.position.y, debugPoint.position.z);

	//インスタンス化された頂点シェーダーを設定
    m_shaderManager->SetVertexShader(ShaderManager::VertexShader::Instanced);
	//ビルボードのジオメトリシェーダーを設定
    m_shaderManager->SetGeometryShader(ShaderManager::GeometryShader::Billboard);
	//色付けのピクセルシェーダーを設定
    m_shaderManager->SetPixelShader(ShaderManager::PixelShader::Color);

	//フレームの定数バッファを設定
    Global::SpriteConstantBuffer frameConstantBuffer = { 0, 0, 1, 1, DirectX::XMFLOAT4(0.25f, 0, 0, 0) };
	//フレームの定数バッファを更新
    m_shaderManager->UpdateConstantBuffer(m_frameBuffer.Get(), &frameConstantBuffer, sizeof(frameConstantBuffer));
	//カメラの定数バッファを初期化
    Global::CameraPerObject cameraPerObjectConstantBuffer;
	//ダミーキャラクターのワールド行列を設定
    cameraPerObjectConstantBuffer.world = dummy.GetWorldMatrix();
	//カメラの定数バッファを更新
    m_shaderManager->UpdateConstantBuffer(m_cameraPerObject.Get(), &cameraPerObjectConstantBuffer, sizeof(cameraPerObjectConstantBuffer));
	//ダミーキャラクターを描画
    dummy.Draw(m_d3dContext.Get());
  }
  //デバッグポイントをクリア
  m_debugPoints.clear();
}

void Game::MoveGhostTowardsPosition(float posX, float posZ, Game::Ghosts ghost)
{
  //対象のゴーストを取得
  Ghost& character = *m_ghosts[static_cast<uint8_t>(ghost)];

  if (m_debugDraw)
    switch (ghost)
    {
      case Ghosts::Blinky:
		//デバッグポイントを追加(Blinky)
        m_debugPoints.push_back({{posX, 0.3f, posZ}, {0.0f ,1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}});
        break;
      case Ghosts::Pinky:
		//デバッグポイントを追加(Pinky)
        m_debugPoints.push_back({{posX, 0.3f, posZ}, {0.0f ,1.0f, 0.0f}, {1.0f, 0.6f, 0.8f}});
        break;
      case Ghosts::Inky:
		//デバッグポイントを追加(Inky)
        m_debugPoints.push_back({{posX, 0.3f, posZ}, {0.0f ,1.0f, 0.0f}, {0.2f, 1.0f, 1.0f}});
        break;
      case Ghosts::Clyde:
		//デバッグポイントを追加(Clyde)
        m_debugPoints.push_back({{posX, 0.3f, posZ}, {0.0f ,1.0f, 0.0f}, {1.0f, 0.8f, 0.2f}});
        break;
    }
  //フレームカウンターを増加
  character.IncreaseFrameCounter();

  switch (character.GetMovement())
  {
  case Character::Movement::Left:
	//左に移動
    character.AdjustPosition(-Global::ghostSpeed, 0, 0);
    break;
  case Character::Movement::Right:
	//右に移動
    character.AdjustPosition(Global::ghostSpeed, 0, 0);
    break;
  case Character::Movement::Up:
	//上に移動
    character.AdjustPosition(0, 0, Global::ghostSpeed);
    break;
  case Character::Movement::Down:
	//下に移動
    character.AdjustPosition(0, 0, -Global::ghostSpeed);
    break;
  default:
	//ここに到達することはありません//
    break;
  }

  //ゴーストの現在位置を取得
  const DirectX::XMFLOAT3& characterCurrentPos = character.GetPosition();
  //ゴーストが死亡しているかを確認
  bool isCharacterDead = character.IsDead() || (character.GetMovement() == Character::Movement::InHouse);

  if (isCharacterDead)
  {
	//ゴーストとターゲット位置との距離を計算
    float d = sqrt((characterCurrentPos.x - posX) * (characterCurrentPos.x - posX) + (characterCurrentPos.z - posZ) * (characterCurrentPos.z - posZ));

    if (d < 0.1f)
    {
	  //マップに合わせて位置を調整
      character.AlignToMap();
	  //移動方向を上に設定
      character.SetMovement(Character::Movement::Up);
	  //モードをチェイスに設定
      character.SetMode(Global::Mode::Chase);
	  //ゴーストを生き返らせる
      character.SetDead(false);

      switch (ghost)
      {
      case Ghosts::Blinky:
	    //Blinkyのスプライト行を設定
        character.SetSpriteY(Global::rowBlinky);
        break;
      case Ghosts::Pinky:
		//Pinkyのスプライト行を設定
        character.SetSpriteY(Global::rowPinky);
        break;
      case Ghosts::Inky:
		//Inkyのスプライト行を設定
        character.SetSpriteY(Global::rowInky);
        break;
      case Ghosts::Clyde:
		//Clydeのスプライト行を設定
        character.SetSpriteY(Global::rowClyde);
        break;
      }
	  //関数の終了
      return;
    }
  }

  if (character.GetMovement() == Character::Movement::Left)
  {
	//キャラクターが左に移動している場合
    bool teleport = (characterCurrentPos.x - 0.5f) < Global::ghostSpeed ? true : false;

    if (teleport)
    {
	  //テレポートが必要な場合,キャラクターを右端に移動
      character.SetPosition(20.5f, characterCurrentPos.y, characterCurrentPos.z);
      return;
    }
  }
  else if (character.GetMovement() == Character::Movement::Right)
  {
	//キャラクターが右に移動している場合
    bool teleport = (20.5f - characterCurrentPos.x) < Global::ghostSpeed ? true : false;

    if (teleport)
    {
	  //テレポートが必要な場合,キャラクターを左端に移動
      character.SetPosition(0.5f, characterCurrentPos.y, characterCurrentPos.z);
      return;
    }
  }
  //キャラクターがマップのグリッドに揃っているかどうかをチェック
  bool isAligned = (fmod(characterCurrentPos.x - 0.5f, 1.0f) < Global::ghostSpeed) && (fmod(characterCurrentPos.z - 0.5f, 1.0f) < Global::ghostSpeed);

  if (isAligned)
  {
	//キャラクターがグリッドに揃っている場合
    bool moves[Character::Direction::_Count] = { false, false, false, false };

	//各方向への移動可能性をチェック
    moves[Character::Direction::Up] = m_world.IsPassable(static_cast<uint8_t>(characterCurrentPos.x), static_cast<uint8_t>(characterCurrentPos.z + 1.0f), isCharacterDead);
    moves[Character::Direction::Right] = m_world.IsPassable(static_cast<uint8_t>(characterCurrentPos.x + 1.0f), static_cast<uint8_t>(characterCurrentPos.z), isCharacterDead);
    moves[Character::Direction::Down] = m_world.IsPassable(static_cast<uint8_t>(characterCurrentPos.x), static_cast<uint8_t>(characterCurrentPos.z - 1.0f), isCharacterDead);
    moves[Character::Direction::Left] = m_world.IsPassable(static_cast<uint8_t>(characterCurrentPos.x - 1.0f), static_cast<uint8_t>(characterCurrentPos.z), isCharacterDead);
	//各方向への距離を計算
    std::array<float, Character::Direction::_Count> distances = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX };

    if (moves[Character::Direction::Up])
      distances[Character::Direction::Up] = sqrt((posX - characterCurrentPos.x) * (posX - characterCurrentPos.x) + (posZ - (characterCurrentPos.z + 1.0f)) * (posZ - (characterCurrentPos.z + 1.0f)));

    if (moves[Character::Direction::Down])
      distances[Character::Direction::Down] = sqrt((posX - characterCurrentPos.x) * (posX - characterCurrentPos.x) + (posZ - (characterCurrentPos.z - 1.0f)) * (posZ - (characterCurrentPos.z - 1.0f)));

    if (moves[Character::Direction::Left])
      distances[Character::Direction::Left] = sqrt((posX - (characterCurrentPos.x - 1.0f)) * (posX - (characterCurrentPos.x - 1.0f)) + (posZ - characterCurrentPos.z) * (posZ - characterCurrentPos.z));

    if (moves[Character::Direction::Right])
      distances[Character::Direction::Right] = sqrt((posX - (characterCurrentPos.x + 1.0f)) * (posX - (characterCurrentPos.x + 1.0f)) + (posZ - characterCurrentPos.z) * (posZ - characterCurrentPos.z));

    const Character::Movement characterMovement = character.GetMovement();

    switch (characterMovement)
    {
    case Character::Movement::Up:
      distances[Character::Direction::Down] = FLT_MAX;
      break;
    case Character::Movement::Down:
      distances[Character::Direction::Up] = FLT_MAX;
      break;
    case Character::Movement::Left:
      distances[Character::Direction::Right] = FLT_MAX;
      break;
    case Character::Movement::Right:
      distances[Character::Direction::Left] = FLT_MAX;
      break;
    }
	//最短距離の方向を新しい移動方向として設定
    Character::Movement newMoment = static_cast<Character::Movement>(std::min_element(distances.begin(), distances.end()) - distances.begin());

    if (newMoment != characterMovement && character.GetNumberOfFrames() >= Global::minFramesPerDirection)
    {
	  //移動方向が変わった場合、キャラクターをマップに揃え、新しい移動方向を設定
      character.AlignToMap();
      character.SetMovement(newMoment);
	  //フレームカウンターをリセット
      character.ResetFrameCounter();
    }
  }
}

void Game::MoveGhostTowardsRandomPosition(Game::Ghosts ghost)
{
  Ghost& character = *m_ghosts[static_cast<uint8_t>(ghost)];

  const DirectX::XMFLOAT3 charPos = character.GetPosition();
  uint8_t direction = rand() % 4;

  //ランダムな方向にゴーストを移動
  switch (direction)
  {
  case 0:
    MoveGhostTowardsPosition(charPos.x + 1.0f, charPos.z, ghost);
    break;
  case 1:
    MoveGhostTowardsPosition(charPos.x - 1.0f, charPos.z, ghost);
    break;
  case 2:
    MoveGhostTowardsPosition(charPos.x, charPos.z + 1.0f, ghost);
    break;
  case 3:
    MoveGhostTowardsPosition(charPos.x, charPos.z - 1.0f, ghost);
    break;
  }
}

void Game::SetGhostsDefaultSprites()
{
  //ゴーストが生きている場合は,デフォルトのスプライトを設定
  if (BLINKY->IsAlive()) BLINKY->SetSpriteY(Global::rowBlinky);
  if (PINKY->IsAlive())  PINKY->SetSpriteY(Global::rowPinky);
  if (INKY->IsAlive())   INKY->SetSpriteY(Global::rowInky);
  if (CLYDE->IsAlive())  CLYDE->SetSpriteY(Global::rowClyde);
}

void Game::CreatePhases()
{
  //レベル1の各フェーズを設定
  m_phasesLevel1[0] = { Global::Mode::Frightened, 0.0, 6.0 };
  m_phasesLevel1[1] = { Global::Mode::Scatter, 0.0, 7.0 };
  m_phasesLevel1[2] = { Global::Mode::Chase, 0.0, 20.0 };
  m_phasesLevel1[3] = { Global::Mode::Scatter, 0.0, 7.0 };
  m_phasesLevel1[4] = { Global::Mode::Chase, 0.0, 20.0 };
  m_phasesLevel1[5] = { Global::Mode::Scatter, 0.0, 5.0 };
  m_phasesLevel1[6] = { Global::Mode::Chase, 0.0, 20.0 };
  m_phasesLevel1[7] = { Global::Mode::Scatter, 0.0, 5.0 };
  m_phasesLevel1[8] = { Global::Mode::Chase, 0.0, 0.0 };
}

void Game::HandleCollisions()
{
  //各ゴーストとパックマンの距離を計算し,衝突を処理
  std::for_each(m_ghosts.begin(), m_ghosts.end(), [&](auto& ghost)
  {
    float distance = DistanceBetweenCharacters(*PACMAN, *ghost);

    if (distance < 0.1f)
    {
      if (ghost->GetMode() == Global::Mode::Frightened)
      {
        ghost->SetSpriteY(Global::rowDead);
        ghost->SetDead(true);
      }
      else
      {
        PACMAN->SetDead(true);
        PACMAN->SetSpriteX(0);
        PACMAN->SetSpriteY(1);
        PACMAN->SetFramesPerState(12);
        PACMAN->SetOneCycle(true);
        PACMAN->Restart();
      }
    }
  });
}

void Game::UpdateCameraForStartAnimation()
{
  //ゲーム開始アニメーションのためにカメラを更新
  Global::CameraPerFrame cameraConstantBufferPerFrame = {};
  cameraConstantBufferPerFrame.view = m_camera.GetViewMatrix();
  cameraConstantBufferPerFrame.projection = m_camera.GetProjectionMatrix();

  m_shaderManager->UpdateConstantBuffer(m_cameraPerFrame.Get(), &cameraConstantBufferPerFrame, sizeof(cameraConstantBufferPerFrame));
}

void Game::NewGameInitialization()
{
  //新しいゲームの初期化//

  //ドットのリセット
  if (m_dots)
    m_dots.reset();

  m_dots = std::make_unique<Dots>();
  m_dots->Init(m_d3dDevice.Get());

  //ゴーストの初期設定
  BLINKY->SetPosition(10.5f, 0.30f, 13.5f);
  BLINKY->SetMovement(Character::Movement::Left);
  BLINKY->SetMode(Global::Mode::Scatter);
  BLINKY->SetSpriteY(Global::rowBlinky);

  PINKY->SetPosition(10.5f, 0.31f, 11.5f);
  PINKY->SetMovement(Character::Movement::Stop);
  PINKY->SetMode(Global::Mode::Scatter);
  PINKY->SetSpriteX(0);
  PINKY->SetDotLimit(0);
  PINKY->ResetEatenDots();
  PINKY->SetSpriteY(Global::rowPinky);

  INKY->SetPosition(9.5f, 0.32f, 11.5f);
  INKY->SetMovement(Character::Movement::Stop);
  INKY->SetMode(Global::Mode::Scatter);
  INKY->SetSpriteX(0);
  INKY->SetDotLimit(30);
  INKY->ResetEatenDots();
  INKY->SetSpriteY(Global::rowInky);

  CLYDE->SetPosition(11.5f, 0.33f, 11.5f);
  CLYDE->SetMovement(Character::Movement::Stop);
  CLYDE->SetMode(Global::Mode::Scatter);
  CLYDE->SetSpriteX(0);
  CLYDE->SetDotLimit(60);
  CLYDE->ResetEatenDots();
  CLYDE->SetSpriteY(Global::rowClyde);

  //パックマンの初期設定
  PACMAN->SetDead(false);
  PACMAN->SetPosition(10.5f, 0.25f, 5.5f);
  PACMAN->SetMovement(Character::Movement::Left);
  PACMAN->SetSpriteY(0);
  PACMAN->SetFramesPerState(2);
  PACMAN->SetOneCycle(false);
  PACMAN->SetColumnsAndRowsOfAssociatedSpriteSheet(12, 2);
  PACMAN->SetSpriteScaleFactor(Global::pacManSize);
  PACMAN->Restart();

  //ゲームの初期設定
  m_gamePaused = true;
  m_pacmanMovementRequest = Character::Movement::Stop;
  m_currentPhaseIndex = 1;
  m_previousPhaseIndex = 1;
  m_frightenedTransition = false;
  m_currentGhostCounter = Ghosts::Pinky;
  m_timer.ResetElapsedTime();
}

bool Game::AreMovementsOppositeOrSame(Character::Movement m1, Character::Movement m2)
{
  //2つの移動方向が同じか反対かを判定
  if (m1 == m2) return true;

  if (m1 == Character::Movement::Up && m2 == Character::Movement::Down) return true;
  if (m1 == Character::Movement::Right && m2 == Character::Movement::Left) return true;
  if (m1 == Character::Movement::Down && m2 == Character::Movement::Up) return true;
  if (m1 == Character::Movement::Left && m2 == Character::Movement::Right) return true;

  return false;
}

float Game::DistanceBetweenCharacters(const Character& ch1, const Character& ch2)
{
  //2つのキャラクターの位置を取得
  const XMFLOAT3& pos1 = ch1.GetPosition();
  const XMFLOAT3& pos2 = ch2.GetPosition();

  //2つのキャラクター間の距離を計算して返す
  return sqrt((pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.z - pos2.z) * (pos1.z - pos2.z));
}

void Game::UpdatePositionOfBlinky()
{
  //ゴーストが死んでいる場合,特定の位置に移動
  if (BLINKY->IsDead())
  {
    MoveGhostTowardsPosition(10.5f, 11.5f, Ghosts::Blinky);
    return;
  }
  //ゴーストがハウス内にいる場合,現在のフェーズのモードを設定
  if (BLINKY->GetMovement() == Character::Movement::InHouse)
    BLINKY->SetMode(CURRENT_PHASE.mode);

  //ゴーストのモードに応じて動作を変更
  switch (BLINKY->GetMode())
  {
  case Global::Mode::Scatter:
	//Scatterモードでは特定の位置に移動
    MoveGhostTowardsPosition(18.5f, 21.5f, Ghosts::Blinky);
    break;
  case Global::Mode::Chase:
  {
	//Chaseモードではパックマンの位置に移動
    const DirectX::XMFLOAT3& pacmanPos = PACMAN->GetPosition();
    MoveGhostTowardsPosition(pacmanPos.x, pacmanPos.z, Ghosts::Blinky);
  }
  break;
  case Global::Mode::Frightened:
	//Frightenedモードではランダムな位置に移動
    MoveGhostTowardsRandomPosition(Ghosts::Blinky);
    break;
  }
}

void Game::UpdatePositionOfPinky()
{
  //ゴーストが停止している場合,何もしない
  if (PINKY->GetMovement() == Character::Movement::Stop)
    return;
  //ゴーストが死んでいる場合,特定の位置に移動
  if (PINKY->IsDead())
  {
    MoveGhostTowardsPosition(10.5f, 11.5f, Ghosts::Pinky);
    return;
  }
  //ゴーストがハウス内にいる場合,現在のフェーズのモードを設定
  if (PINKY->GetMovement() == Character::Movement::InHouse)
    PINKY->SetMode(CURRENT_PHASE.mode);

  switch (PINKY->GetMode())
  {
  case Global::Mode::Scatter:
	//特定の位置に移動
    MoveGhostTowardsPosition(2.5f, 21.5f, Ghosts::Pinky);
    break;
  case Global::Mode::Chase:
  {
	//パックマンの位置に基づいて移動
    DirectX::XMFLOAT3 pacmanPos = PACMAN->GetPosition();

	//パックマンの向きに応じて位置を調整
    switch (PACMAN->GetFacingDirection())
    {
    case Character::Direction::Left:
      pacmanPos.x -= 4;
      break;
    case Character::Direction::Right:
      pacmanPos.x += 4;
      break;
    case Character::Direction::Up:
      pacmanPos.z += 4;
      break;
    case Character::Direction::Down:
      pacmanPos.z -= 4;
      break;
    }

    MoveGhostTowardsPosition(pacmanPos.x, pacmanPos.z, Ghosts::Pinky);
  }
  break;
  case Global::Mode::Frightened:
	//ランダムな位置に移動
    MoveGhostTowardsRandomPosition(Ghosts::Pinky);
    break;
  }
}

void Game::UpdatePositionOfInky()
{
  //ゴーストが停止している場合,何もしない
  if (INKY->GetMovement() == Character::Movement::Stop)
    return;

  if (INKY->IsDead())
  {
	//ゴーストが死んでいる場合,特定の位置に移動
    MoveGhostTowardsPosition(10.5f, 11.5f, Ghosts::Inky);
    return;
  }
  //ゴーストがハウス内にいる場合,現在のフェーズのモードを設定
  if (INKY->GetMovement() == Character::Movement::InHouse)
    INKY->SetMode(CURRENT_PHASE.mode);

  //ゴーストのモードに応じて動作を変更
  switch (INKY->GetMode())
  {
  case Global::Mode::Scatter:
	//特定の位置に移動
    MoveGhostTowardsPosition(21.5f, 0.0f, Ghosts::Inky);
    break;
  case Global::Mode::Chase:
  {
	//パックマンの位置に基づいて移動
    DirectX::XMFLOAT3 pacmanPos = PACMAN->GetPosition();

	//パックマンの向きに応じて位置を調整
    switch (PACMAN->GetFacingDirection())
    {
    case Character::Direction::Left:
      pacmanPos.x -= 2;
      break;
    case Character::Direction::Right:
      pacmanPos.x += 2;
      break;
    case Character::Direction::Up:
      pacmanPos.z += 2;
      break;
    case Character::Direction::Down:
      pacmanPos.z -= 2;
      break;
    }
	//ブリンキーの位置を取得
    const DirectX::XMFLOAT3 blinkyPos = BLINKY->GetPosition();

    float finalPosX = 0;
    float finalPosZ = 0;

	//ブリンキーの位置を基に、最終的な移動位置を計算
    finalPosX = pacmanPos.x + (pacmanPos.x - blinkyPos.x);
    finalPosZ = pacmanPos.z + (pacmanPos.z - blinkyPos.z);

    MoveGhostTowardsPosition(finalPosX, finalPosZ, Ghosts::Inky);
  }
  break;
  case Global::Mode::Frightened:
	//ランダムな位置に移動
    MoveGhostTowardsRandomPosition(Ghosts::Inky);
    break;
  }
}

void Game::UpdatePositionOfClyde()
{
  //ゴーストが停止している場合,何もしない
  if (CLYDE->GetMovement() == Character::Movement::Stop)
    return;

  //ゴーストが死んでいる場合,特定の位置に移動
  if (CLYDE->IsDead())
  {
    MoveGhostTowardsPosition(10.5f, 11.5f, Ghosts::Clyde);
    return;
  }
  //ゴーストがハウス内にいる場合,現在のフェーズのモードを設定
  if (CLYDE->GetMovement() == Character::Movement::InHouse)
    CLYDE->SetMode(CURRENT_PHASE.mode);

  //ゴーストのモードに応じて動作を変更
  switch (CLYDE->GetMode())
  {
  case Global::Mode::Scatter:
	//特定の位置に移動
    MoveGhostTowardsPosition(0.0f, 0.0f, Ghosts::Clyde);
    break;
  case Global::Mode::Chase:
  {
	//パックマンの位置に基づいて移動
    const DirectX::XMFLOAT3& pacmanPos = PACMAN->GetPosition();
    const float distance = DistanceBetweenCharacters(*PACMAN, *CLYDE);

    if (distance > 8)
    {
	  // 距離が8以上の場合,ブリンキーのように振る舞う
      MoveGhostTowardsPosition(pacmanPos.x, pacmanPos.z, Ghosts::Clyde);
    }
    else
    {
	  //距離が8未満の場合,Scatterモードとして振る舞う
      MoveGhostTowardsPosition(0.0f, 0.0f, Ghosts::Clyde);
    }
  }
  break;
  case Global::Mode::Frightened:
	//ランダムな位置に移動
    MoveGhostTowardsRandomPosition(Ghosts::Clyde);
    break;
  }
}

//デバイスに依存するリソースを作成
void Game::CreateDevice()
{
  UINT creationFlags = 0;

#ifdef _DEBUG
  //デバッグフラグを設定
  creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  static const D3D_FEATURE_LEVEL featureLevels[] =
  {
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
    D3D_FEATURE_LEVEL_9_3,
    D3D_FEATURE_LEVEL_9_2,
    D3D_FEATURE_LEVEL_9_1,
  };

  //DX11APIデバイスオブジェクトを作成し,対応するコンテキストを取得
  ComPtr<ID3D11Device> device;
  ComPtr<ID3D11DeviceContext> context;
  DX::ThrowIfFailed(D3D11CreateDevice(
    nullptr,                            //デフォルトアダプタを使用するためにnullptrを指定
    D3D_DRIVER_TYPE_HARDWARE,
    nullptr,
    creationFlags,
    featureLevels,
    _countof(featureLevels),
    D3D11_SDK_VERSION,
    device.ReleaseAndGetAddressOf(),    //作成されたDirect3Dデバイスを返す
    &m_featureLevel,                    //作成されたデバイスの機能レベルを返す
    context.ReleaseAndGetAddressOf()    //デバイスの即時コンテキストを返す
  ));

#ifndef NDEBUG
  ComPtr<ID3D11Debug> d3dDebug;
  if (SUCCEEDED(device.As(&d3dDebug)))
  {
    ComPtr<ID3D11InfoQueue> d3dInfoQueue;
    if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
    {
#ifdef _DEBUG
      d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
      d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
      D3D11_MESSAGE_ID hide[] =
      {
          D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
          
      };
      D3D11_INFO_QUEUE_FILTER filter = {};
      filter.DenyList.NumIDs = _countof(hide);
      filter.DenyList.pIDList = hide;
      d3dInfoQueue->AddStorageFilterEntries(&filter);
    }
  }
#endif
  //デバイスとコンテキストを保存
  DX::ThrowIfFailed(device.As(&m_d3dDevice));
  DX::ThrowIfFailed(context.As(&m_d3dContext));
  //シェーダーマネージャを初期化
  m_shaderManager = std::make_unique<ShaderManager>(m_d3dDevice.Get(), m_d3dContext.Get());
}


//ウィンドウのサイズ変更イベントに依存するすべてのメモリリソースを割り当てる
void Game::CreateResources()
{
  //前のウィンドウサイズに依存するコンテキストをクリア
  ID3D11RenderTargetView* nullViews[] = { nullptr };
  m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
  m_renderTargetView.Reset();
  m_depthStencilView.Reset();
  m_d3dContext->Flush();

  UINT backBufferWidth = static_cast<UINT>(m_outputWidth);
  UINT backBufferHeight = static_cast<UINT>(m_outputHeight);
  DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
  DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
  UINT backBufferCount = 2;

  //スワップチェーンが既に存在する場合はリサイズし,存在しない場合は新しく作成
  if (m_swapChain)
  {
    HRESULT hr = m_swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);

    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
	  //デバイスが何らかの理由で削除された場合,新しいデバイスとスワップチェーンを作成する必要がある
      OnDeviceLost();

     //OnDeviceLostがこのメソッドを再入力し,新しいデバイスを正しくセットアップする
      return;
    }
    else
    {
      DX::ThrowIfFailed(hr);
    }
  }
  else
  {
	//最初に,D3Dデバイスから基になるDXGIデバイスを取得
    ComPtr<IDXGIDevice1> dxgiDevice;
    DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

	//このデバイスが動作している物理アダプタ(GPUまたはカード)を識別
    ComPtr<IDXGIAdapter> dxgiAdapter;
    DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

	//そして,それを作成したファクトリオブジェクトを取得
    ComPtr<IDXGIFactory2> dxgiFactory;
    DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

	//スワップチェーンのディスクリプタを作成
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = backBufferWidth;
    swapChainDesc.Height = backBufferHeight;
    swapChainDesc.Format = backBufferFormat;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = backBufferCount;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
    fsSwapChainDesc.Windowed = TRUE;

	//Win32ウィンドウからスワップチェーンを作成
    DX::ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
      m_d3dDevice.Get(),
      m_window,
      &swapChainDesc,
      &fsSwapChainDesc,
      nullptr,
      m_swapChain.ReleaseAndGetAddressOf()
    ));

	//DXGIがALT + ENTERショートカットに応答するのを防ぐ
    DX::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
  }

  //このウィンドウのバックバッファを取得し,最終的な3Dレンダーターゲットとして使用
  ComPtr<ID3D11Texture2D> backBuffer;
  DX::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

  //バックバッファ上にレンダーターゲットビューを作成し,バインドに使用
  DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf()));

  //深度/ステンシルバッファとして2Dサーフェスを割り当て,このサーフェス上に深度ステンシルビューを作成してバインドに使用
  CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);

  ComPtr<ID3D11Texture2D> depthStencil;
  DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

  CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
  DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf()));

  //ウィンドウサイズ依存のオブジェクトを初期化
  m_camera.SetProjectionValues(75.0f, static_cast<float>(m_outputWidth) / static_cast<float>(m_outputHeight), 0.1f, 1000.0f);
  m_camera.SetOrthographicValues(static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));

  XMFLOAT4X4 projection = m_camera.GetProjectionMatrix();
  //定数バッファの設定
  D3D11_BUFFER_DESC cbd = {};
  cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  cbd.Usage = D3D11_USAGE_DYNAMIC;
  cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  cbd.MiscFlags = 0;
  cbd.ByteWidth = sizeof(XMFLOAT4X4);
  cbd.StructureByteStride = 0;

  D3D11_SUBRESOURCE_DATA csd = {};
  csd.pSysMem = &projection;

  //プロジェクションマトリックス用の定数バッファを作成
  DX::ThrowIfFailed(m_d3dDevice->CreateBuffer(&cbd, &csd, &m_projectionMatrixConstantBuffer));

  //カメラのフレームごとの定数バッファを設定
  Global::CameraPerFrame cameraConstantBufferPerFrame = {};
  cameraConstantBufferPerFrame.view = m_camera.GetViewMatrix();
  cameraConstantBufferPerFrame.projection = m_camera.GetProjectionMatrix();

  cbd = {};
  cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  cbd.Usage = D3D11_USAGE_DYNAMIC;
  cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  cbd.MiscFlags = 0;
  cbd.ByteWidth = sizeof(Global::CameraPerFrame);
  cbd.StructureByteStride = 0;

  csd = {};
  csd.pSysMem = &cameraConstantBufferPerFrame;
  //フレームごとの定数バッファを作成
  DX::ThrowIfFailed(m_d3dDevice->CreateBuffer(&cbd, &csd, &m_cameraPerFrame));

  //オブジェクトごとのカメラの定数バッファを設定
  Global::CameraPerObject cameraConstantBufferPerObject;
  XMStoreFloat4x4(&cameraConstantBufferPerObject.world, DirectX::XMMatrixIdentity());

  cbd.ByteWidth = sizeof(Global::CameraPerObject);

  csd.pSysMem = &cameraConstantBufferPerObject;

  //オブジェクトごとの定数バッファを作成
  DX::ThrowIfFailed(m_d3dDevice->CreateBuffer(&cbd, &csd, &m_cameraPerObject));

  //フレーム定数バッファの設定
  Global::SpriteConstantBuffer frameConstantBuffer = {};

  cbd = {};
  cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  cbd.Usage = D3D11_USAGE_DYNAMIC;
  cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  cbd.MiscFlags = 0;
  cbd.ByteWidth = sizeof(Global::SpriteConstantBuffer);
  cbd.StructureByteStride = 0;

  csd = {};
  csd.pSysMem = &frameConstantBuffer;

  //フレーム定数バッファを作成
  DX::ThrowIfFailed(m_d3dDevice->CreateBuffer(&cbd, &csd, &m_frameBuffer));
  //定数バッファをジオメトリシェーダにバインド
  ID3D11Buffer* geometryShaderBuffers[2] = { m_projectionMatrixConstantBuffer.Get(), m_frameBuffer.Get() };
  m_shaderManager->BindConstantBuffersToGeometryShader(ShaderManager::GeometryShader::Billboard, geometryShaderBuffers, 2);
  //定数バッファを頂点シェーダにバインド
  ID3D11Buffer* vertexShaderBuffers[2] = { m_cameraPerFrame.Get(), m_cameraPerObject.Get() };
  m_shaderManager->BindConstantBuffersToVertexShader(ShaderManager::VertexShader::Instanced, vertexShaderBuffers, 2);
  m_shaderManager->BindConstantBuffersToVertexShader(ShaderManager::VertexShader::Indexed, vertexShaderBuffers, 2);
  //キャプションの位置を設定
  m_caption->SetPosition(static_cast<float>((m_outputWidth / 2) - (m_caption->GetWidth() / 2)), static_cast<float>(-m_caption->GetHeight() * 2), 0);
}

void Game::OnDeviceLost()
{
  //デバイスが失われた場合のリソースのリセット
  m_depthStencilView.Reset();
  m_renderTargetView.Reset();
  m_swapChain.Reset();
  m_d3dContext.Reset();
  m_d3dDevice.Reset();
  //デバイスを再作成
  CreateDevice();
  //リソースを再作成
  CreateResources();
}

void Game::SetSpriteConstantBufferForCharacter(Global::SpriteConstantBuffer& spriteConstantBuffer, const Character& character)
{
  //キャラクターのスプライト定数バッファを設定
  spriteConstantBuffer.spriteX = character.GetSpriteX();
  spriteConstantBuffer.spriteY = character.GetSpriteY();
  spriteConstantBuffer.spriteSheetColumns = character.GetSpriteSheetColumns();
  spriteConstantBuffer.spriteSheetRows = character.GetSpriteSheetRows();
  spriteConstantBuffer.billboardSize_0_0_0 = DirectX::XMFLOAT4(character.GetSpriteScaleFactor(), 0, 0, 0);
}
