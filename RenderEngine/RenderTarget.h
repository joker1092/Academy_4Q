#pragma once
#include "Core.Minimal.h"
#include "DeviceResources.h"

class RenderTarget
{
public:
    RenderTarget(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources,
        ID3D11Texture2D* texture, DXGI_FORMAT format, uint32 width, uint32 height);
    RenderTarget(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources,
        DXGI_FORMAT format, uint32 width, uint32 height);
    RenderTarget(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
        DXGI_FORMAT format, uint32 width, uint32 height);
    ~RenderTarget();

    void Resize(uint32 width, uint32 height);

    void ClearView();

    ID3D11RenderTargetView* const* GetRTVAddressOf() const { return m_RTV.GetAddressOf(); }
    ID3D11RenderTargetView* GetRTV() const { return m_RTV.Get(); }
    ID3D11DepthStencilView* const* GetDSVAddressOf() const { return m_DSV.GetAddressOf(); }
    ID3D11DepthStencilView* GetDSV() const { return m_DSV.Get(); }
    ID3D11ShaderResourceView* GetSRV() const { return m_SRV.Get(); }

    ID3D11Texture2D* GetTexture() const { return m_Texture.Get(); }
    ID3D11Texture2D* GetStencil() const { return m_Stencil.Get(); }

    XMVECTORF32 color{};
    uint32 width{};
    uint32 height{};
    DXGI_FORMAT format{};

private:
    void CreateTexture();
    void CreateTargets();
    void CreateShaderResourceView();
    void DestroyTargets();

    bool m_bHasSRV{ false };
    ID3D11Device* m_Device{};
    ID3D11DeviceContext* m_DeviceContext{};
    ComPtr<ID3D11Texture2D> m_Texture{};
    ComPtr<ID3D11Texture2D> m_Stencil{};

    ComPtr<ID3D11RenderTargetView> m_RTV{};
    ComPtr<ID3D11DepthStencilView> m_DSV{};
    ComPtr<ID3D11ShaderResourceView> m_SRV{};
};
