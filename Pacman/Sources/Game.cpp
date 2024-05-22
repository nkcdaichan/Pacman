#include "pch.h"

#include <cmath>

#include "Game.h"
#include "WICTextureLoader.h"

extern void ExitGame();           //�O���Œ�`����Ă���Q�[���I���֐���錾

using namespace DirectX;

using Microsoft::WRL::ComPtr;

//�}�N����`
#define CURRENT_PHASE m_phasesLevel1[m_currentPhaseIndex]            //���݂̃t�F�[�Y���擾����}�N��

#define BLINKY m_ghosts[static_cast<uint8_t>(Game::Ghosts::Blinky)]  //Blinky���擾����}�N��
#define PINKY m_ghosts[static_cast<uint8_t>(Game::Ghosts::Pinky)]    //Pinky���擾����}�N��
#define INKY m_ghosts[static_cast<uint8_t>(Game::Ghosts::Inky)]      //Inky���擾����}�N��
#define CLYDE m_ghosts[static_cast<uint8_t>(Game::Ghosts::Clyde)]    //Clyde���擾����}�N��
#define PACMAN m_pacman                                              //Pacman���擾����}�N��

//�R���X�g���N�^
Game::Game() noexcept :
  m_window(nullptr),                                                 //�E�B���h�E�n���h���̏�����
  m_outputWidth(800),                                                //�o�͕��̏�����
  m_outputHeight(600),                                               //�o�͍����̏�����
  m_featureLevel(D3D_FEATURE_LEVEL_9_1),                             //DirectX�̋@�\���x���̏�����
  m_pacmanMovementRequest(Character::Movement::Stop),                //Pacman�̏����ړ��v�����~�ɐݒ�
  m_debugDraw(false),                                                //�f�o�b�O�`��̃t���O��������
  m_currentPhaseIndex(1),                                            //���݂̃t�F�[�Y�C���f�b�N�X��������
  m_previousPhaseIndex(1),                                           //�O�̃t�F�[�Y�C���f�b�N�X��������
  m_frightenedTransition(false),                                     //���|���[�h�J�ڂ̃t���O��������
  m_currentGhostCounter(Ghosts::Pinky),                              //���݂̃S�[�X�g�J�E���^�[��������
  m_gameState(State::Intro),                                         //�Q�[����Ԃ��C���g���ɐݒ�
  m_gamePaused(true)                                                 //�Q�[���̈ꎞ��~�t���O��������
{
  CreatePhases();                                                    //�t�F�[�Y���쐬
}
//�Q�[���̏�����
void Game::Initialize(HWND window, uint16_t width, uint16_t height)
{
  assert(width > 1 && height > 1);                        //���ƍ�����1���傫�����Ƃ��m�F

  m_window = window;                                      //�E�B���h�E�n���h����ݒ�
  m_outputWidth = width;                                  //�o�͕���ݒ�
  m_outputHeight = height;                                //�o�͍�����ݒ�

  CreateDevice();                                         //�f�o�C�X���쐬

  m_caption = std::make_unique<Caption>();                //�L���v�V�������쐬
  m_caption->Init(m_d3dDevice.Get(), 400, 100);           //�L���v�V������������

  CreateResources();                                      //���\�[�X���쐬

  m_world.Init(m_d3dDevice.Get());                        //���[���h��������

  for (auto& ghost : m_ghosts)                            //�S�[�X�g�z���������
  {
    ghost = std::make_unique<Ghost>();                    //�S�[�X�g���쐬
    ghost->Init(m_d3dDevice.Get());                       //�S�[�X�g��������
  }

  PACMAN = std::make_unique<Character>();                 //Pacman���쐬
  PACMAN->Init(m_d3dDevice.Get());                        //Pacman��������

  //Pacman�̃e�N�X�`����ǂݍ���
  DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), nullptr, L"Resources/pacman.png", m_pacManResource.GetAddressOf(), m_pacManShaderResourceView.GetAddressOf()));
  //�S�[�X�g�̃e�N�X�`����ǂݍ���
  DX::ThrowIfFailed(CreateWICTextureFromFile(m_d3dDevice.Get(), nullptr, L"Resources/ghosts.png", m_ghostsResource.GetAddressOf(), m_ghostsShaderResourceView.GetAddressOf()));

  //�L�[�{�[�h���쐬
  m_keyboard = std::make_unique<Keyboard>();              

  //���C�g�萔�o�b�t�@���쐬
  Global::LightConstantBuffer lightCB;  
  //���C�g�̒l��ݒ�
  lightCB.values = XMFLOAT4(10.5f, 2.5f, 11.5f, 1.0f);

  //�萔�o�b�t�@�̃f�B�X�N���v�^��ݒ�
  D3D11_BUFFER_DESC cbd = {};
  cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  cbd.Usage = D3D11_USAGE_DYNAMIC;
  cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  cbd.MiscFlags = 0;
  cbd.ByteWidth = sizeof(Global::LightConstantBuffer);
  cbd.StructureByteStride = 0;

  //�T�u���\�[�X�f�[�^��ݒ�
  D3D11_SUBRESOURCE_DATA csd = {};
  csd.pSysMem = &lightCB;

  //���C�g�o�b�t�@���쐬
  DX::ThrowIfFailed(m_d3dDevice->CreateBuffer(&cbd, &csd, &m_light));

  //�s�N�Z���V�F�[�_�ɒ萔�o�b�t�@���o�C���h
  m_shaderManager->BindConstantBuffersToPixelShader(ShaderManager::PixelShader::Phong, m_light.GetAddressOf(), 1);

  //�Œ�^�C���X�e�b�v��ݒ�
  m_timer.SetFixedTimeStep(true);
  //�ڕW�o�ߕb��ݒ�(60fps�j
  m_timer.SetTargetElapsedSeconds(1.f / 60.f);
}

//�Q�[���̃e�B�b�N����
void Game::Tick()
{
  //�^�C�}�[���e�B�b�N���A�A�b�v�f�[�g�֐����Ăяo��
  m_timer.Tick([&]()
  {
    Update(m_timer);
  });

  //�����_�����O
  Render();
}

