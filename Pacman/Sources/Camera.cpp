#include "pch.h"

#include "Camera.h"
#include "Global.h"

using namespace DirectX;
//コンストラクタ
Camera::Camera() :
  m_position(0.0f, 0.0f, 0.0f),   //初期位置を(0,0,0)に設定
  m_rotation(0.0f, 0.0f, 0.0f),   //初期回転値を(0,0,0)に設定
  m_lerpCoef(0.0f),               //補間係数を0に初期化
  m_lerpDone(false)               //補間完了フラグをfalseに初期化
{
  //カメラ位置をGlobal::frontCameraの位置に設定
  SetPosition(Global::frontCamera.x, Global::frontCamera.y, Global::frontCamera.z);
  //カメラの注視点を(10.5, 0, 10.5)に設定
  SetLookAtPos(10.5, 0, 10.5);
  //ビューマトリックスを更新
  UpdateViewMatrix();
}
//デストラクタ
Camera::~Camera()
{
}
//2つのカメラ位置間を線形補間する関数
void Camera::LerpBetweenCameraPositions(float lerpCoef)
{
  m_lerpCoef += lerpCoef;          //補間係数を加算
  //補間係数が1を超えたら補間完了
  if (m_lerpCoef > 1.0f)
  {
    m_lerpCoef = 1.0f;
    m_lerpDone = true;
  }
  else
  {
    m_lerpDone = false;
  }
  //補間された位置を計算
  float posX = Global::frontCamera.x + m_lerpCoef * (Global::upCamera.x - Global::frontCamera.x);
  float posY = Global::frontCamera.y + m_lerpCoef * (Global::upCamera.y - Global::frontCamera.y);
  float posZ = Global::frontCamera.z + m_lerpCoef * (Global::upCamera.z - Global::frontCamera.z);
  //計算された位置にカメラを移動
  SetPosition(posX, posY, posZ);
  //カメラの注視点を再設定
  SetLookAtPos(10.5, 0, 10.5);
}
//逆方向に線形補間する
void Camera::InverseLerpBetweenCameraPositions(float lerpCoef)
{
  m_lerpCoef += lerpCoef;
  //補間係数が1を超えたら補間完了
  if (m_lerpCoef > 1.0f)
  {
    m_lerpCoef = 1.0f;
    m_lerpDone = true;
  }
  else
  {
    m_lerpDone = false;
  }
  //逆方向の補間された位置を計算
  float posX = Global::upCamera.x + m_lerpCoef * (Global::frontCamera.x - Global::upCamera.x);
  float posY = Global::upCamera.y + m_lerpCoef * (Global::frontCamera.y - Global::upCamera.y);
  float posZ = Global::upCamera.z + m_lerpCoef * (Global::frontCamera.z - Global::upCamera.z);
  //計算された位置にカメラを移動
  SetPosition(posX, posY, posZ);
  //カメラの注視点を再設定
  SetLookAtPos(10.5, 0, 10.5);
}
//射影行列を設定
void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
  //度をラジアンに変換
  float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
  //射影行列を計算し,転置して保存
  XMStoreFloat4x4(&m_projectionMatrix, XMMatrixTranspose(XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ)));
}
//投影行列を設定する
void Camera::SetOrthographicValues(float width, float height)
{
  //投影行列を計算し,転置して保存
  XMStoreFloat4x4(&m_orthoMatrix, XMMatrixTranspose(XMMatrixOrthographicOffCenterLH(0, width, height, 0, 0.0f, 100.0f)));
}
//カメラの位置を設定する
void Camera::SetPosition(float x, float y, float z)
{
  m_position.x = x;                //x座標を設定
  m_position.y = y;                //y座標を設定
  m_position.z = z;                //z座標を設定
  //ビューマトリックスを更新
  UpdateViewMatrix();
}
//カメラの回転を設定
void Camera::SetRotation(float x, float y, float z)
{
  m_rotation.x = x;                //ピッチを設定
  m_rotation.y = y;                //ヨーを設定
  m_rotation.z = z;                //ロールを設定
  //ビューマトリックスを更新
  UpdateViewMatrix();
}
//カメラの注視点を設定する
void Camera::SetLookAtPos(float x, float y, float z)
{
  //注視点の位置
  XMFLOAT3 lookAtPos = {};

  lookAtPos.x = m_position.x - x;   //x座標を計算
  lookAtPos.y = m_position.y - y;   //y座標を計算
  lookAtPos.z = m_position.z - z;   //z座標を計算

  //ピッチ角
  float pitch = 0.0f;           
  if (lookAtPos.y != 0.0f)
  {
    const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
	//ピッチ角を計算
    pitch = atan(lookAtPos.y / distance);   
  }
  //ヨー角
  float yaw = 0.0f;
  if (lookAtPos.x != 0.0f)
	//ヨー角を計算
    yaw = atan(lookAtPos.x / lookAtPos.z);

  if (lookAtPos.z > 0)
	//zが正ならばπを加算
    yaw += XM_PI;
  //回転を設定
  SetRotation(pitch, yaw, 0.0f);
}
//ビューマトリックスを更新する
void Camera::UpdateViewMatrix()
{
  //カメラの回転行列を計算
  XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
  //カメラの前方ベクトルを回転
  XMVECTOR camTarget = XMVector3TransformCoord(forwardVector, camRotationMatrix);
  //カメラの位置ベクトルをロード
  XMVECTOR positionVector = XMLoadFloat3(&m_position);
  //上方向ベクトルを回転
  XMVECTOR upDir = XMVector3TransformCoord(upVector, camRotationMatrix);
  //カメラの注視点を計算
  camTarget += positionVector;
  //ビューマトリックスを計算し,転置して保存
  XMStoreFloat4x4(&m_viewMatrix, XMMatrixTranspose(XMMatrixLookAtLH(positionVector, camTarget, upDir)));
}
