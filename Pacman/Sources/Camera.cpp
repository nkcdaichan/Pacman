#include "pch.h"

#include "Camera.h"
#include "Global.h"

using namespace DirectX;
//�R���X�g���N�^
Camera::Camera() :
  m_position(0.0f, 0.0f, 0.0f),   //�����ʒu��(0,0,0)�ɐݒ�
  m_rotation(0.0f, 0.0f, 0.0f),   //������]�l��(0,0,0)�ɐݒ�
  m_lerpCoef(0.0f),               //��ԌW����0�ɏ�����
  m_lerpDone(false)               //��Ԋ����t���O��false�ɏ�����
{
  //�J�����ʒu��Global::frontCamera�̈ʒu�ɐݒ�
  SetPosition(Global::frontCamera.x, Global::frontCamera.y, Global::frontCamera.z);
  //�J�����̒����_��(10.5, 0, 10.5)�ɐݒ�
  SetLookAtPos(10.5, 0, 10.5);
  //�r���[�}�g���b�N�X���X�V
  UpdateViewMatrix();
}
//�f�X�g���N�^
Camera::~Camera()
{
}
//2�̃J�����ʒu�Ԃ���`��Ԃ���֐�
void Camera::LerpBetweenCameraPositions(float lerpCoef)
{
  m_lerpCoef += lerpCoef;          //��ԌW�������Z
  //��ԌW����1�𒴂������Ԋ���
  if (m_lerpCoef > 1.0f)
  {
    m_lerpCoef = 1.0f;
    m_lerpDone = true;
  }
  else
  {
    m_lerpDone = false;
  }
  //��Ԃ��ꂽ�ʒu���v�Z
  float posX = Global::frontCamera.x + m_lerpCoef * (Global::upCamera.x - Global::frontCamera.x);
  float posY = Global::frontCamera.y + m_lerpCoef * (Global::upCamera.y - Global::frontCamera.y);
  float posZ = Global::frontCamera.z + m_lerpCoef * (Global::upCamera.z - Global::frontCamera.z);
  //�v�Z���ꂽ�ʒu�ɃJ�������ړ�
  SetPosition(posX, posY, posZ);
  //�J�����̒����_���Đݒ�
  SetLookAtPos(10.5, 0, 10.5);
}
//�t�����ɐ��`��Ԃ���
void Camera::InverseLerpBetweenCameraPositions(float lerpCoef)
{
  m_lerpCoef += lerpCoef;
  //��ԌW����1�𒴂������Ԋ���
  if (m_lerpCoef > 1.0f)
  {
    m_lerpCoef = 1.0f;
    m_lerpDone = true;
  }
  else
  {
    m_lerpDone = false;
  }
  //�t�����̕�Ԃ��ꂽ�ʒu���v�Z
  float posX = Global::upCamera.x + m_lerpCoef * (Global::frontCamera.x - Global::upCamera.x);
  float posY = Global::upCamera.y + m_lerpCoef * (Global::frontCamera.y - Global::upCamera.y);
  float posZ = Global::upCamera.z + m_lerpCoef * (Global::frontCamera.z - Global::upCamera.z);
  //�v�Z���ꂽ�ʒu�ɃJ�������ړ�
  SetPosition(posX, posY, posZ);
  //�J�����̒����_���Đݒ�
  SetLookAtPos(10.5, 0, 10.5);
}
//�ˉe�s���ݒ�
void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
  //�x�����W�A���ɕϊ�
  float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
  //�ˉe�s����v�Z��,�]�u���ĕۑ�
  XMStoreFloat4x4(&m_projectionMatrix, XMMatrixTranspose(XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ)));
}
//���e�s���ݒ肷��
void Camera::SetOrthographicValues(float width, float height)
{
  //���e�s����v�Z��,�]�u���ĕۑ�
  XMStoreFloat4x4(&m_orthoMatrix, XMMatrixTranspose(XMMatrixOrthographicOffCenterLH(0, width, height, 0, 0.0f, 100.0f)));
}
//�J�����̈ʒu��ݒ肷��
void Camera::SetPosition(float x, float y, float z)
{
  m_position.x = x;                //x���W��ݒ�
  m_position.y = y;                //y���W��ݒ�
  m_position.z = z;                //z���W��ݒ�
  //�r���[�}�g���b�N�X���X�V
  UpdateViewMatrix();
}
//�J�����̉�]��ݒ�
void Camera::SetRotation(float x, float y, float z)
{
  m_rotation.x = x;                //�s�b�`��ݒ�
  m_rotation.y = y;                //���[��ݒ�
  m_rotation.z = z;                //���[����ݒ�
  //�r���[�}�g���b�N�X���X�V
  UpdateViewMatrix();
}
//�J�����̒����_��ݒ肷��
void Camera::SetLookAtPos(float x, float y, float z)
{
  //�����_�̈ʒu
  XMFLOAT3 lookAtPos = {};

  lookAtPos.x = m_position.x - x;   //x���W���v�Z
  lookAtPos.y = m_position.y - y;   //y���W���v�Z
  lookAtPos.z = m_position.z - z;   //z���W���v�Z

  //�s�b�`�p
  float pitch = 0.0f;           
  if (lookAtPos.y != 0.0f)
  {
    const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
	//�s�b�`�p���v�Z
    pitch = atan(lookAtPos.y / distance);   
  }
  //���[�p
  float yaw = 0.0f;
  if (lookAtPos.x != 0.0f)
	//���[�p���v�Z
    yaw = atan(lookAtPos.x / lookAtPos.z);

  if (lookAtPos.z > 0)
	//z�����Ȃ�΃΂����Z
    yaw += XM_PI;
  //��]��ݒ�
  SetRotation(pitch, yaw, 0.0f);
}
//�r���[�}�g���b�N�X���X�V����
void Camera::UpdateViewMatrix()
{
  //�J�����̉�]�s����v�Z
  XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
  //�J�����̑O���x�N�g������]
  XMVECTOR camTarget = XMVector3TransformCoord(forwardVector, camRotationMatrix);
  //�J�����̈ʒu�x�N�g�������[�h
  XMVECTOR positionVector = XMLoadFloat3(&m_position);
  //������x�N�g������]
  XMVECTOR upDir = XMVector3TransformCoord(upVector, camRotationMatrix);
  //�J�����̒����_���v�Z
  camTarget += positionVector;
  //�r���[�}�g���b�N�X���v�Z��,�]�u���ĕۑ�
  XMStoreFloat4x4(&m_viewMatrix, XMMatrixTranspose(XMMatrixLookAtLH(positionVector, camTarget, upDir)));
}
