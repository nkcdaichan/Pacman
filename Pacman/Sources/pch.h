//
//pch.h
//標準システムインクルードファイルのヘッダー
//

#pragma once

#include <WinSDKVer.h>
#define _WIN32_WINNT 0x0601
#include <SDKDDKVer.h>

//C++標準のテンプレート化されたmin/maxを使用
#define NOMINMAX

//DirectXアプリにはGDIは不要
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

//<mcx.h>を含める
#define NOMCX

//<winsvc.h>を含める
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
	  //この行にブレークポイントを設定してDirectX APIエラーをキャッチする
      throw std::exception();
    }
  }
}
