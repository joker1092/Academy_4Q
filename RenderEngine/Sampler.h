#pragma once
#include "Core.Minimal.h"
#include "DeviceResources.h"

class Sampler
{
public:
    Sampler() = default;
    Sampler(
        const std::shared_ptr<DirectX11::DeviceResources>& deviceResources,
        D3D11_FILTER filter,
        D3D11_TEXTURE_ADDRESS_MODE addressMode = D3D11_TEXTURE_ADDRESS_MIRROR,
        float offset = 0.0f,
        Mathf::Color4* borderColor = nullptr
    );
	Sampler(
		ID3D11Device* device,
		D3D11_FILTER filter,
		D3D11_TEXTURE_ADDRESS_MODE addressMode = D3D11_TEXTURE_ADDRESS_MIRROR,
		float offset = 0.0f,
		Mathf::Color4* borderColor = nullptr
	);
    ~Sampler() = default;

    ID3D11SamplerState* Get() { return m_Sampler.Get(); }
    ID3D11SamplerState* const* GetAddressOf() const { return m_Sampler.GetAddressOf(); }

private:
    void Create(
        ID3D11Device* device,
        D3D11_FILTER filter,
        D3D11_TEXTURE_ADDRESS_MODE addressMode = D3D11_TEXTURE_ADDRESS_MIRROR,
        float offset = 0,
		Mathf::Color4* borderColor = nullptr
    );

    ComPtr<ID3D11SamplerState> m_Sampler{};
};

