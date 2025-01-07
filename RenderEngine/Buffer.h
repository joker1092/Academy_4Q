#pragma once
#include "Core.Minimal.h"
#include "DeviceResources.h"

interface IBuffer
{
    IBuffer() = default;
    IBuffer(uint32 access, D3D11_USAGE usage, D3D11_BIND_FLAG flags) : 
        m_Access(access), 
        m_Usage(usage), 
        m_Flags(flags),
		m_Size(1)
    {}
    virtual ~IBuffer() = default;

    ComPtr<ID3D11Buffer> m_Buffer{};
    uint32 m_Access{};
    uint32 m_Size{};
    D3D11_USAGE m_Usage{};
    D3D11_BIND_FLAG m_Flags{};

    void SetName(const std::string_view& name) const
    {
        DirectX::SetName(m_Buffer.Get(), name);
    }

    D3D11_USAGE GetUsage() const
    {
        return m_Usage;
    }

    ID3D11Buffer* Get() const
    {
        return m_Buffer.Get();
    }

    ID3D11Buffer* const* GetAddressOf() const
    {
        return m_Buffer.GetAddressOf();
    }
};

template<typename T>
struct Buffer : public IBuffer
{
public:
    Buffer() = default;
    Buffer(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources, D3D11_BIND_FLAG flags, D3D11_USAGE usage = D3D11_USAGE::D3D11_USAGE_DEFAULT, uint32 cpuAccess = 0) :
        IBuffer(cpuAccess, usage, flags),
        m_pDevice(deviceResources->GetD3DDevice()),
        m_pContext(deviceResources->GetD3DDeviceContext())
    {
		m_Size = 1;
        Create();
    }

	Buffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, D3D11_BIND_FLAG flags, D3D11_USAGE usage = D3D11_USAGE::D3D11_USAGE_DEFAULT, uint32 cpuAccess = 0) :
		IBuffer(cpuAccess, usage, flags),
		m_pDevice(pDevice),
		m_pContext(pContext)
	{
		m_Size = 1;
		Create();
	}

    Buffer(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources, const std::vector<T>& initializeData, D3D11_BIND_FLAG flags, D3D11_USAGE usage = D3D11_USAGE::D3D11_USAGE_DEFAULT, uint32 cpuAccess = 0) :
        IBuffer(cpuAccess, usage, flags),
        m_pDevice(deviceResources->GetD3DDevice()),
        m_pContext(deviceResources->GetD3DDeviceContext())
    {
		m_Size = static_cast<uint32>(initializeData.size());
        Create(initializeData);
    }

	Buffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const std::vector<T>& initializeData, D3D11_BIND_FLAG flags, D3D11_USAGE usage = D3D11_USAGE::D3D11_USAGE_DEFAULT, uint32 cpuAccess = 0) :
		IBuffer(cpuAccess, usage, flags),
		m_pDevice(pDevice),
		m_pContext(pContext)
	{
		m_Size = static_cast<uint32>(initializeData.size());
		Create(initializeData);
	}

    ~Buffer() override
    {
       Reset();
    }
    
    unsafe D3D11_MAPPED_SUBRESOURCE Map(D3D11_MAP mapType, uint32 subResource = 0)
    {
#if defined(_DEBUG)
        CORE_ASSERT(m_Usage != D3D11_USAGE::D3D11_USAGE_IMMUTABLE);
        CORE_ASSERT(nullptr != m_Buffer.Get());
        CORE_ASSERT(NULL != m_Access)
#endif
        D3D11_MAPPED_SUBRESOURCE mappedResource{};
        DirectX11::ThrowIfFailed(m_pContext->Map(m_Buffer.Get(), subResource, mapType, NULL, &mappedResource));
        return mappedResource;
    }

    unsafe void Unmap(uint32 subResource = 0)
    {
        m_pContext->Unmap(m_Buffer.Get(), subResource);
    }

    unsafe void MapFromStaging(const Buffer<T>& stagingBuffer)
    {
#if defined(_DEBUG)
        CORE_ASSERT(m_Buffer.GetUsage() == D3D11_USAGE::D3D11_USAGE_STAGING);
        CORE_ASSERT(m_Usage != D3D11_USAGE::D3D11_USAGE_IMMUTABLE);
#endif
        m_pContext->CopyResource(m_Buffer.Get(), stagingBuffer.m_Buffer.Get());
    }

    void Reset()
    {
        m_Buffer.Reset();
        m_Access = 0;
    }

    uint32 Stride() const
    {
        return sizeof(T);
    }

    uint32 Size() const
    {
        return m_Size;
    }

private:
    void Create(const std::vector<T>& initializeData)
    {
        CD3D11_BUFFER_DESC bufferDesc{
            static_cast<uint32>(sizeof(T) * m_Size),
            (uint32)m_Flags,
            m_Usage,
            m_Access,
        };

        D3D11_SUBRESOURCE_DATA initData{};
        initData.pSysMem = initializeData.data();

        DirectX11::ThrowIfFailed(m_pDevice->CreateBuffer(&bufferDesc, &initData, m_Buffer.GetAddressOf()));
    }

    void Create()
    {
        CD3D11_BUFFER_DESC bufferDesc{
            static_cast<uint32>(sizeof(T)),
            (uint32)m_Flags,
            m_Usage,
            m_Access,
        };

        DirectX11::ThrowIfFailed(m_pDevice->CreateBuffer(&bufferDesc, nullptr, m_Buffer.GetAddressOf()));
    }

private:
    ID3D11Device* m_pDevice{};
    ID3D11DeviceContext* m_pContext{};
};

