#pragma once
#include "Core.Minimal.h"

interface ITextureData
{
    ITextureData() = default;
    virtual ~ITextureData() = default;
};

struct Texture2D : public ITextureData
{
    Texture2D() : m_Width(0), m_Height(0), m_format(DXGI_FORMAT_UNKNOWN) {}
    Texture2D(ID3D11Texture2D* texture, ID3D11ShaderResourceView* srv) : m_Texture(texture), m_SRV(srv)
    {
        D3D11_TEXTURE2D_DESC desc{};
        m_Texture->GetDesc(&desc);
        m_Width = desc.Width;
        m_Height = desc.Height;
        m_format = desc.Format;
    }
    Texture2D(const std::string_view& name, ID3D11Texture2D* texture, ID3D11ShaderResourceView* srv) :
        m_Name(name), m_Texture(texture), m_SRV(srv)
    {
        D3D11_TEXTURE2D_DESC desc{};
        m_Texture->GetDesc(&desc);
        m_Width = desc.Width;
        m_Height = desc.Height;
        m_format = desc.Format;
        DirectX::SetName(m_Texture.Get(), name);
    }

    Texture2D(ID3D11Texture2D* texture, D3D11_TEXTURE2D_DESC desc) :
        m_Texture(texture), m_Width(desc.Width), m_Height(desc.Height), m_format(desc.Format)
    {
    }
    ~Texture2D() override = default;

    ID3D11Texture2D* GetTexture() const { return m_Texture.Get(); }
    ID3D11Texture2D* const* GetTextureAddressOf() const { return m_Texture.GetAddressOf(); }
    ID3D11ShaderResourceView* Get() const { return m_SRV.Get(); }
	ID3D11ShaderResourceView* const* GetAddressOf() const { return m_SRV.GetAddressOf(); }

    void Reset()
    {
        m_Texture.Reset();
    }

    void Resize(ID3D11Device* pDevice, uint32 width, uint32 height)
    {
        D3D11_TEXTURE2D_DESC desc{};
        m_Texture->GetDesc(&desc);
        if (desc.Width == width && desc.Height == height)
        {
            return;
        }

        m_Width = width;
        m_Height = height;

        desc.Width = width;
        desc.Height = height;

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        m_SRV->GetDesc(&srvDesc);

        pDevice->CreateTexture2D(&desc, nullptr, m_Texture.ReleaseAndGetAddressOf());
        pDevice->CreateShaderResourceView(m_Texture.Get(), &srvDesc, m_SRV.ReleaseAndGetAddressOf());
    }

	void SetName(const std::string_view& name)
	{
		m_Name = name;
		DirectX::SetName(m_Texture.Get(), name);
	}

    uint32 m_Width{};
    uint32 m_Height{};
    DXGI_FORMAT m_format{};
    std::string m_Name{};

private:
    ComPtr<ID3D11Texture2D> m_Texture{};
    ComPtr<ID3D11ShaderResourceView> m_SRV{};
};
