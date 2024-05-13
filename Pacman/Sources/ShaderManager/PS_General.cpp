#include "pch.h"

#include "PS_General.h"

PS_General::PS_General(const std::wstring& file, ID3D11Device1* device)
{
  Microsoft::WRL::ComPtr<ID3DBlob> blob;
  D3DReadFileToBlob(file.c_str(), &blob);

  device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, GetShaderPP());
}

void PS_General::ActivateShader(ID3D11DeviceContext1* context)
{
  context->PSSetShader(GetShader(), NULL, 0);
}

void PS_General::BindConstantBuffers(ID3D11DeviceContext1* context, ID3D11Buffer** constantBuffers, uint8_t numberOfConstantBuffers)
{
  (void)context;
  (void)constantBuffers;
  (void)numberOfConstantBuffers;
}
