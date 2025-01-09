#include "TextureLoader.h"
#include "LogicalDevice.h"

Texture2D TextureLoader::LoadFromFile(const file::path& filepath)
{
	if (filepath.extension() == ".dds")
	{
		return LoadDDSFromFile(filepath);
	}
	else if (filepath.extension() == ".png")
	{
		return LoadPNGFromFile(filepath);
	}
	else if (filepath.extension() == ".jpg")
	{
		return LoadPNGFromFile(filepath);
	}
	else 
	{
		WARN(std::string("Unrecognized extension \"") + filepath.extension().string() + "\" for Texture2D");
		return Texture2D();
	}
}

Texture2D TextureLoader::LoadCubemapFromFile(const file::path& filepath)
{
	if (filepath.extension() == ".dds")
	{
		return LoadCubemapDDSFromFile(filepath);
	}
	else
	{
		WARN(std::string("Unrecognized Cubemap extension \"") + filepath.extension().string() + "\" for Texture2D");
		return Texture2D();
	}
}

// Expects that mips are already in dds, so thread safe
Texture2D TextureLoader::LoadDDSFromFile(const file::path& filepath)
{
	if (!file::exists(filepath))
	{
		WARN("File does not exist \"" + filepath.string() + "\"");
		return Texture2D();
	}

	file::path file = filepath.filename();
	std::string filename = file.replace_extension().string();

	if (m_textures.find(filename) != m_textures.end())
	{
		return m_textures[filename];
	}

	ComPtr<ID3D11Resource> res;
	ComPtr<ID3D11ShaderResourceView> srv;

	DirectX11::ThrowIfFailed(DirectX::CreateDDSTextureFromFileEx(
		DX::States::Device,
		filepath.wstring().c_str(),
		0,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE,
		0,
		0,
        DDS_LOADER_FORCE_SRGB,
		res.ReleaseAndGetAddressOf(),
		srv.ReleaseAndGetAddressOf(),
		nullptr
	));

	Texture2D texture = CreateTexture(filename, res, srv);
	m_textures[filename] = texture;

	return texture;
}

Texture2D TextureLoader::LoadPNGFromFile(const file::path& filepath)
{
	if (!file::exists(filepath))
	{
		WARN("File does not exist \"" + filepath.string() + "\"");
		return Texture2D();
	}

	file::path file = filepath.filename();
	std::string filename = file.replace_extension().string();

	if (m_textures.find(filename) != m_textures.end())
	{
		return m_textures[filename];
	}

	ComPtr<ID3D11Resource> res;
	ComPtr<ID3D11ShaderResourceView> srv;

	DirectX11::ThrowIfFailed(DirectX::CreateWICTextureFromFileEx(
		DX::States::Device,
		nullptr,
		filepath.wstring().c_str(),
		0,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE,
		0,
		0,
        WIC_LOADER_FORCE_SRGB,	// WIC_LOADER_FORCE_SRGB No actual conversion takes, just renames to correct format
		res.ReleaseAndGetAddressOf(),
		srv.ReleaseAndGetAddressOf()
	));

	Texture2D texture = CreateTexture(filename, res, srv);
	m_textures[filename] = texture;

	return texture;
}

Texture2D TextureLoader::LoadCubemapDDSFromFile(const file::path& filepath)
{
	if (!file::exists(filepath))
	{
		WARN("File does not exist \"" + filepath.string() + "\"");
		return Texture2D();
	}

	file::path file = filepath.filename();
	std::string filename = file.replace_extension().string();

	if (m_textures.find(filename) != m_textures.end())
	{
		return m_textures[filename];
	}

	ComPtr<ID3D11Resource> res;
	ComPtr<ID3D11ShaderResourceView> srv;

	DirectX11::ThrowIfFailed(DirectX::CreateDDSTextureFromFileEx(
		DX::States::Device,
		filepath.wstring().c_str(),
		0,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE,
		0,
		D3D11_RESOURCE_MISC_TEXTURECUBE,
        DDS_LOADER_DEFAULT,
		res.ReleaseAndGetAddressOf(),
		srv.ReleaseAndGetAddressOf(),
		nullptr
	));

	Texture2D texture = CreateTexture(filename, res, srv);
	m_textures[filename] = texture;

	return texture;
}

Texture2D TextureLoader::CreateTexture(const std::string& name, const ComPtr<ID3D11Resource>& res, const ComPtr<ID3D11ShaderResourceView>& srv)
{
	D3D11_RESOURCE_DIMENSION dim = D3D11_RESOURCE_DIMENSION_UNKNOWN;
	res->GetType(&dim);

	switch (dim) {
	case(D3D11_RESOURCE_DIMENSION_TEXTURE2D):
	{
		ComPtr<ID3D11Texture2D> tex2d;
		DirectX11::ThrowIfFailed(res.As(&tex2d));
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		srv->GetDesc(&desc);

		return Texture2D(name, tex2d, srv);
	}
	default:
	{
		WARN("Unknown texuture dimension, expected 2D \"" + name + "\"");
	}
	}

	// Empty
	return Texture2D();
}
