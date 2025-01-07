#pragma once
#include "Core.Minimal.h"
#include "DeviceResources.h"

interface IShaderResource
{
    IShaderResource() : m_name("null") {};
    IShaderResource(
        const std::shared_ptr<DirectX11::DeviceResources>& deviceResources,
        const std::string_view& name, ID3DBlob* blob) :
        m_d3dDevice(deviceResources->GetD3DDevice()),
        m_name(name),
        m_bCompiled(false),
        m_Blob(blob)
    {
    };
    virtual ~IShaderResource() = default;

    void SetDevice(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources)
    {
        m_d3dDevice = deviceResources->GetD3DDevice();
    }

    virtual void Compile() abstract;

    void* GetBufferPointer() const
    {
        return m_Blob->GetBufferPointer();
    }

    size_t GetBufferSize() const
    {
        return m_Blob->GetBufferSize();
    }

    std::string m_name{};

protected:
    ID3D11Device* m_d3dDevice{};
    bool m_bCompiled{};
    ComPtr<ID3DBlob> m_Blob{};
};

class VertexShader : public IShaderResource
{
public:
    VertexShader() = default;
    VertexShader(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources,
        const std::string_view& name, ID3DBlob* blob) :
        IShaderResource(deviceResources, name, blob)
    {
    };
    ~VertexShader() override = default;
    void Compile() override
    {
        if (m_bCompiled)
            return;

        DirectX11::ThrowIfFailed(
            m_d3dDevice->CreateVertexShader(
                m_Blob->GetBufferPointer(),
                m_Blob->GetBufferSize(),
                nullptr,
                m_VertexShader.ReleaseAndGetAddressOf()
            )
        );
        m_bCompiled = true;
    }

    ID3D11VertexShader* Get()
    {
        return m_VertexShader.Get();
    }

    void Reset()
    {
        m_VertexShader.Reset();
    }

private:
    ComPtr<ID3D11VertexShader> m_VertexShader{};
};

class HullShader : public IShaderResource
{
public:
    HullShader() = default;
    HullShader(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources,
        const std::string_view& name, ID3DBlob* blob) :
        IShaderResource(deviceResources, name, blob)
    {
    };
    ~HullShader() override = default;
    void Compile() override
    {
        if (m_bCompiled)
            return;
        DirectX11::ThrowIfFailed(
            m_d3dDevice->CreateHullShader(
                m_Blob->GetBufferPointer(),
                m_Blob->GetBufferSize(),
                nullptr,
                m_HullShader.ReleaseAndGetAddressOf()
            )
        );
        m_bCompiled = true;
    }
    ID3D11HullShader* Get()
    {
        return m_HullShader.Get();
    }
    void Reset()
    {
        m_HullShader.Reset();
    }

private:
    ComPtr<ID3D11HullShader> m_HullShader{};
};

class DomainShader : public IShaderResource
{
public:
    DomainShader() = default;
    DomainShader(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources,
        const std::string_view& name, ID3DBlob* blob) :
        IShaderResource(deviceResources, name, blob)
    {
    };
    ~DomainShader() override = default;
    void Compile() override
    {
        if (m_bCompiled)
            return;
        DirectX11::ThrowIfFailed(
            m_d3dDevice->CreateDomainShader(
                m_Blob->GetBufferPointer(),
                m_Blob->GetBufferSize(),
                nullptr,
                m_DomainShader.ReleaseAndGetAddressOf()
            )
        );
        m_bCompiled = true;
    }
    ID3D11DomainShader* Get()
    {
        return m_DomainShader.Get();
    }
    void Reset()
    {
        m_DomainShader.Reset();
    }

private:
    ComPtr<ID3D11DomainShader> m_DomainShader{};
};

class GeometryShader : public IShaderResource
{
public:
    GeometryShader() = default;
    GeometryShader(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources,
        const std::string_view& name, ID3DBlob* blob) :
        IShaderResource(deviceResources, name, blob)
    {
    };
    ~GeometryShader() override = default;
    void Compile() override
    {
        if (m_bCompiled)
            return;
        DirectX11::ThrowIfFailed(
            m_d3dDevice->CreateGeometryShader(
                m_Blob->GetBufferPointer(),
                m_Blob->GetBufferSize(),
                nullptr,
                m_GeometryShader.ReleaseAndGetAddressOf()
            )
        );
        m_bCompiled = true;
    }
    ID3D11GeometryShader* Get()
    {
        return m_GeometryShader.Get();
    }
    void Reset()
    {
        m_GeometryShader.Reset();
    }

private:
    ComPtr<ID3D11GeometryShader> m_GeometryShader{};
};

class PixelShader : public IShaderResource
{
public:
    PixelShader() = default;
    PixelShader(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources,
        const std::string_view& name, ID3DBlob* blob) :
        IShaderResource(deviceResources, name, blob)
    {
    };
    ~PixelShader() override = default;
    void Compile() override
    {
        if (m_bCompiled)
            return;
        DirectX11::ThrowIfFailed(
            m_d3dDevice->CreatePixelShader(
                m_Blob->GetBufferPointer(),
                m_Blob->GetBufferSize(),
                nullptr,
                m_PixelShader.ReleaseAndGetAddressOf()
            )
        );
        m_bCompiled = true;
    }
    ID3D11PixelShader* Get()
    {
        return m_PixelShader.Get();
    }
    void Reset()
    {
        m_PixelShader.Reset();
    }

private:
    ComPtr<ID3D11PixelShader> m_PixelShader{};
};


class ComputeShader : public IShaderResource
{
public:
    ComputeShader() = default;
    ComputeShader(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources,
        const std::string_view& name, ID3DBlob* blob) :
        IShaderResource(deviceResources, name, blob)
    {
    };
    ~ComputeShader() override = default;
    void Compile() override
    {
        if (m_bCompiled)
            return;
        DirectX11::ThrowIfFailed(
            m_d3dDevice->CreateComputeShader(
                m_Blob->GetBufferPointer(),
                m_Blob->GetBufferSize(),
                nullptr,
                m_ComputeShader.ReleaseAndGetAddressOf()
            )
        );
        m_bCompiled = true;
    }
    ID3D11ComputeShader* Get()
    {
        return m_ComputeShader.Get();
    }
    void Reset()
    {
        m_ComputeShader.Reset();
    }

private:
    ComPtr<ID3D11ComputeShader> m_ComputeShader{};
};

