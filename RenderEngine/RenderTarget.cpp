#include "RenderTarget.h"

RenderTarget::RenderTarget(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources, ID3D11Texture2D* texture, DXGI_FORMAT format, uint32 width, uint32 height)
	: m_Device(deviceResources->GetD3DDevice()), m_Texture(texture), format(format), width(width), height(height)
{
	CreateTargets();
}

RenderTarget::RenderTarget(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources, DXGI_FORMAT format, uint32 width, uint32 height)
	: m_Device(deviceResources->GetD3DDevice()), format(format), width(width), height(height)
{
	CreateTexture();
	CreateTargets();
	CreateShaderResourceView();
}

RenderTarget::RenderTarget(ID3D11Device* device, DXGI_FORMAT format, uint32 width, uint32 height) :
	m_Device(device), format(format), width(width), height(height)
{
	CreateTexture();
	CreateTargets();
	CreateShaderResourceView();
}

RenderTarget::~RenderTarget()
{
	DestroyTargets();
}

void RenderTarget::Resize(uint32 width, uint32 height)
{
	if (this->width == width && this->height == height)
	{
		return;
	}

	this->width = width;
	this->height = height;

	DestroyTargets();
	if (m_bHasSRV)
	{
		CreateTexture();
	}

	CreateTargets();

	if (m_bHasSRV)
	{
		CreateShaderResourceView();
	}
}

void RenderTarget::ClearView()
{
	m_DeviceContext->ClearRenderTargetView(m_RTV.Get(), color);
	m_DeviceContext->ClearDepthStencilView(m_DSV.Get(), D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH | D3D11_CLEAR_FLAG::D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void RenderTarget::CreateTexture()
{
	CD3D11_TEXTURE2D_DESC desc{
		format,
		width,
		height,
		1,
		1,
		D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE
	};

	DirectX11::ThrowIfFailed(
		m_Device->CreateTexture2D(&desc, nullptr, m_Texture.ReleaseAndGetAddressOf())
	);
}

void RenderTarget::CreateTargets()
{
	CD3D11_RENDER_TARGET_VIEW_DESC rtvDesc{
		D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2D,
		format
	};

	DirectX11::ThrowIfFailed(
		m_Device->CreateRenderTargetView(m_Texture.Get(), &rtvDesc, m_RTV.ReleaseAndGetAddressOf())
	);

	CD3D11_TEXTURE2D_DESC desc{
		DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT,
		width,
		height,
		1,
		1,
		D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL
	};

	DirectX11::ThrowIfFailed(
		m_Device->CreateTexture2D(&desc, nullptr, m_Stencil.ReleaseAndGetAddressOf())
	);

	CD3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{	D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D };

	DirectX11::ThrowIfFailed(
		m_Device->CreateDepthStencilView(m_Stencil.Get(), &dsvDesc, m_DSV.ReleaseAndGetAddressOf())
	);
}

void RenderTarget::CreateShaderResourceView()
{
	if (!m_bHasSRV)
	{
		return;
	}

	DirectX11::ThrowIfFailed(
		m_Device->CreateShaderResourceView(m_Texture.Get(), nullptr, m_SRV.ReleaseAndGetAddressOf())
	);
}

void RenderTarget::DestroyTargets()
{
	m_RTV.Reset();
	m_DSV.Reset();

	m_Stencil.Reset();
	m_Texture.Reset();
	
	m_SRV.Reset();
}
