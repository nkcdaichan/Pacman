#pragma once

#include <vector>

#include "Global.h"

class World
{
public:
  //コンストラクタ
  explicit World();
  //デストラクタ
  ~World();

  //ワールドの初期化
  void Init(ID3D11Device1* device);
  //ワールドの描画
  void Draw(ID3D11DeviceContext1* context);
  //通行可能かどうかを判定
  bool IsPassable(uint8_t column, uint8_t row, bool canGoHome);
  //指定した位置のタイルを取得
  uint8_t GetTile(uint8_t column, uint8_t row) { return m_map[row][column]; }
  //ワールド変換行列を取得
  const DirectX::XMFLOAT4X4& GetWorldMatrix() const { return m_worldMatrix; }

private:
  //ブロックを追加
  void AddBlock(float x, float z, float depth, bool north, bool west, bool south, bool east);
  //ワールドの生成
  void Generate(ID3D11Device1* device);

  //ワールドのマップデータ
  const uint8_t m_map[Global::worldSize][Global::worldSize];

  //頂点バッファ
  Microsoft::WRL::ComPtr<ID3D11Buffer>           m_vertexBuffer;
  //インデックスバッファ
  Microsoft::WRL::ComPtr<ID3D11Buffer>           m_indexBuffer;
  //ラスタライザステート
  Microsoft::WRL::ComPtr<ID3D11RasterizerState>  m_cullCW;

  //ワールド変換行列
  DirectX::XMFLOAT4X4 m_worldMatrix;

  //頂点情報
  std::vector<Global::Vertex> m_vertices;
  //インデックス情報
  std::vector<uint16_t> m_indices;
};

