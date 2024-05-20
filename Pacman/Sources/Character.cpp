#include "pch.h"

#include "Character.h"

using namespace DirectX;
//コンストラクタ
Character::Character() :   
  m_currentFrame(4),                      //現在のフレームを初期化
  m_spriteY(0),                           //スプライトのY座標を初期化
  m_position(0, 0, 0),                    //キャラクターの位置を初期化
  m_movement(Movement::Stop),             //キャラクターの移動状態を初期化
  m_frameCounter(0),                      //フレームカウンタを初期化
  m_facingDirection(Direction::Left),     //キャラクターの向きを初期化
  m_spriteSheetColumns(0),                //スプライトシートの列数を初期化
  m_spriteSheetRows(0),                   //スプライトシートの行数を初期化
  m_spriteXAddition(0),                   //スプライトのX座標の追加値を初期化
  m_framesPerState(1),                    //状態ごとのフレーム数を初期化
  m_oneCycle(false),                      //アニメーションが一周期のみかどうかを初期化
  m_isAnimationDone(false),               //アニメーションが完了したかどうかを初期化
  m_isDead(false),                        //キャラクターが死んでいるかどうかを初期化
  m_totalElapsed(0.0),                    //経過時間を初期化
  m_timePerFrame(0.0)                     //フレームごとの時間を初期化
{
  //ワールドマトリックスを更新
  UpdateWorldMatrix();
  //フレームごとの時間を設定(1秒間に10フレーム)
  m_timePerFrame = 1.0f / 10.0f;
}
//デストラクタ
Character::~Character()
{
}
//位置を設定
void Character::SetPosition(float x, float y, float z)
{
  m_position.x = x;
  m_position.y = y;
  m_position.z = z;
  //ワールドマトリックスの更新
  UpdateWorldMatrix();
}
//位置を調整
void Character::AdjustPosition(float x, float y, float z)
{
  m_position.x += x;
  m_position.y += y;
  m_position.z += z;
  //ワールドマトリックスの更新
  UpdateWorldMatrix();
}
//移動状態を設定
void Character::SetMovement(Movement movement)
{
  m_movement = movement;
  //キャラクターが動いていて死んでいない場合は向きを更新
  if (movement != Movement::Stop && !m_isDead)
    m_facingDirection = static_cast<Direction>(static_cast<uint8_t>(movement));

  //移動状態に応じてフレームとスプライトX座標の追加値を設定
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
//フレームを更新
void Character::UpdateFrame(double elapsedTime)
{
  m_totalElapsed += elapsedTime;

  //フレームの更新時間に達していない場合はリターン
  if (m_totalElapsed < m_timePerFrame)
    return;

  m_totalElapsed -= m_timePerFrame;

  //一周期のみのアニメーションの場合はX追加値をリセット
  if (m_oneCycle)
    m_spriteXAddition = 0;

  //新しいフレームを計算
  uint8_t newFrame = (m_currentFrame + 1) % m_framesPerState + m_spriteXAddition;

  //アニメーションが完了したかを判定
  m_isAnimationDone = newFrame < m_currentFrame ? true : false;

  //アニメーションが完了していないか、一周期のみでない場合はフレームを更新
  if (!(m_isAnimationDone && m_oneCycle))
    m_currentFrame = newFrame;
}
//初期化
void Character::Init(ID3D11Device1* device, float r, float g, float b)
{
  m_vertices.push_back({ {0,0,0}, {0.0, 1.0, 0.0}, {r, g, b} });

  //頂点バッファの設定
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

  //ラスタライザの設定
  D3D11_RASTERIZER_DESC cmdesc = {};
  cmdesc.FillMode = D3D11_FILL_SOLID;
  cmdesc.FrontCounterClockwise = false;
  cmdesc.CullMode = D3D11_CULL_NONE;

  DX::ThrowIfFailed(device->CreateRasterizerState(&cmdesc, m_cullNone.GetAddressOf()));

  //サンプラーステートの設定
  D3D11_SAMPLER_DESC sampDesc = {};
  sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
  sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
  sampDesc.MinLOD = 0;
  sampDesc.MaxLOD = 0;

  //サンプラーステートの作成
  DX::ThrowIfFailed(device->CreateSamplerState(&sampDesc, m_samplerState.GetAddressOf()));

  //インスタンスの設定
  m_instances.push_back({ {0,0,0}, 1 });

  //インスタンスバッファの設定
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

  //インスタンスバッファの作成
  DX::ThrowIfFailed(device->CreateBuffer(&instanceBufferDesc, &instanceData, m_instanceBuffer.GetAddressOf()));
}
//初期化(デフォルト色)
void Character::Init(ID3D11Device1* device)
{
  Init(device, 0.0f, 0.0f, 0.0f);
}
//描画 キャラクターを描画
void Character::Draw(ID3D11DeviceContext1* context)
{
  unsigned int strides[2];
  unsigned int offsets[2];

  //バッファのストライドを設定
  strides[0] = sizeof(Global::Vertex);
  strides[1] = sizeof(InstanceType);

  //バッファのオフセットを設定
  offsets[0] = 0;
  offsets[1] = 0;

  //頂点バッファとインスタンスバッファのポインタを設定
  ID3D11Buffer* bufferPointers[2];
  bufferPointers[0] = m_vertexBuffer.Get();
  bufferPointers[1] = m_instanceBuffer.Get();

  //プリミティブのトポロジーを設定
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
  context->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

  //サンプラーをピクセルシェーダーに設定
  context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

  //ラスタライザステートを設定
  context->RSSetState(m_cullNone.Get());

  //インスタンス描画
  context->DrawInstanced(1, (UINT)m_instances.size(), 0, 0);
}
//マップに位置を合わせる
void Character::AlignToMap()
{
  m_position.x = floor(m_position.x) + 0.5f;
  m_position.z = floor(m_position.z) + 0.5f;
  //ワールドマトリックス更新
  UpdateWorldMatrix();
}
//フレームカウンタを増加
void Character::IncreaseFrameCounter()
{
  m_frameCounter++;
}
//フレームカウンタをリセット
void Character::ResetFrameCounter()
{
  m_frameCounter = 0;
}
//移動方向を逆にする
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
//関連スプライトシートの列数と行数を設定
void Character::SetColumnsAndRowsOfAssociatedSpriteSheet(uint8_t columns, uint8_t rows)
{
  m_spriteSheetColumns = columns;
  m_spriteSheetRows = rows;
}
//ワールドマトリックスを更新
void Character::UpdateWorldMatrix()
{
  XMStoreFloat4x4(&m_worldMatrix, XMMatrixTranspose(XMMatrixTranslation(m_position.x, m_position.y, m_position.z)));
}
