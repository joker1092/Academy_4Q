#include "Sampler.h"

Sampler::Sampler(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode, float offset, Mathf::Color4* borderColor)
{
    auto device = deviceResources->GetD3DDevice();
    Create(device, filter, addressMode, offset, borderColor);
}

void Sampler::Create(ID3D11Device* device, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode, float offset, Mathf::Color4* borderColor)
{
    CD3D11_SAMPLER_DESC samplerDesc{
        filter,
        addressMode,
        addressMode,
        addressMode,
        offset,
        D3D11_MAX_MAXANISOTROPY,
        D3D11_COMPARISON_NEVER,
        borderColor ? *borderColor : Mathf::Color4(0.0f, 0.0f, 0.0f, 0.0f),
        0,
        D3D11_FLOAT32_MAX
    };

    if (borderColor)
    {
        samplerDesc.BorderColor[0] = borderColor->x;
        samplerDesc.BorderColor[1] = borderColor->y;
        samplerDesc.BorderColor[2] = borderColor->z;
        samplerDesc.BorderColor[3] = borderColor->w;
    }

    DirectX11::ThrowIfFailed(
        device->CreateSamplerState(
            &samplerDesc,
            m_Sampler.GetAddressOf()
        )
    );
}
