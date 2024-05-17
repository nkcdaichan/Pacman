#pragma once
//std::array���g�p���邽��
#include <array>
//std::vector���g�p���邽��
#include <vector>

#include "IShader.h"
//�N���X�̒�`
class ShaderManager
{
public:
  //VertexShader�̎�ނ�񋓂���
  enum class VertexShader
  {
    Indexed,      //�C���f�b�N�X�t�����_�V�F�[�_�[
    Instanced,    //�C���X�^���V���O�p���_�V�F�[�_�[
    UI,           //UI�p���_�V�F�[�_�[
    _Count        //���_�V�F�[�_�[�̐���\��
  };
  //GeometryShader�̎�ނ�񋓂���
  enum class GeometryShader
  {
    Billboard,    //�r���{�[�h�p�W�I���g���V�F�[�_�[
    _Count        //�W�I���g���V�F�[�_�[�̐���\��
  };
  //PixelShader�̎�ނ�񋓂���
  enum class PixelShader
  {
    Color,        //�J���[�p�s�N�Z���V�F�[�_�[
    Texture,      //�e�N�X�`���p�s�N�Z���V�F�[�_�[
    Phong,        //�t�H���V�F�[�f�B���O�p�s�N�Z���V�F�[�_�[
    UI,           //UI�p�s�N�Z���V�F�[�_�[
    _Count        //�s�N�Z���V�F�[�_�[�̐���\��
  };
  //�f�o�C�X�ƃR���e�L�X�g��������
  ShaderManager(ID3D11Device1* device, ID3D11DeviceContext1* context);
  //�f�X�g���N�^
  ~ShaderManager();
  //�w�肳�ꂽ���_�V�F�[�_�[���Z�b�g
  void SetVertexShader(VertexShader vertexShader);
  //�w�肳�ꂽ�W�I���g���V�F�[�_�[���Z�b�g
  void SetGeometryShader(GeometryShader geometryShader);
  //�w�肳�ꂽ�s�N�Z���V�F�[�_�[���Z�b�g
  void SetPixelShader(PixelShader pixelShader);
  //���_�V�F�[�_�[�Ƀo�C���h����萔�o�b�t�@��ݒ�
  void BindConstantBuffersToVertexShader(VertexShader vertexShader, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers);
  //�W�I���g���V�F�[�_�[�Ƀo�C���h����萔�o�b�t�@��ݒ�
  void BindConstantBuffersToGeometryShader(GeometryShader geometryShader, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers);
  //�s�N�Z���V�F�[�_�[�Ƀo�C���h����萔�o�b�t�@��ݒ�
  void BindConstantBuffersToPixelShader(PixelShader pixelShader, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers);
  //�w�肳�ꂽ�萔�o�b�t�@�̓��e���X�V
  void UpdateConstantBuffer(ID3D11Buffer* constantBuffer, void* data, size_t dataLength);
  //�p�C�v���C�����炷�ׂẴV�F�[�_�[���N���A
  void ClearShadersFromThePipeline();

private:
  //���O��`���ꂽ�V�F�[�_�[��ǉ�
  void AddPredefinedOnes();
  //�s�N�Z���V�F�[�_�[���i�[����z��
  std::array<std::unique_ptr<IShader<ID3D11PixelShader>>, static_cast<uint8_t>(PixelShader::_Count)>        m_pixelShaders;
  //�W�I���g���V�F�[�_�[���i�[����z��
  std::array<std::unique_ptr<IShader<ID3D11GeometryShader>>, static_cast<uint8_t>(GeometryShader::_Count)>  m_geometryShaders;
  //���_�V�F�[�_�[���i�[����z��
  std::array<std::unique_ptr<IShader<ID3D11VertexShader>>, static_cast<uint8_t>(VertexShader::_Count)>      m_vertexShaders;
  //�f�o�C�X�C���^�[�t�F�[�X
  ID3D11Device1*        m_device;
  //�f�o�C�X�R���e�L�X�g�C���^�[�t�F�[�X
  ID3D11DeviceContext1* m_context;
};