//�Q�[���̍X�V����
void Game::Update(const DX::StepTimer& timer)
{

  //�L�[�{�[�h�̏�Ԃ��擾
  const auto& kb = m_keyboard->GetState();

  //�G�X�P�[�v�L�[�������ꂽ��Q�[���I��
  if (kb.Escape)
    ExitGame();
  //�X�y�[�X�L�[�������ꂽ��...
  if (kb.Space)
  {
	//�Q�[����Ԃ��C���g���Ȃ�X�^�[�g�ɕύX
    if (m_gameState == Game::State::Intro)
      m_gameState = Game::State::Start;

	//�Q�[����Ԃ����x���Ȃ�
    if (m_gameState == Game::State::Level)
    {
	  //�Q�[�����ĊJ
      m_gamePaused = false;
	  //Pacman�����Ɉړ�
      m_pacmanMovementRequest = Character::Movement::Left;
    }
  }

  //Pacman������ł��ăA�j���[�V�������I�����Ă���ꍇ
  if (PACMAN->IsDead() && PACMAN->IsAnimationDone() && m_gameState == Game::State::Level)
	//�Q�[����Ԃ��f�b�h�ɕύX
    m_gameState = Game::State::Dead;

  //�Q�[����Ԃɂ�鏈��
  switch (m_gameState)
  {
  //�C���g�����
  case Game::State::Intro:

	//�L���v�V������Y���W�𒲐�
    m_caption->AdjustY(5, 80);
	//�J�����̃��[�v�����Z�b�g
    m_camera.ResetLerp();
    break;
  //�X�^�[�g���
  case Game::State::Start:
	//�J���������[�v
    m_camera.LerpBetweenCameraPositions(0.04f);

	//�J�����̃��[�v������������
    if (m_camera.IsCameraLerpDone())
    {
	  //�V�����Q�[����������
      NewGameInitialization();
	  //�Q�[����Ԃ����x���ɕύX
      m_gameState = Game::State::Level;
	  //���݂̃t�F�[�Y�̊J�n���Ԃ�ݒ�
      CURRENT_PHASE.startingTime = timer.GetTotalSeconds();
    }
    break;
  //���x�����
  case Game::State::Level:
	//�J�����̃��[�v�����Z�b�g
    m_camera.ResetLerp();
    break;
  //�f�b�h���
  case Game::State::Dead:
	//�J�������t���[�v
    m_camera.InverseLerpBetweenCameraPositions(0.04f);

	// �J�����̃��[�v������������Q�[����Ԃ��C���g���ɖ߂�
    if (m_camera.IsCameraLerpDone())
      m_gameState = Game::State::Intro;
    break;
  }

  //�Q�[�����J�n����Ă��Ȃ��ꍇ��,�ꎞ��~���̏ꍇ�̓V�~�����[�V�������X�L�b�v��,�t�F�[�Y�̃^�C�}�[��i�߂Ȃ�
  if (m_gameState != Game::State::Level || m_gamePaused)
  {
	//�t�F�[�Y�̊J�n���Ԃ��X�V
    CURRENT_PHASE.startingTime = timer.GetTotalSeconds();
	//�����𒆒f
    return;
  }

  //�t���[�����X�V
  //Pacman�̃t���[�����X�V
  PACMAN->UpdateFrame(m_timer.GetElapsedSeconds());  
  //�S�ẴS�[�X�g�̃t���[�����X�V
  std::for_each(m_ghosts.begin(), m_ghosts.end(), [&](auto& ghost) { ghost->UpdateFrame(m_timer.GetElapsedSeconds()); });

  //�t�F�[�Y�̊��Ԃ��o�߂�,�����݂̃t�F�[�Y�C���f�b�N�X���ő�l�����̏ꍇ
  if (timer.GetTotalSeconds() >= (CURRENT_PHASE.startingTime + CURRENT_PHASE.duration) && m_currentPhaseIndex < Global::phasesNum - 1)
  {
	//�t�F�[�Y�C���f�b�N�X��0�̏ꍇ
    if (m_currentPhaseIndex == 0)
    {
	  //�O�̃t�F�[�Y�C���f�b�N�X�ɖ߂�
      m_currentPhaseIndex = m_previousPhaseIndex;
	  //�S�[�X�g�̃X�v���C�g���f�t�H���g�ɐݒ�
      SetGhostsDefaultSprites();
    }
    else
    {
	  //�t�F�[�Y�C���f�b�N�X���C���N�������g
      m_currentPhaseIndex++;
	  //�S�ẴS�[�X�g�̈ړ������𔽓]
      std::for_each(m_ghosts.begin(), m_ghosts.end(), [](auto& ghost) { ghost->ReverseMovementDirection(); });
    }
	//�t�F�[�Y�̊J�n���Ԃ��X�V
    CURRENT_PHASE.startingTime = timer.GetTotalSeconds();

	//���݂̃��[�h���S�[�X�g�ɋ����I�ɓK�p
    std::for_each(m_ghosts.begin(), m_ghosts.end(), [&](auto& ghost)
    {
	  //�S�[�X�g�������Ă���ꍇ
      if (ghost->IsAlive())
		//�S�[�X�g�̃��[�h��ݒ�
        ghost->SetMode(CURRENT_PHASE.mode);
    });

	//���|���[�h�J�ڃt���O�����Z�b�g
    m_frightenedTransition = false;
  }
  else if (CURRENT_PHASE.mode == Global::Mode::Frightened && (timer.GetTotalSeconds() >= (CURRENT_PHASE.startingTime + CURRENT_PHASE.duration / 2.0)) && !m_frightenedTransition)
  {
    std::for_each(m_ghosts.begin(), m_ghosts.end(), [](auto& ghost)
    {
	  //�S�[�X�g�����|���[�h�Ő����Ă���ꍇ
      if (ghost->GetMode() == Global::Mode::Frightened && ghost->IsAlive())
		//�S�[�X�g�̃X�v���C�gY���W��ύX
        ghost->SetSpriteY(Global::rowTransition);
    });
	//���|���[�h�J�ڃt���O��ݒ�
    m_frightenedTransition = true;
  }
  //���݂̃S�[�X�g�J�E���^�[�ɉ�����������
  switch (m_currentGhostCounter)
  {
  case Ghosts::Pinky:
	//���̃S�[�X�g�ɃJ�E���^�[��i�߂�
    m_currentGhostCounter = Ghosts::Inky;
	//Pinky���Ƃ̒��Ɉړ�
    PINKY->SetMovement(Character::Movement::InHouse);
    break;
  //Inky���Ƃ��o�鏀�����ł����ꍇ
  case Ghosts::Inky:
    if (INKY->ReadyToLeaveHouse())
    {
	  //���̃S�[�X�g�ɃJ�E���^�[��i�߂�
      m_currentGhostCounter = Ghosts::Clyde;
	  //Inky���Ƃ̒��Ɉړ�
      INKY->SetMovement(Character::Movement::InHouse);
    }
    break;
  //Clyde���Ƃ��o�鏀�����ł����ꍇ
  case Ghosts::Clyde:
    if (CLYDE->ReadyToLeaveHouse())
    {
	  //�S�[�X�g�J�E���^�[�����Z�b�g
      m_currentGhostCounter = Ghosts::None;
	  //Clyde���Ƃ̒��Ɉړ�
      CLYDE->SetMovement(Character::Movement::InHouse);
    }
    break;
  }
  //Pacman�̌��݂̈ړ��������擾
  const Character::Movement pacmanMovement = PACMAN->GetMovement();
  //Pacman�̌��݂̈ʒu���擾
  const DirectX::XMFLOAT3& pacmanPosCurrent = PACMAN->GetPosition();

  //�e���|�[�g����(���Α�����o�Ă��鏈��)
  if (pacmanMovement == Character::Movement::Left && (pacmanPosCurrent.x - 0.5f) < Global::pacManSpeed)
  {
	//Pacman���E�[�Ƀe���|�[�g
    PACMAN->SetPosition(20.5f, pacmanPosCurrent.y, pacmanPosCurrent.z);
	//�����𒆒f
    return;
  }
  else if (pacmanMovement == Character::Movement::Right && (20.5f - pacmanPosCurrent.x) < Global::pacManSpeed)
  {
	//Pacman�����[�Ƀe���|�[�g
    PACMAN->SetPosition(0.5f, pacmanPosCurrent.y, pacmanPosCurrent.z);
	//�����𒆒f
    return;
  }

  //Pacman������ł���ꍇ�̏���
  if (PACMAN->IsDead())
	//�����𒆒f
    return;

  //��������ѐ����ʒu���킹�̔���
  bool isHorizontallyAligned = (fmod(pacmanPosCurrent.x - 0.5f, 1.0f) < Global::pacManSpeed);      //���������Ɉʒu���킹����Ă��邩
  bool isVerticallyAligned = (fmod(pacmanPosCurrent.z - 0.5f, 1.0f) < Global::pacManSpeed);        //���������Ɉʒu���킹����Ă��邩

  bool moves[Character::Direction::_Count] = { false, false, false, false };                       //�e�����ւ̈ړ��\����������

  //�e�����̈ړ��\���𔻒�
  moves[Character::Direction::Up] = m_world.IsPassable(static_cast<uint8_t>(pacmanPosCurrent.x), static_cast<uint8_t>(pacmanPosCurrent.z + 1.0f), false);
  moves[Character::Direction::Right] = m_world.IsPassable(static_cast<uint8_t>(pacmanPosCurrent.x + 1.0f), static_cast<uint8_t>(pacmanPosCurrent.z), false);
  moves[Character::Direction::Down] = m_world.IsPassable(static_cast<uint8_t>(pacmanPosCurrent.x), static_cast<uint8_t>(pacmanPosCurrent.z - 1.0f), false);
  moves[Character::Direction::Left] = m_world.IsPassable(static_cast<uint8_t>(pacmanPosCurrent.x - 1.0f), static_cast<uint8_t>(pacmanPosCurrent.z), false);

  //�L�[�{�[�h���͂ɉ�����Pacman�̈ړ��v����ݒ�
  if (isVerticallyAligned)
  {
    if (kb.Right && moves[Character::Direction::Right])
	  //�E�ړ��v����ݒ�
      m_pacmanMovementRequest = Character::Movement::Right;
    else if (kb.Left && moves[Character::Direction::Left])
	  //���ړ��v����ݒ�
      m_pacmanMovementRequest = Character::Movement::Left;
  }

  if (isHorizontallyAligned)
  {
    if (kb.Up && moves[Character::Direction::Up])
	  //��ړ��v����ݒ�
      m_pacmanMovementRequest = Character::Movement::Up;
    else if (kb.Down && moves[Character::Direction::Down])
	  //���ړ��v����ݒ�
      m_pacmanMovementRequest = Character::Movement::Down;
  }
  //Pacman�̈ړ�����
  auto pacmanHandleMovement = [&](bool isPassable, bool alignment)
  {
	//�ړ��\�ȏꍇ
    if (isPassable)
    {
	  //�ړ����������΂⓯���łȂ��ꍇ
      if (!AreMovementsOppositeOrSame(m_pacmanMovementRequest, pacmanMovement))
		//Pacman���}�b�v�Ɉʒu���킹
        PACMAN->AlignToMap();
	  //�ړ��v�������݂̈ړ������ƈقȂ�ꍇ
      if (m_pacmanMovementRequest != pacmanMovement)
		//�ړ�������ݒ�
        PACMAN->SetMovement(m_pacmanMovementRequest);
    }
	//�ʒu���킹���K�v�ȏꍇ
    else if (alignment)
    {
	  //�ړ��v�������݂̈ړ������Ɠ����ꍇ
      if (m_pacmanMovementRequest == pacmanMovement)
      {
		//Pacman���}�b�v�Ɉʒu���킹
        PACMAN->AlignToMap();
		//�ړ����~
        PACMAN->SetMovement(Character::Movement::Stop);
      }
      else
		//�ړ��v�������݂̈ړ������ɐݒ�
        m_pacmanMovementRequest = pacmanMovement;
    }
  };
  //Pacman�̈ړ��v���ɉ���������
  switch (m_pacmanMovementRequest)
  {
  case Character::Movement::Right:
	//�E�ړ�����
    pacmanHandleMovement(moves[Character::Direction::Right], isHorizontallyAligned);
    break;
  case Character::Movement::Left:
	//���ړ�����
    pacmanHandleMovement(moves[Character::Direction::Left], isHorizontallyAligned);
    break;
  case Character::Movement::Up:
	//��ړ�����
    pacmanHandleMovement(moves[Character::Direction::Up], isVerticallyAligned);
    break;
  case Character::Movement::Down:
	//���ړ�����
    pacmanHandleMovement(moves[Character::Direction::Down], isVerticallyAligned);
    break;
  default:
	//�������Ȃ�
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

  //�H�ׂ��h�b�g�̎�ނ��擾
  Dots::Type dotEaten;
  m_dots->Update(static_cast<uint8_t>(pacmanPosCurrent.x), static_cast<uint8_t>(pacmanPosCurrent.z), m_d3dContext.Get(), dotEaten);

  //�Ō�̃h�b�g��H�ׂ��ꍇ,�V�����Q�[������������,�Q�[����Ԃ����x���ɐݒ�
  if (dotEaten == Dots::Type::LastOne)
  {
    NewGameInitialization();
    m_gameState = Game::State::Level;
  }
  //�h�b�g��H�ׂ��ꍇ,���S�[�X�g�J�E���^�[�����Z�b�g����Ă��Ȃ��ꍇ,�S�[�X�g�̐H�ׂ��h�b�g�����C���N�������g
  if (dotEaten != Dots::Type::Nothing && m_currentGhostCounter != Ghosts::None)
    m_ghosts[static_cast<uint8_t>(m_currentGhostCounter)]->IncrementEatenDots();
  //����h�b�g��H�ׂ��ꍇ
  if (dotEaten == Dots::Type::Extra)
  {
    m_previousPhaseIndex = m_currentPhaseIndex == 0 ? m_previousPhaseIndex : m_currentPhaseIndex;    //�t�F�[�Y�C���f�b�N�X�̍X�V
	m_frightenedTransition = false;
    m_frightenedTransition = false;
    m_currentPhaseIndex = 0;                                                                         //���|���[�h������

    CURRENT_PHASE.startingTime = timer.GetTotalSeconds();                                            //�t�F�[�Y�̊J�n���Ԃ��X�V

	//�S�ẴS�[�X�g�ɋ��|���[�h��K�p
    std::for_each(m_ghosts.begin(), m_ghosts.end(), [](auto& ghost)
    {
      if (ghost->IsAlive())
      {
        ghost->SetMode(Global::Mode::Frightened);     //�S�[�X�g�̃��[�h�����|�ɐݒ�
        ghost->SetSpriteY(Global::rowFrightened);     //�S�[�X�g�̃X�v���C�gY���W�����|���[�h�ɕύX
        ghost->ReverseMovementDirection();            //�S�[�X�g�̈ړ������𔽓]
      }
    });
  }

  //�e�S�[�X�g�̈ʒu���X�V
  UpdatePositionOfBlinky();
  UpdatePositionOfPinky();
  UpdatePositionOfInky();
  UpdatePositionOfClyde();

  //�Փˏ������n���h�����O
  HandleCollisions();
}

void Game::Render()
{
  //�ŏ��̍X�V�O�ɉ��������_�����O���Ȃ�
  if (m_timer.GetFrameCount() == 0)
    return;

  Clear();
  //�Q�[���̏�Ԃɉ����ĕ`��
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

//�o�b�N�o�b�t�@���N���A����w���p�[���\�b�h
void Game::Clear()
{
  //�r���[���N���A
  m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::Black);
  m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

  m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

  //�r���[�|�[�g��ݒ�
  CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));
  m_d3dContext->RSSetViewports(1, &viewport);

  m_shaderManager->ClearShadersFromThePipeline();
}

