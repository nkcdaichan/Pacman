#pragma once

#include <DirectXMath.h>

class Camera
{
public:
  //コンストラクタ
  explicit Camera();
  //デストラクタ
  ~Camera();

  //カメラ位置を補間する
  void LerpBetweenCameraPositions(float lerpCoef);
  //カメラ位置を逆補間する
  void InverseLerpBetweenCameraPositions(float lerpCoef);
  //補間係数をリセットする
  void ResetLerp() { m_lerpCoef = 0; }

  //射影行列の設定
  void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);
  //正射影行列の設定
  void SetOrthographicValues(float width, float height);
  //カメラ位置の設定
  void SetPosition(float x, float y, float z);
  //カメラ回転の設定
  void SetRotation(float x, float y, float z);
  //注視点の設定
  void SetLookAtPos(float x, float y, float z);

  //ビュー行列の取得
  const DirectX::XMFLOAT4X4& GetViewMatrix() const { return m_viewMatrix; }
  //射影行列の取得
  const DirectX::XMFLOAT4X4& GetProjectionMatrix() const { return m_projectionMatrix; }
  //正射影行列の取得
  const DirectX::XMFLOAT4X4& GetOrthographicMatrix() const { return m_orthoMatrix; }
  //カメラの補間が完了したかを確認
  bool IsCameraLerpDone() { return m_lerpDone; }

private:
  //ビュー行列を更新する
  void UpdateViewMatrix();

  //カメラの前方ベクトル
  const DirectX::XMVECTOR forwardVector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
  //カメラの上方向ベクトル
  const DirectX::XMVECTOR upVector = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

  //カメラの位置
  DirectX::XMFLOAT3 m_position;
  //カメラの回転
  DirectX::XMFLOAT3 m_rotation;

  //ビュー行列
  DirectX::XMFLOAT4X4 m_viewMatrix;
  //射影行列
  DirectX::XMFLOAT4X4 m_projectionMatrix;
  //正射影行列
  DirectX::XMFLOAT4X4 m_orthoMatrix;
  //補間係数
  float m_lerpCoef;

  //補間が完了したかどうかを示すフラグ
  bool m_lerpDone;
};
