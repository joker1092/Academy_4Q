#include "TextureLoader.h"

ID3D11Device* TextureLoader::m_d3dDevice = nullptr;

void TextureLoader::Initialize(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources)
{
	m_d3dDevice = deviceResources->GetD3DDevice();
}

Texture2D TextureLoader::LoadTextureFromFile(const std::string_view& filename)
{
	if (nullptr == m_d3dDevice)
	{
		OutputDebugString(L"TextureLoader::LoadTextureFromFile: m_d3dDevice is nullptr\n");
		return Texture2D();
	}

	ComPtr<ID3D11Resource> resource{};
	ComPtr<ID3D11ShaderResourceView> srv{};
	file::path path(filename);

	HRESULT hr = DirectX::CreateTextureFromFile(m_d3dDevice, path, resource.GetAddressOf(), srv.GetAddressOf());
	if (FAILED(hr))
	{
		OutputDebugString(L"TextureLoader::LoadTextureFromFile: CreateTextureFromFile failed\n");
		assert(false);
		return Texture2D();
	}

	return CreateTexture(filename, resource.Get(), srv.Get());
}

Texture2D TextureLoader::CreateTexture(const std::string_view& name, ID3D11Resource* resource, ID3D11ShaderResourceView* srv)
{
	ComPtr<ID3D11Texture2D> texture{};
	DirectX11::ThrowIfFailed(
		resource->QueryInterface(IID_PPV_ARGS(texture.GetAddressOf()))
	);

	return Texture2D(name, texture.Get(), srv);
}
