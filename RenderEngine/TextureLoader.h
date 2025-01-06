#pragma once
#include "Core.Minimal.h"
#include "DeviceResources.h"
#include "Texture.h"

class TextureLoader
{
public:
	TextureLoader() = default;
	~TextureLoader() = default;

	static void Initialize(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources);

	static Texture2D LoadTextureFromFile(const std::string_view& filename);

private:
	static Texture2D CreateTexture(const std::string_view& name, ID3D11Resource* resource, ID3D11ShaderResourceView* srv);

	static ID3D11Device* m_d3dDevice;
};