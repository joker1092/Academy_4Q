#pragma once
#include "Core.Minimal.h"
#include "DXStates.h"

alstruct BillboardVertex
{
	float3 Position;
};

alstruct BillboardSize
{
	float Size;
};

struct Billboard
{

	std::vector<ComPtr<ID3D11ShaderResourceView>> Textures;
	BillboardVertex CenterVertex{};
	BillboardSize SizeBuffer{};
	Buffer<BillboardVertex> bvertex;
	Buffer<BillboardSize> bsize;
	std::string name{};

	Billboard() = default;
	void AddTexture(const ComPtr<ID3D11ShaderResourceView>& texture)
	{
		Textures.push_back(texture);
	}
	void CreateBuffers()
	{
		bvertex = Buffer<BillboardVertex>(
			D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
			D3D11_USAGE::D3D11_USAGE_DYNAMIC,
			D3D11_CPU_ACCESS_WRITE
		);

		bsize = Buffer<BillboardSize>(
			D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
			D3D11_USAGE::D3D11_USAGE_DYNAMIC,
			D3D11_CPU_ACCESS_WRITE
		);
	}

	~Billboard() = default;

};
