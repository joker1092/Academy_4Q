#pragma once
#include "Texture.h"

class TextureLoader
{
public:
	static Texture2D LoadFromFile(const file::path& filepath);
	static Texture2D LoadCubemapFromFile(const file::path& filepath);

private:
	static Texture2D LoadDDSFromFile(const file::path& filepath);
	unsafe static Texture2D LoadPNGFromFile(const file::path& filepath);
	static Texture2D LoadCubemapDDSFromFile(const file::path& filepath);
	static Texture2D CreateTexture(const std::string& name, const ComPtr<ID3D11Resource>& res, const ComPtr<ID3D11ShaderResourceView>& srv);

private:
	static inline std::unordered_map<std::string, Texture2D> m_textures;
};