//�o�b�N�o�b�t�@�̓��e����ʂɕ\��.
void Game::Present()
{
   //��������VSync�܂Ńu���b�N����悤��DXGI�Ɏw����,
   //����VSync�܂ŃA�v���P�[�V�������X���[�v������B
   //����ɂ��,�\������Ȃ��t���[���������_�����O���閳�ʂ�h���B
  HRESULT hr = m_swapChain->Present(1, 0);

  //�f�o�C�X�����Z�b�g���ꂽ�ꍇ,�����_���[�����S�ɍď���������K�v������B
  if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    OnDeviceLost();
  else
    DX::ThrowIfFailed(hr);
}

//���b�Z�[�W�n���h��
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
  //���W���[�����Ɍo�ߎ��Ԃ����Z�b�g
  m_timer.ResetElapsedTime();
}

void Game::OnWindowSizeChanged(uint16_t width, uint16_t height)
{
  assert(width > 1 && height > 1);

  m_outputWidth = width;
  m_outputHeight = height;

  //���\�[�X���č쐬
  CreateResources();
}

void Game::GetDefaultSize(uint16_t& width, uint16_t& height) const
{
  width = 800;
  height = 600;
}
//�Q�[�����[���h��`��
void Game::DrawWorld()
{
  m_shaderManager->SetVertexShader(ShaderManager::VertexShader::Indexed);
  m_shaderManager->SetPixelShader(ShaderManager::PixelShader::Phong);

  Global::CameraPerObject cameraPerObjectConstantBuffer;
  cameraPerObjectConstantBuffer.world = m_world.GetWorldMatrix();

  m_shaderManager->UpdateConstantBuffer(m_cameraPerObject.Get(), &cameraPerObjectConstantBuffer, sizeof(cameraPerObjectConstantBuffer));

  //�J����
  Global::CameraPerFrame cameraConstantBufferPerFrame = {};
  cameraConstantBufferPerFrame.view = m_camera.GetViewMatrix();
  cameraConstantBufferPerFrame.projection = m_camera.GetProjectionMatrix();

  m_shaderManager->UpdateConstantBuffer(m_cameraPerFrame.Get(), &cameraConstantBufferPerFrame, sizeof(cameraConstantBufferPerFrame));

  m_world.Draw(m_d3dContext.Get());
}
//�C���g����ʂ�`��
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
  //�h�b�g��`��
  m_shaderManager->SetVertexShader(ShaderManager::VertexShader::Instanced);        //�C���X�^���X�����ꂽ���_�V�F�[�_�[��ݒ�
  m_shaderManager->SetGeometryShader(ShaderManager::GeometryShader::Billboard);    //�r���{�[�h�̃W�I���g���V�F�[�_�[��ݒ�
  m_shaderManager->SetPixelShader(ShaderManager::PixelShader::Texture);            //�e�N�X�`���̃s�N�Z���V�F�[�_�[��ݒ�

  //�X�v���C�g�̒萔�o�b�t�@�̐ݒ�
  Global::SpriteConstantBuffer spriteConstantBuffer = { 0, 0, 1, 1, DirectX::XMFLOAT4(0.2f, 0.4f, 0, 0) };
  //�t���[���̒萔�o�b�t�@���X�V
  m_shaderManager->UpdateConstantBuffer(m_frameBuffer.Get(), &spriteConstantBuffer, sizeof(spriteConstantBuffer));

  //�J�����̒萔�o�b�t�@��������
  Global::CameraPerObject cameraPerObjectConstantBuffer = {};
  //�J�����̒萔�o�b�t�@��������
  cameraPerObjectConstantBuffer.world = m_dots->GetWorldMatrix();
  //�J�����̒萔�o�b�t�@���X�V
  m_shaderManager->UpdateConstantBuffer(m_cameraPerObject.Get(), &cameraPerObjectConstantBuffer, sizeof(cameraPerObjectConstantBuffer));
  //�h�b�g��`��
  m_dots->Draw(m_d3dContext.Get());

  //�p�b�N�}����`��
  m_d3dContext->PSSetShaderResources(0, 1, m_pacManShaderResourceView.GetAddressOf());

  //�p�b�N�}���̃X�v���C�g�萔�o�b�t�@��ݒ�
  SetSpriteConstantBufferForCharacter(spriteConstantBuffer, *PACMAN);
  //�t���[���̒萔�o�b�t�@���X�V
  m_shaderManager->UpdateConstantBuffer(m_frameBuffer.Get(), &spriteConstantBuffer, sizeof(spriteConstantBuffer));

  //�p�b�N�}���̃��[���h�s���ݒ�
  cameraPerObjectConstantBuffer.world = PACMAN->GetWorldMatrix();

  //�J�����̒萔�o�b�t�@���X�V
  m_shaderManager->UpdateConstantBuffer(m_cameraPerObject.Get(), &cameraPerObjectConstantBuffer, sizeof(cameraPerObjectConstantBuffer));

  //�p�b�N�}����`��
  PACMAN->Draw(m_d3dContext.Get());

  //�S�[�X�g��`��
  //�p�b�N�}���������Ă���ꍇ�̂ݕ`��
  if (PACMAN->IsAlive())
  {
	//�S�[�X�g�̃V�F�[�_�[���\�[�X�r���[��ݒ�
    m_d3dContext->PSSetShaderResources(0, 1, m_ghostsShaderResourceView.GetAddressOf());

    std::for_each(m_ghosts.begin(), m_ghosts.end(), [&](auto& ghost)
    {
	  //�S�[�X�g�̃X�v���C�g�萔�o�b�t�@��ݒ�
      SetSpriteConstantBufferForCharacter(spriteConstantBuffer, *ghost);
	  //�t���[���̒萔�o�b�t�@���X�V
      m_shaderManager->UpdateConstantBuffer(m_frameBuffer.Get(), &spriteConstantBuffer, sizeof(spriteConstantBuffer));

	  //�S�[�X�g�̃��[���h�s���ݒ�
      cameraPerObjectConstantBuffer.world = ghost->GetWorldMatrix();
	  //�J�����̒萔�o�b�t�@���X�V
      m_shaderManager->UpdateConstantBuffer(m_cameraPerObject.Get(), &cameraPerObjectConstantBuffer, sizeof(cameraPerObjectConstantBuffer));
	  //�S�[�X�g��`��
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
	//�f�o�b�O�p�̃|�C���g��\���_�~�[�L�����N�^�[���쐬
    Character dummy;
	//�_�~�[�L�����N�^�[�̏�����
    dummy.Init(m_d3dDevice.Get(), debugPoint.color.x, debugPoint.color.y, debugPoint.color.z);
	//�_�~�[�L�����N�^�[�̈ʒu��ݒ�
    dummy.SetPosition(debugPoint.position.x, debugPoint.position.y, debugPoint.position.z);

	//�C���X�^���X�����ꂽ���_�V�F�[�_�[��ݒ�
    m_shaderManager->SetVertexShader(ShaderManager::VertexShader::Instanced);
	//�r���{�[�h�̃W�I���g���V�F�[�_�[��ݒ�
    m_shaderManager->SetGeometryShader(ShaderManager::GeometryShader::Billboard);
	//�F�t���̃s�N�Z���V�F�[�_�[��ݒ�
    m_shaderManager->SetPixelShader(ShaderManager::PixelShader::Color);

	//�t���[���̒萔�o�b�t�@��ݒ�
    Global::SpriteConstantBuffer frameConstantBuffer = { 0, 0, 1, 1, DirectX::XMFLOAT4(0.25f, 0, 0, 0) };
	//�t���[���̒萔�o�b�t�@���X�V
    m_shaderManager->UpdateConstantBuffer(m_frameBuffer.Get(), &frameConstantBuffer, sizeof(frameConstantBuffer));
	//�J�����̒萔�o�b�t�@��������
    Global::CameraPerObject cameraPerObjectConstantBuffer;
	//�_�~�[�L�����N�^�[�̃��[���h�s���ݒ�
    cameraPerObjectConstantBuffer.world = dummy.GetWorldMatrix();
	//�J�����̒萔�o�b�t�@���X�V
    m_shaderManager->UpdateConstantBuffer(m_cameraPerObject.Get(), &cameraPerObjectConstantBuffer, sizeof(cameraPerObjectConstantBuffer));
	//�_�~�[�L�����N�^�[��`��
    dummy.Draw(m_d3dContext.Get());
  }
  //�f�o�b�O�|�C���g���N���A
  m_debugPoints.clear();
}

void Game::MoveGhostTowardsPosition(float posX, float posZ, Game::Ghosts ghost)
{
  //�Ώۂ̃S�[�X�g���擾
  Ghost& character = *m_ghosts[static_cast<uint8_t>(ghost)];

  if (m_debugDraw)
    switch (ghost)
    {
      case Ghosts::Blinky:
		//�f�o�b�O�|�C���g��ǉ�(Blinky)
        m_debugPoints.push_back({{posX, 0.3f, posZ}, {0.0f ,1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}});
        break;
      case Ghosts::Pinky:
		//�f�o�b�O�|�C���g��ǉ�(Pinky)
        m_debugPoints.push_back({{posX, 0.3f, posZ}, {0.0f ,1.0f, 0.0f}, {1.0f, 0.6f, 0.8f}});
        break;
      case Ghosts::Inky:
		//�f�o�b�O�|�C���g��ǉ�(Inky)
        m_debugPoints.push_back({{posX, 0.3f, posZ}, {0.0f ,1.0f, 0.0f}, {0.2f, 1.0f, 1.0f}});
        break;
      case Ghosts::Clyde:
		//�f�o�b�O�|�C���g��ǉ�(Clyde)
        m_debugPoints.push_back({{posX, 0.3f, posZ}, {0.0f ,1.0f, 0.0f}, {1.0f, 0.8f, 0.2f}});
        break;
    }
  //�t���[���J�E���^�[�𑝉�
  character.IncreaseFrameCounter();

  switch (character.GetMovement())
  {
  case Character::Movement::Left:
	//���Ɉړ�
    character.AdjustPosition(-Global::ghostSpeed, 0, 0);
    break;
  case Character::Movement::Right:
	//�E�Ɉړ�
    character.AdjustPosition(Global::ghostSpeed, 0, 0);
    break;
  case Character::Movement::Up:
	//��Ɉړ�
    character.AdjustPosition(0, 0, Global::ghostSpeed);
    break;
  case Character::Movement::Down:
	//���Ɉړ�
    character.AdjustPosition(0, 0, -Global::ghostSpeed);
    break;
  default:
	//�����ɓ��B���邱�Ƃ͂���܂���//
    break;
  }

  //�S�[�X�g�̌��݈ʒu���擾
  const DirectX::XMFLOAT3& characterCurrentPos = character.GetPosition();
  //�S�[�X�g�����S���Ă��邩���m�F
  bool isCharacterDead = character.IsDead() || (character.GetMovement() == Character::Movement::InHouse);

  if (isCharacterDead)
  {
	//�S�[�X�g�ƃ^�[�Q�b�g�ʒu�Ƃ̋������v�Z
    float d = sqrt((characterCurrentPos.x - posX) * (characterCurrentPos.x - posX) + (characterCurrentPos.z - posZ) * (characterCurrentPos.z - posZ));

    if (d < 0.1f)
    {
	  //�}�b�v�ɍ��킹�Ĉʒu�𒲐�
      character.AlignToMap();
	  //�ړ���������ɐݒ�
      character.SetMovement(Character::Movement::Up);
	  //���[�h���`�F�C�X�ɐݒ�
      character.SetMode(Global::Mode::Chase);
	  //�S�[�X�g�𐶂��Ԃ点��
      character.SetDead(false);

      switch (ghost)
      {
      case Ghosts::Blinky:
	    //Blinky�̃X�v���C�g�s��ݒ�
        character.SetSpriteY(Global::rowBlinky);
        break;
      case Ghosts::Pinky:
		//Pinky�̃X�v���C�g�s��ݒ�
        character.SetSpriteY(Global::rowPinky);
        break;
      case Ghosts::Inky:
		//Inky�̃X�v���C�g�s��ݒ�
        character.SetSpriteY(Global::rowInky);
        break;
      case Ghosts::Clyde:
		//Clyde�̃X�v���C�g�s��ݒ�
        character.SetSpriteY(Global::rowClyde);
        break;
      }
	  //�֐��̏I��
      return;
    }
  }

  if (character.GetMovement() == Character::Movement::Left)
  {
	//�L�����N�^�[�����Ɉړ����Ă���ꍇ
    bool teleport = (characterCurrentPos.x - 0.5f) < Global::ghostSpeed ? true : false;

    if (teleport)
    {
	  //�e���|�[�g���K�v�ȏꍇ,�L�����N�^�[���E�[�Ɉړ�
      character.SetPosition(20.5f, characterCurrentPos.y, characterCurrentPos.z);
      return;
    }
  }
  else if (character.GetMovement() == Character::Movement::Right)
  {
	//�L�����N�^�[���E�Ɉړ����Ă���ꍇ
    bool teleport = (20.5f - characterCurrentPos.x) < Global::ghostSpeed ? true : false;

    if (teleport)
    {
	  //�e���|�[�g���K�v�ȏꍇ,�L�����N�^�[�����[�Ɉړ�
      character.SetPosition(0.5f, characterCurrentPos.y, characterCurrentPos.z);
      return;
    }
  }
  //�L�����N�^�[���}�b�v�̃O���b�h�ɑ����Ă��邩�ǂ������`�F�b�N
  bool isAligned = (fmod(characterCurrentPos.x - 0.5f, 1.0f) < Global::ghostSpeed) && (fmod(characterCurrentPos.z - 0.5f, 1.0f) < Global::ghostSpeed);

  if (isAligned)
  {
	//�L�����N�^�[���O���b�h�ɑ����Ă���ꍇ
    bool moves[Character::Direction::_Count] = { false, false, false, false };

	//�e�����ւ̈ړ��\�����`�F�b�N
    moves[Character::Direction::Up] = m_world.IsPassable(static_cast<uint8_t>(characterCurrentPos.x), static_cast<uint8_t>(characterCurrentPos.z + 1.0f), isCharacterDead);
    moves[Character::Direction::Right] = m_world.IsPassable(static_cast<uint8_t>(characterCurrentPos.x + 1.0f), static_cast<uint8_t>(characterCurrentPos.z), isCharacterDead);
    moves[Character::Direction::Down] = m_world.IsPassable(static_cast<uint8_t>(characterCurrentPos.x), static_cast<uint8_t>(characterCurrentPos.z - 1.0f), isCharacterDead);
    moves[Character::Direction::Left] = m_world.IsPassable(static_cast<uint8_t>(characterCurrentPos.x - 1.0f), static_cast<uint8_t>(characterCurrentPos.z), isCharacterDead);
	//�e�����ւ̋������v�Z
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
	//�ŒZ�����̕�����V�����ړ������Ƃ��Đݒ�
    Character::Movement newMoment = static_cast<Character::Movement>(std::min_element(distances.begin(), distances.end()) - distances.begin());

    if (newMoment != characterMovement && character.GetNumberOfFrames() >= Global::minFramesPerDirection)
    {
	  //�ړ��������ς�����ꍇ�A�L�����N�^�[���}�b�v�ɑ����A�V�����ړ�������ݒ�
      character.AlignToMap();
      character.SetMovement(newMoment);
	  //�t���[���J�E���^�[�����Z�b�g
      character.ResetFrameCounter();
    }
  }
}

void Game::MoveGhostTowardsRandomPosition(Game::Ghosts ghost)
{
  Ghost& character = *m_ghosts[static_cast<uint8_t>(ghost)];

  const DirectX::XMFLOAT3 charPos = character.GetPosition();
  uint8_t direction = rand() % 4;

  //�����_���ȕ����ɃS�[�X�g���ړ�
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
  //�S�[�X�g�������Ă���ꍇ��,�f�t�H���g�̃X�v���C�g��ݒ�
  if (BLINKY->IsAlive()) BLINKY->SetSpriteY(Global::rowBlinky);
  if (PINKY->IsAlive())  PINKY->SetSpriteY(Global::rowPinky);
  if (INKY->IsAlive())   INKY->SetSpriteY(Global::rowInky);
  if (CLYDE->IsAlive())  CLYDE->SetSpriteY(Global::rowClyde);
}

void Game::CreatePhases()
{
  //���x��1�̊e�t�F�[�Y��ݒ�
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
  //�e�S�[�X�g�ƃp�b�N�}���̋������v�Z��,�Փ˂�����
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
  //�Q�[���J�n�A�j���[�V�����̂��߂ɃJ�������X�V
  Global::CameraPerFrame cameraConstantBufferPerFrame = {};
  cameraConstantBufferPerFrame.view = m_camera.GetViewMatrix();
  cameraConstantBufferPerFrame.projection = m_camera.GetProjectionMatrix();

  m_shaderManager->UpdateConstantBuffer(m_cameraPerFrame.Get(), &cameraConstantBufferPerFrame, sizeof(cameraConstantBufferPerFrame));
}

void Game::NewGameInitialization()
{
  //�V�����Q�[���̏�����//

  //�h�b�g�̃��Z�b�g
  if (m_dots)
    m_dots.reset();

  m_dots = std::make_unique<Dots>();
  m_dots->Init(m_d3dDevice.Get());

  //�S�[�X�g�̏����ݒ�
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

  //�p�b�N�}���̏����ݒ�
  PACMAN->SetDead(false);
  PACMAN->SetPosition(10.5f, 0.25f, 5.5f);
  PACMAN->SetMovement(Character::Movement::Left);
  PACMAN->SetSpriteY(0);
  PACMAN->SetFramesPerState(2);
  PACMAN->SetOneCycle(false);
  PACMAN->SetColumnsAndRowsOfAssociatedSpriteSheet(12, 2);
  PACMAN->SetSpriteScaleFactor(Global::pacManSize);
  PACMAN->Restart();

  //�Q�[���̏����ݒ�
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
  //2�̈ړ����������������΂��𔻒�
  if (m1 == m2) return true;

  if (m1 == Character::Movement::Up && m2 == Character::Movement::Down) return true;
  if (m1 == Character::Movement::Right && m2 == Character::Movement::Left) return true;
  if (m1 == Character::Movement::Down && m2 == Character::Movement::Up) return true;
  if (m1 == Character::Movement::Left && m2 == Character::Movement::Right) return true;

  return false;
}

float Game::DistanceBetweenCharacters(const Character& ch1, const Character& ch2)
{
  //2�̃L�����N�^�[�̈ʒu���擾
  const XMFLOAT3& pos1 = ch1.GetPosition();
  const XMFLOAT3& pos2 = ch2.GetPosition();

  //2�̃L�����N�^�[�Ԃ̋������v�Z���ĕԂ�
  return sqrt((pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.z - pos2.z) * (pos1.z - pos2.z));
}

void Game::UpdatePositionOfBlinky()
{
  //�S�[�X�g������ł���ꍇ,����̈ʒu�Ɉړ�
  if (BLINKY->IsDead())
  {
    MoveGhostTowardsPosition(10.5f, 11.5f, Ghosts::Blinky);
    return;
  }
  //�S�[�X�g���n�E�X���ɂ���ꍇ,���݂̃t�F�[�Y�̃��[�h��ݒ�
  if (BLINKY->GetMovement() == Character::Movement::InHouse)
    BLINKY->SetMode(CURRENT_PHASE.mode);

  //�S�[�X�g�̃��[�h�ɉ����ē����ύX
  switch (BLINKY->GetMode())
  {
  case Global::Mode::Scatter:
	//Scatter���[�h�ł͓���̈ʒu�Ɉړ�
    MoveGhostTowardsPosition(18.5f, 21.5f, Ghosts::Blinky);
    break;
  case Global::Mode::Chase:
  {
	//Chase���[�h�ł̓p�b�N�}���̈ʒu�Ɉړ�
    const DirectX::XMFLOAT3& pacmanPos = PACMAN->GetPosition();
    MoveGhostTowardsPosition(pacmanPos.x, pacmanPos.z, Ghosts::Blinky);
  }
  break;
  case Global::Mode::Frightened:
	//Frightened���[�h�ł̓����_���Ȉʒu�Ɉړ�
    MoveGhostTowardsRandomPosition(Ghosts::Blinky);
    break;
  }
}

void Game::UpdatePositionOfPinky()
{
  //�S�[�X�g����~���Ă���ꍇ,�������Ȃ�
  if (PINKY->GetMovement() == Character::Movement::Stop)
    return;
  //�S�[�X�g������ł���ꍇ,����̈ʒu�Ɉړ�
  if (PINKY->IsDead())
  {
    MoveGhostTowardsPosition(10.5f, 11.5f, Ghosts::Pinky);
    return;
  }
  //�S�[�X�g���n�E�X���ɂ���ꍇ,���݂̃t�F�[�Y�̃��[�h��ݒ�
  if (PINKY->GetMovement() == Character::Movement::InHouse)
    PINKY->SetMode(CURRENT_PHASE.mode);

  switch (PINKY->GetMode())
  {
  case Global::Mode::Scatter:
	//����̈ʒu�Ɉړ�
    MoveGhostTowardsPosition(2.5f, 21.5f, Ghosts::Pinky);
    break;
  case Global::Mode::Chase:
  {
	//�p�b�N�}���̈ʒu�Ɋ�Â��Ĉړ�
    DirectX::XMFLOAT3 pacmanPos = PACMAN->GetPosition();

	//�p�b�N�}���̌����ɉ����Ĉʒu�𒲐�
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
	//�����_���Ȉʒu�Ɉړ�
    MoveGhostTowardsRandomPosition(Ghosts::Pinky);
    break;
  }
}

void Game::UpdatePositionOfInky()
{
  //�S�[�X�g����~���Ă���ꍇ,�������Ȃ�
  if (INKY->GetMovement() == Character::Movement::Stop)
    return;

  if (INKY->IsDead())
  {
	//�S�[�X�g������ł���ꍇ,����̈ʒu�Ɉړ�
    MoveGhostTowardsPosition(10.5f, 11.5f, Ghosts::Inky);
    return;
  }
  //�S�[�X�g���n�E�X���ɂ���ꍇ,���݂̃t�F�[�Y�̃��[�h��ݒ�
  if (INKY->GetMovement() == Character::Movement::InHouse)
    INKY->SetMode(CURRENT_PHASE.mode);

  //�S�[�X�g�̃��[�h�ɉ����ē����ύX
  switch (INKY->GetMode())
  {
  case Global::Mode::Scatter:
	//����̈ʒu�Ɉړ�
    MoveGhostTowardsPosition(21.5f, 0.0f, Ghosts::Inky);
    break;
  case Global::Mode::Chase:
  {
	//�p�b�N�}���̈ʒu�Ɋ�Â��Ĉړ�
    DirectX::XMFLOAT3 pacmanPos = PACMAN->GetPosition();

	//�p�b�N�}���̌����ɉ����Ĉʒu�𒲐�
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
	//�u�����L�[�̈ʒu���擾
    const DirectX::XMFLOAT3 blinkyPos = BLINKY->GetPosition();

    float finalPosX = 0;
    float finalPosZ = 0;

	//�u�����L�[�̈ʒu����ɁA�ŏI�I�Ȉړ��ʒu���v�Z
    finalPosX = pacmanPos.x + (pacmanPos.x - blinkyPos.x);
    finalPosZ = pacmanPos.z + (pacmanPos.z - blinkyPos.z);

    MoveGhostTowardsPosition(finalPosX, finalPosZ, Ghosts::Inky);
  }
  break;
  case Global::Mode::Frightened:
	//�����_���Ȉʒu�Ɉړ�
    MoveGhostTowardsRandomPosition(Ghosts::Inky);
    break;
  }
}

void Game::UpdatePositionOfClyde()
{
  //�S�[�X�g����~���Ă���ꍇ,�������Ȃ�
  if (CLYDE->GetMovement() == Character::Movement::Stop)
    return;

  //�S�[�X�g������ł���ꍇ,����̈ʒu�Ɉړ�
  if (CLYDE->IsDead())
  {
    MoveGhostTowardsPosition(10.5f, 11.5f, Ghosts::Clyde);
    return;
  }
  //�S�[�X�g���n�E�X���ɂ���ꍇ,���݂̃t�F�[�Y�̃��[�h��ݒ�
  if (CLYDE->GetMovement() == Character::Movement::InHouse)
    CLYDE->SetMode(CURRENT_PHASE.mode);

  //�S�[�X�g�̃��[�h�ɉ����ē����ύX
  switch (CLYDE->GetMode())
  {
  case Global::Mode::Scatter:
	//����̈ʒu�Ɉړ�
    MoveGhostTowardsPosition(0.0f, 0.0f, Ghosts::Clyde);
    break;
  case Global::Mode::Chase:
  {
	//�p�b�N�}���̈ʒu�Ɋ�Â��Ĉړ�
    const DirectX::XMFLOAT3& pacmanPos = PACMAN->GetPosition();
    const float distance = DistanceBetweenCharacters(*PACMAN, *CLYDE);

    if (distance > 8)
    {
	  // ������8�ȏ�̏ꍇ,�u�����L�[�̂悤�ɐU�镑��
      MoveGhostTowardsPosition(pacmanPos.x, pacmanPos.z, Ghosts::Clyde);
    }
    else
    {
	  //������8�����̏ꍇ,Scatter���[�h�Ƃ��ĐU�镑��
      MoveGhostTowardsPosition(0.0f, 0.0f, Ghosts::Clyde);
    }
  }
  break;
  case Global::Mode::Frightened:
	//�����_���Ȉʒu�Ɉړ�
    MoveGhostTowardsRandomPosition(Ghosts::Clyde);
    break;
  }
}

//�f�o�C�X�Ɉˑ����郊�\�[�X���쐬
void Game::CreateDevice()
{
  UINT creationFlags = 0;

#ifdef _DEBUG
  //�f�o�b�O�t���O��ݒ�
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

  //DX11API�f�o�C�X�I�u�W�F�N�g���쐬��,�Ή�����R���e�L�X�g���擾
  ComPtr<ID3D11Device> device;
  ComPtr<ID3D11DeviceContext> context;
  DX::ThrowIfFailed(D3D11CreateDevice(
    nullptr,                            //�f�t�H���g�A�_�v�^���g�p���邽�߂�nullptr���w��
    D3D_DRIVER_TYPE_HARDWARE,
    nullptr,
    creationFlags,
    featureLevels,
    _countof(featureLevels),
    D3D11_SDK_VERSION,
    device.ReleaseAndGetAddressOf(),    //�쐬���ꂽDirect3D�f�o�C�X��Ԃ�
    &m_featureLevel,                    //�쐬���ꂽ�f�o�C�X�̋@�\���x����Ԃ�
    context.ReleaseAndGetAddressOf()    //�f�o�C�X�̑����R���e�L�X�g��Ԃ�
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
  //�f�o�C�X�ƃR���e�L�X�g��ۑ�
  DX::ThrowIfFailed(device.As(&m_d3dDevice));
  DX::ThrowIfFailed(context.As(&m_d3dContext));
  //�V�F�[�_�[�}�l�[�W����������
  m_shaderManager = std::make_unique<ShaderManager>(m_d3dDevice.Get(), m_d3dContext.Get());
}


//�E�B���h�E�̃T�C�Y�ύX�C�x���g�Ɉˑ����邷�ׂẴ��������\�[�X�����蓖�Ă�
void Game::CreateResources()
{
  //�O�̃E�B���h�E�T�C�Y�Ɉˑ�����R���e�L�X�g���N���A
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

  //�X���b�v�`�F�[�������ɑ��݂���ꍇ�̓��T�C�Y��,���݂��Ȃ��ꍇ�͐V�����쐬
  if (m_swapChain)
  {
    HRESULT hr = m_swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);

    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
	  //�f�o�C�X�����炩�̗��R�ō폜���ꂽ�ꍇ,�V�����f�o�C�X�ƃX���b�v�`�F�[�����쐬����K�v������
      OnDeviceLost();

     //OnDeviceLost�����̃��\�b�h���ē��͂�,�V�����f�o�C�X�𐳂����Z�b�g�A�b�v����
      return;
    }
    else
    {
      DX::ThrowIfFailed(hr);
    }
  }
  else
  {
	//�ŏ���,D3D�f�o�C�X�����ɂȂ�DXGI�f�o�C�X���擾
    ComPtr<IDXGIDevice1> dxgiDevice;
    DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

	//���̃f�o�C�X�����삵�Ă��镨���A�_�v�^(GPU�܂��̓J�[�h)������
    ComPtr<IDXGIAdapter> dxgiAdapter;
    DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

	//������,������쐬�����t�@�N�g���I�u�W�F�N�g���擾
    ComPtr<IDXGIFactory2> dxgiFactory;
    DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

	//�X���b�v�`�F�[���̃f�B�X�N���v�^���쐬
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

	//Win32�E�B���h�E����X���b�v�`�F�[�����쐬
    DX::ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
      m_d3dDevice.Get(),
      m_window,
      &swapChainDesc,
      &fsSwapChainDesc,
      nullptr,
      m_swapChain.ReleaseAndGetAddressOf()
    ));

	//DXGI��ALT + ENTER�V���[�g�J�b�g�ɉ�������̂�h��
    DX::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
  }

  //���̃E�B���h�E�̃o�b�N�o�b�t�@���擾��,�ŏI�I��3D�����_�[�^�[�Q�b�g�Ƃ��Ďg�p
  ComPtr<ID3D11Texture2D> backBuffer;
  DX::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

  //�o�b�N�o�b�t�@��Ƀ����_�[�^�[�Q�b�g�r���[���쐬��,�o�C���h�Ɏg�p
  DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf()));

  //�[�x/�X�e���V���o�b�t�@�Ƃ���2D�T�[�t�F�X�����蓖��,���̃T�[�t�F�X��ɐ[�x�X�e���V���r���[���쐬���ăo�C���h�Ɏg�p
  CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);

  ComPtr<ID3D11Texture2D> depthStencil;
  DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

  CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
  DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf()));

  //�E�B���h�E�T�C�Y�ˑ��̃I�u�W�F�N�g��������
  m_camera.SetProjectionValues(75.0f, static_cast<float>(m_outputWidth) / static_cast<float>(m_outputHeight), 0.1f, 1000.0f);
  m_camera.SetOrthographicValues(static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));

  XMFLOAT4X4 projection = m_camera.GetProjectionMatrix();
  //�萔�o�b�t�@�̐ݒ�
  D3D11_BUFFER_DESC cbd = {};
  cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  cbd.Usage = D3D11_USAGE_DYNAMIC;
  cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  cbd.MiscFlags = 0;
  cbd.ByteWidth = sizeof(XMFLOAT4X4);
  cbd.StructureByteStride = 0;

  D3D11_SUBRESOURCE_DATA csd = {};
  csd.pSysMem = &projection;

  //�v���W�F�N�V�����}�g���b�N�X�p�̒萔�o�b�t�@���쐬
  DX::ThrowIfFailed(m_d3dDevice->CreateBuffer(&cbd, &csd, &m_projectionMatrixConstantBuffer));

  //�J�����̃t���[�����Ƃ̒萔�o�b�t�@��ݒ�
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
  //�t���[�����Ƃ̒萔�o�b�t�@���쐬
  DX::ThrowIfFailed(m_d3dDevice->CreateBuffer(&cbd, &csd, &m_cameraPerFrame));

  //�I�u�W�F�N�g���Ƃ̃J�����̒萔�o�b�t�@��ݒ�
  Global::CameraPerObject cameraConstantBufferPerObject;
  XMStoreFloat4x4(&cameraConstantBufferPerObject.world, DirectX::XMMatrixIdentity());

  cbd.ByteWidth = sizeof(Global::CameraPerObject);

  csd.pSysMem = &cameraConstantBufferPerObject;

  //�I�u�W�F�N�g���Ƃ̒萔�o�b�t�@���쐬
  DX::ThrowIfFailed(m_d3dDevice->CreateBuffer(&cbd, &csd, &m_cameraPerObject));

  //�t���[���萔�o�b�t�@�̐ݒ�
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

  //�t���[���萔�o�b�t�@���쐬
  DX::ThrowIfFailed(m_d3dDevice->CreateBuffer(&cbd, &csd, &m_frameBuffer));
  //�萔�o�b�t�@���W�I���g���V�F�[�_�Ƀo�C���h
  ID3D11Buffer* geometryShaderBuffers[2] = { m_projectionMatrixConstantBuffer.Get(), m_frameBuffer.Get() };
  m_shaderManager->BindConstantBuffersToGeometryShader(ShaderManager::GeometryShader::Billboard, geometryShaderBuffers, 2);
  //�萔�o�b�t�@�𒸓_�V�F�[�_�Ƀo�C���h
  ID3D11Buffer* vertexShaderBuffers[2] = { m_cameraPerFrame.Get(), m_cameraPerObject.Get() };
  m_shaderManager->BindConstantBuffersToVertexShader(ShaderManager::VertexShader::Instanced, vertexShaderBuffers, 2);
  m_shaderManager->BindConstantBuffersToVertexShader(ShaderManager::VertexShader::Indexed, vertexShaderBuffers, 2);
  //�L���v�V�����̈ʒu��ݒ�
  m_caption->SetPosition(static_cast<float>((m_outputWidth / 2) - (m_caption->GetWidth() / 2)), static_cast<float>(-m_caption->GetHeight() * 2), 0);
}

void Game::OnDeviceLost()
{
  //�f�o�C�X������ꂽ�ꍇ�̃��\�[�X�̃��Z�b�g
  m_depthStencilView.Reset();
  m_renderTargetView.Reset();
  m_swapChain.Reset();
  m_d3dContext.Reset();
  m_d3dDevice.Reset();
  //�f�o�C�X���č쐬
  CreateDevice();
  //���\�[�X���č쐬
  CreateResources();
}

void Game::SetSpriteConstantBufferForCharacter(Global::SpriteConstantBuffer& spriteConstantBuffer, const Character& character)
{
  //�L�����N�^�[�̃X�v���C�g�萔�o�b�t�@��ݒ�
  spriteConstantBuffer.spriteX = character.GetSpriteX();
  spriteConstantBuffer.spriteY = character.GetSpriteY();
  spriteConstantBuffer.spriteSheetColumns = character.GetSpriteSheetColumns();
  spriteConstantBuffer.spriteSheetRows = character.GetSpriteSheetRows();
  spriteConstantBuffer.billboardSize_0_0_0 = DirectX::XMFLOAT4(character.GetSpriteScaleFactor(), 0, 0, 0);
}
