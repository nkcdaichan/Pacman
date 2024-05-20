#pragma once

#include <DirectXMath.h>

class Camera
{
public:
  //�R���X�g���N�^
  explicit Camera();
  //�f�X�g���N�^
  ~Camera();

  //�J�����ʒu���Ԃ���
  void LerpBetweenCameraPositions(float lerpCoef);
  //�J�����ʒu���t��Ԃ���
  void InverseLerpBetweenCameraPositions(float lerpCoef);
  //��ԌW�������Z�b�g����
  void ResetLerp() { m_lerpCoef = 0; }

  //�ˉe�s��̐ݒ�
  void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);
  //���ˉe�s��̐ݒ�
  void SetOrthographicValues(float width, float height);
  //�J�����ʒu�̐ݒ�
  void SetPosition(float x, float y, float z);
  //�J������]�̐ݒ�
  void SetRotation(float x, float y, float z);
  //�����_�̐ݒ�
  void SetLookAtPos(float x, float y, float z);

  //�r���[�s��̎擾
  const DirectX::XMFLOAT4X4& GetViewMatrix() const { return m_viewMatrix; }
  //�ˉe�s��̎擾
  const DirectX::XMFLOAT4X4& GetProjectionMatrix() const { return m_projectionMatrix; }
  //���ˉe�s��̎擾
  const DirectX::XMFLOAT4X4& GetOrthographicMatrix() const { return m_orthoMatrix; }
  //�J�����̕�Ԃ��������������m�F
  bool IsCameraLerpDone() { return m_lerpDone; }

private:
  //�r���[�s����X�V����
  void UpdateViewMatrix();

  //�J�����̑O���x�N�g��
  const DirectX::XMVECTOR forwardVector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
  //�J�����̏�����x�N�g��
  const DirectX::XMVECTOR upVector = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

  //�J�����̈ʒu
  DirectX::XMFLOAT3 m_position;
  //�J�����̉�]
  DirectX::XMFLOAT3 m_rotation;

  //�r���[�s��
  DirectX::XMFLOAT4X4 m_viewMatrix;
  //�ˉe�s��
  DirectX::XMFLOAT4X4 m_projectionMatrix;
  //���ˉe�s��
  DirectX::XMFLOAT4X4 m_orthoMatrix;
  //��ԌW��
  float m_lerpCoef;

  //��Ԃ������������ǂ����������t���O
  bool m_lerpDone;
};
