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

//�N���X�̐錾
class Game
{
public:
  //�Q�[���̏�Ԃ�\���񋓌^
  enum class State
  {
    Intro,   //�C���g�����
    Start,   //�Q�[���J�n
    Level,   //���x���v���C��
    Dead     //�v���C���[�����S
  };
  //�S�[�X�g�̎�ނ�\���񋓌^
  enum class Ghosts
  {
    Blinky,  //�u�����L�[
    Pinky,   //�s���L�[
    Inky,    //�C���L�[
    Clyde,   //�N���C�h
    None     //�S�[�X�g�Ȃ�
  };
  //�t�F�[�Y�̍\����
  struct Phase
  {
    Global::Mode mode;     //���[�h
    double startingTime;   //�J�n����
    double duration;       //�p������
  };

  Game() noexcept;

  //�������ƊǗ�
  void Initialize(HWND window, uint16_t width, uint16_t height);

  //��{�I�ȃQ�[�����[�v
  void Tick();

  //���b�Z�[�W����
  void OnActivated();
  void OnDeactivated();
  void OnSuspending();
  void OnResuming();
  void OnWindowSizeChanged(uint16_t width, uint16_t height);

  //�v���p�e�B
  void GetDefaultSize(uint16_t& width, uint16_t& height) const;

private:
  //���[���h�`��
  void DrawWorld();
  //�C���g����ʕ`��
  void DrawIntro();
  //�X�v���C�g�`��
  void DrawSprites();
  //�f�o�b�O�`��
  void DrawDebug();

  //�e�S�[�X�g�̈ʒu�X�V
  void UpdatePositionOfBlinky();
  void UpdatePositionOfPinky();
  void UpdatePositionOfInky();
  void UpdatePositionOfClyde();
  void Update(const DX::StepTimer& timer);

  //�����_�����O����
  void Render();
  //�N���A����
  void Clear();
  //�\������
  void Present();

  //�f�o�C�X�쐬
  void CreateDevice();
  //���\�[�X�쐬
  void CreateResources();

  //�f�o�C�X���X�g����
  void OnDeviceLost();

  //�L�����N�^�[�p�̃X�v���C�g�萔�o�b�t�@�ݒ�
  void SetSpriteConstantBufferForCharacter(Global::SpriteConstantBuffer& spriteConstantBuffer, const Character& character);
  //�S�[�X�g�����̈ʒu�Ɉړ�
  void MoveGhostTowardsPosition(float posX, float posZ, Game::Ghosts ghost);
  //�S�[�X�g�������_���Ȉʒu�Ɉړ�
  void MoveGhostTowardsRandomPosition(Game::Ghosts ghost);
  //�S�[�X�g�̃f�t�H���g�X�v���C�g�ݒ�
  void SetGhostsDefaultSprites();
  //�t�F�[�Y�쐬
  void CreatePhases();
  //�Փˏ���
  void HandleCollisions();
  //�X�^�[�g�A�j���[�V�����p�̃J�����X�V
  void UpdateCameraForStartAnimation();
  //�V�����Q�[���̏�����
  void NewGameInitialization();

  //�ړ����t�܂��͓������ǂ����𔻒�
  bool AreMovementsOppositeOrSame(Character::Movement m1, Character::Movement m2);

  //�L�����N�^�[�Ԃ̋������v�Z
  float DistanceBetweenCharacters(const Character& ch1, const Character& ch2);

  //Direct3D11�̃f�o�C�X�ƃR���e�L�X�g
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

  //�E�B���h�E�n���h��
  HWND m_window;
  //�X�e�b�v�^�C�}�[
  DX::StepTimer m_timer;
  //�p�b�N�}���̈ړ��v��
  Character::Movement m_pacmanMovementRequest;

  //�J����
  Camera m_camera;
  //���[���h
  World m_world;

  //���j�[�N�|�C���^�ɂ�郊�\�[�X�Ǘ�
  std::unique_ptr<Dots> m_dots;
  std::unique_ptr<Caption> m_caption;
  std::unique_ptr<ShaderManager> m_shaderManager;
  std::unique_ptr<DirectX::Keyboard> m_keyboard;

  //�f�o�b�O�|�C���g�̃x�N�g��
  std::vector<Global::Vertex> m_debugPoints;
  //�S�[�X�g�̔z��
  std::array<std::unique_ptr<Ghost>, Global::numGhosts> m_ghosts;
  //�p�b�N�}���̃��j�[�N�|�C���^
  std::unique_ptr<Character> m_pacman;

  //���x��1�̃t�F�[�Y�z��
  std::array<Phase, Global::phasesNum> m_phasesLevel1; 

  //���݂̃t�F�[�Y�C���f�b�N�X
  uint8_t m_currentPhaseIndex;
  //�O�̃t�F�[�Y�C���f�b�N�X
  uint8_t m_previousPhaseIndex;
  //���݂̃S�[�X�g�J�E���^�[
  Ghosts m_currentGhostCounter;

  //�f�o�b�O�`��t���O
  bool m_debugDraw;
  //���т���ԑJ�ڃt���O
  bool m_frightenedTransition;
  //�Q�[���ꎞ��~�t���O
  bool m_gamePaused;

  //�o�͂̕��ƍ���
  uint16_t m_outputWidth;
  uint16_t m_outputHeight;
  //�Q�[���̏��
  State m_gameState;
};
