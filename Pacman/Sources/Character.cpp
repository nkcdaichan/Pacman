#include "pch.h"

#include "Character.h"

using namespace DirectX;
//�R���X�g���N�^
Character::Character() :   
  m_currentFrame(4),                      //���݂̃t���[����������
  m_spriteY(0),                           //�X�v���C�g��Y���W��������
  m_position(0, 0, 0),                    //�L�����N�^�[�̈ʒu��������
  m_movement(Movement::Stop),             //�L�����N�^�[�̈ړ���Ԃ�������
  m_frameCounter(0),                      //�t���[���J�E���^��������
  m_facingDirection(Direction::Left),     //�L�����N�^�[�̌�����������
  m_spriteSheetColumns(0),                //�X�v���C�g�V�[�g�̗񐔂�������
  m_spriteSheetRows(0),                   //�X�v���C�g�V�[�g�̍s����������
  m_spriteXAddition(0),                   //�X�v���C�g��X���W�̒ǉ��l��������
  m_framesPerState(1),                    //��Ԃ��Ƃ̃t���[������������
  m_oneCycle(false),                      //�A�j���[�V������������݂̂��ǂ�����������
  m_isAnimationDone(false),               //�A�j���[�V�����������������ǂ�����������
  m_isDead(false),                        //�L�����N�^�[������ł��邩�ǂ�����������
  m_totalElapsed(0.0),                    //�o�ߎ��Ԃ�������
  m_timePerFrame(0.0)                     //�t���[�����Ƃ̎��Ԃ�������
{
  //���[���h�}�g���b�N�X���X�V
  UpdateWorldMatrix();
  //�t���[�����Ƃ̎��Ԃ�ݒ�(1�b�Ԃ�10�t���[��)
  m_timePerFrame = 1.0f / 10.0f;
}
//�f�X�g���N�^
Character::~Character()
{
}
//�ʒu��ݒ�
void Character::SetPosition(float x, float y, float z)
{
  m_position.x = x;
  m_position.y = y;
  m_position.z = z;
  //���[���h�}�g���b�N�X�̍X�V
  UpdateWorldMatrix();
}
//�ʒu�𒲐�
void Character::AdjustPosition(float x, float y, float z)
{
  m_position.x += x;
  m_position.y += y;
  m_position.z += z;
  //���[���h�}�g���b�N�X�̍X�V
  UpdateWorldMatrix();
}
//�ړ���Ԃ�ݒ�
void Character::SetMovement(Movement movement)
{
  m_movement = movement;
  //�L�����N�^�[�������Ă��Ď���ł��Ȃ��ꍇ�͌������X�V
  if (movement != Movement::Stop && !m_isDead)
    m_facingDirection = static_cast<Direction>(static_cast<uint8_t>(movement));

  //�ړ���Ԃɉ����ăt���[���ƃX�v���C�gX���W�̒ǉ��l��ݒ�
  switch (movement)
  {
  case Movement::Up:
    m_currentFrame = 0;
    m_spriteXAddition = 0;
    break;
  case Movement::Down:
    m_currentFrame = 2;
    m_spriteXAddition = 2;
    break;
  case Movement::Left:
    m_currentFrame = 4;
    m_spriteXAddition = 4;
    break;
  case Movement::Right:
    m_currentFrame = 6;
    m_spriteXAddition = 6;
    break;
  }
}
//�t���[�����X�V
void Character::UpdateFrame(double elapsedTime)
{
  m_totalElapsed += elapsedTime;

  //�t���[���̍X�V���ԂɒB���Ă��Ȃ��ꍇ�̓��^�[��
  if (m_totalElapsed < m_timePerFrame)
    return;

  m_totalElapsed -= m_timePerFrame;

  //������݂̂̃A�j���[�V�����̏ꍇ��X�ǉ��l�����Z�b�g
  if (m_oneCycle)
    m_spriteXAddition = 0;

  //�V�����t���[�����v�Z
  uint8_t newFrame = (m_currentFrame + 1) % m_framesPerState + m_spriteXAddition;

  //�A�j���[�V�����������������𔻒�
  m_isAnimationDone = newFrame < m_currentFrame ? true : false;

  //�A�j���[�V�������������Ă��Ȃ����A������݂̂łȂ��ꍇ�̓t���[�����X�V
  if (!(m_isAnimationDone && m_oneCycle))
    m_currentFrame = newFrame;
}
//������
void Character::Init(ID3D11Device1* device, float r, float g, float b)
{
  m_vertices.push_back({ {0,0,0}, {0.0, 1.0, 0.0}, {r, g, b} });

  //���_�o�b�t�@�̐ݒ�
  D3D11_BUFFER_DESC bd = {};
  bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  bd.Usage = D3D11_USAGE_DEFAULT;
  bd.CPUAccessFlags = 0;
  bd.MiscFlags = 0;
  bd.ByteWidth = UINT(sizeof(Global::Vertex) * m_vertices.size());
  bd.StructureByteStride = sizeof(Global::Vertex);

  D3D11_SUBRESOURCE_DATA sd = {};
  sd.pSysMem = m_vertices.data();

  DX::ThrowIfFailed(device->CreateBuffer(&bd, &sd, &m_vertexBuffer));

  //���X�^���C�U�̐ݒ�
  D3D11_RASTERIZER_DESC cmdesc = {};
  cmdesc.FillMode = D3D11_FILL_SOLID;
  cmdesc.FrontCounterClockwise = false;
  cmdesc.CullMode = D3D11_CULL_NONE;

  DX::ThrowIfFailed(device->CreateRasterizerState(&cmdesc, m_cullNone.GetAddressOf()));

  //�T���v���[�X�e�[�g�̐ݒ�
  D3D11_SAMPLER_DESC sampDesc = {};
  sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
  sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
  sampDesc.MinLOD = 0;
  sampDesc.MaxLOD = 0;

  //�T���v���[�X�e�[�g�̍쐬
  DX::ThrowIfFailed(device->CreateSamplerState(&sampDesc, m_samplerState.GetAddressOf()));

  //�C���X�^���X�̐ݒ�
  m_instances.push_back({ {0,0,0}, 1 });

  //�C���X�^���X�o�b�t�@�̐ݒ�
  D3D11_BUFFER_DESC instanceBufferDesc = {};
  instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  instanceBufferDesc.ByteWidth = sizeof(InstanceType) * (UINT)m_instances.size();
  instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  instanceBufferDesc.CPUAccessFlags = 0;
  instanceBufferDesc.MiscFlags = 0;
  instanceBufferDesc.StructureByteStride = 0;

  D3D11_SUBRESOURCE_DATA instanceData = {};
  instanceData.pSysMem = m_instances.data();
  instanceData.SysMemPitch = 0;
  instanceData.SysMemSlicePitch = 0;

  //�C���X�^���X�o�b�t�@�̍쐬
  DX::ThrowIfFailed(device->CreateBuffer(&instanceBufferDesc, &instanceData, m_instanceBuffer.GetAddressOf()));
}
//������(�f�t�H���g�F)
void Character::Init(ID3D11Device1* device)
{
  Init(device, 0.0f, 0.0f, 0.0f);
}
//�`�� �L�����N�^�[��`��
void Character::Draw(ID3D11DeviceContext1* context)
{
  unsigned int strides[2];
  unsigned int offsets[2];

  //�o�b�t�@�̃X�g���C�h��ݒ�
  strides[0] = sizeof(Global::Vertex);
  strides[1] = sizeof(InstanceType);

  //�o�b�t�@�̃I�t�Z�b�g��ݒ�
  offsets[0] = 0;
  offsets[1] = 0;

  //���_�o�b�t�@�ƃC���X�^���X�o�b�t�@�̃|�C���^��ݒ�
  ID3D11Buffer* bufferPointers[2];
  bufferPointers[0] = m_vertexBuffer.Get();
  bufferPointers[1] = m_instanceBuffer.Get();

  //�v���~�e�B�u�̃g�|���W�[��ݒ�
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
  context->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

  //�T���v���[���s�N�Z���V�F�[�_�[�ɐݒ�
  context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

  //���X�^���C�U�X�e�[�g��ݒ�
  context->RSSetState(m_cullNone.Get());

  //�C���X�^���X�`��
  context->DrawInstanced(1, (UINT)m_instances.size(), 0, 0);
}
//�}�b�v�Ɉʒu�����킹��
void Character::AlignToMap()
{
  m_position.x = floor(m_position.x) + 0.5f;
  m_position.z = floor(m_position.z) + 0.5f;
  //���[���h�}�g���b�N�X�X�V
  UpdateWorldMatrix();
}
//�t���[���J�E���^�𑝉�
void Character::IncreaseFrameCounter()
{
  m_frameCounter++;
}
//�t���[���J�E���^�����Z�b�g
void Character::ResetFrameCounter()
{
  m_frameCounter = 0;
}
//�ړ��������t�ɂ���
void Character::ReverseMovementDirection()
{
  switch (m_movement)
  {
  case Movement::Left:  SetMovement(Character::Movement::Right); break;
  case Movement::Right: SetMovement(Character::Movement::Left); break;
  case Movement::Up:    SetMovement(Character::Movement::Down); break;
  case Movement::Down:  SetMovement(Character::Movement::Up); break;
  }
}
//�֘A�X�v���C�g�V�[�g�̗񐔂ƍs����ݒ�
void Character::SetColumnsAndRowsOfAssociatedSpriteSheet(uint8_t columns, uint8_t rows)
{
  m_spriteSheetColumns = columns;
  m_spriteSheetRows = rows;
}
//���[���h�}�g���b�N�X���X�V
void Character::UpdateWorldMatrix()
{
  XMStoreFloat4x4(&m_worldMatrix, XMMatrixTranspose(XMMatrixTranslation(m_position.x, m_position.y, m_position.z)));
}
