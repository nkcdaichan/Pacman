#pragma once

#include <vector>

#include "Global.h"

class Caption
{
public:
  //コンストラクタ
  Caption();
  //デストラクタ
  ~Caption();
  //描画処理
  void Draw(ID3D11DeviceContext1* context);
  //初期化処理
  void Init(ID3D11Device1* device, uint16_t width, uint16_t height);
  //Y座標を調整
  void AdjustY(float value, float minimum);
  //位置を設定する
  void SetPosition(float x, float y, float z);
  //幅を取得
  uint16_t GetWidth() { return m_width; }
  //高さを取得
  uint16_t GetHeight() { return m_height; }

  //ワールド行列を取得
  const DirectX::XMFLOAT4X4& GetWorldMatrix() const { return m_worldMatrix; }

private:
  //ワールド行列を更新
  void UpdateWorldMatrix();
  //頂点バッファ
  Microsoft::WRL::ComPtr<ID3D11Buffer>             m_vertexBuffer;
  //カリングなしのラスタライザーステート
  Microsoft::WRL::ComPtr<ID3D11RasterizerState>    m_cullNone;
  //リソース
  Microsoft::WRL::ComPtr<ID3D11Resource>           m_resource;
  //シェーダーリソースビュー
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
  //サンプラーステート
  Microsoft::WRL::ComPtr<ID3D11SamplerState>       m_samplerState;
  //ブレンドステート
  Microsoft::WRL::ComPtr<ID3D11BlendState>         m_blendState;
  //インデックスバッファ
  Microsoft::WRL::ComPtr<ID3D11Buffer>             m_indexBuffer;

  //位置
  DirectX::XMFLOAT3 m_position;
  //ワールド行列
  DirectX::XMFLOAT4X4 m_worldMatrix;

  //頂点データ
  std::vector<Global::Vertex> m_vertices;
  //インデックスデータ
  std::vector<uint16_t> m_indices;
  //幅
  uint16_t m_width;
  //高さ
  uint16_t m_height;
};
