#pragma once

#include <vector>

#include "Global.h"

class Dots
{
public:
  enum class Type
  {
    Nothing,      //何もない状態
    Normal,       //普通のドット
    Extra,        //特別なドット
    LastOne       //最後の一つ
  };

  struct InstanceType
  {
    DirectX::XMFLOAT3 position;      //座標
    uint8_t sizeFlag;                //サイズフラグ
  };

  //コンストラクタ
  explicit Dots();
  //デストラクタ
  ~Dots();

  //描画
  void Draw(ID3D11DeviceContext1* context);
  //初期化
  void Init(ID3D11Device1* device);

  //更新
  void Update(uint8_t column, uint8_t row, ID3D11DeviceContext1* context, Type& dotEaten);

  //ワールド行列の取得
  const DirectX::XMFLOAT4X4& GetWorldMatrix() const { return m_worldMatrix; }

private:
  Microsoft::WRL::ComPtr<ID3D11Buffer>             m_vertexBuffer;          //頂点バッファ
  Microsoft::WRL::ComPtr<ID3D11RasterizerState>    m_cullNone;              //カリングなしのラスタライザステート
  Microsoft::WRL::ComPtr<ID3D11Resource>           m_resource;              //リソース
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;    //シェーダーリソースビュー
  Microsoft::WRL::ComPtr<ID3D11SamplerState>       m_samplerState;          //サンプラステート
  Microsoft::WRL::ComPtr<ID3D11BlendState>         m_blendState;            //ブレンドステート
  Microsoft::WRL::ComPtr<ID3D11Buffer>             m_instanceBuffer;        //インスタンスバッファ

  //ワールド行列
  DirectX::XMFLOAT4X4 m_worldMatrix;

  //ドットの配列
  uint8_t m_dots[Global::worldSize][Global::worldSize];

  //ドットの数
  uint16_t m_numberOfDots;

  //頂点の配列
  std::vector<Global::Vertex> m_vertices;
  //インスタンスの配列
  std::vector<InstanceType> m_instances;
};

