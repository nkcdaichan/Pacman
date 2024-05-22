//
//pch.h
//�W���V�X�e���C���N���[�h�t�@�C���̃w�b�_�[
//

#pragma once

#include <WinSDKVer.h>
#define _WIN32_WINNT 0x0601
#include <SDKDDKVer.h>

//C++�W���̃e���v���[�g�����ꂽmin/max���g�p
#define NOMINMAX

//DirectX�A�v���ɂ�GDI�͕s�v
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

//<mcx.h>���܂߂�
#define NOMCX

//<winsvc.h>���܂߂�
#define NOSERVICE


#define NOHELP

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <wrl/client.h>

#include <d3d11_1.h>
#include <dxgi1_2.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

#include <algorithm>
#include <limits>
#include <exception>
#include <memory>
#include <stdexcept>
namespace DX
{
  inline void ThrowIfFailed(HRESULT hr)
  {
    if (FAILED(hr))
    {
	  //���̍s�Ƀu���[�N�|�C���g��ݒ肵��DirectX API�G���[���L���b�`����
      throw std::exception();
    }
  }
}
