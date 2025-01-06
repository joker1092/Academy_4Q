#pragma once
#include "Core.Minimal.h"
#include "Mesh.h"
#include "Buffer.h"
#include <directxtk/Geometry.h>

using namespace DirectX::DX11;

class Primitive
{
public:
	inline Primitive() {};
	inline Primitive(const std::string& _name) : name(_name) {};
	virtual ~Primitive() = default;

	std::string					name;
	std::shared_ptr<Material>	material;
	Buffer<UINT16>				bindex;
	Buffer<DirectX::VertexPositionNormalTexture> bvertex;

};


class Sphere : public Primitive
{
public:
	inline Sphere(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources, 
		const std::string& _name, float diameter, UINT tessalation = 10)
		: Primitive(_name)
	{
		DirectX::ComputeSphere(vertices, indices, diameter, tessalation, 1, 0);

		bindex = Buffer<uint16>(
			deviceResources,
			indices,
			D3D11_BIND_INDEX_BUFFER,
			D3D11_USAGE_DEFAULT,
			0
		);

		bvertex = Buffer<DirectX::VertexPositionNormalTexture>(
			deviceResources,
			vertices,
			D3D11_BIND_VERTEX_BUFFER,
			D3D11_USAGE_DEFAULT,
			0
		);

		material = std::make_shared<Material>(_name + "mat");
	}

	DirectX::IndexCollection indices;
	DirectX::VertexCollection vertices;


};

class Cube : public Primitive
{
public:
	inline Cube(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources, float size = 1.0f)
	{
		DirectX::ComputeBox(
			vertices,
			indices,
			DirectX::XMFLOAT3{ size, size, size },
			0,
			0
		);

		this->bindex = Buffer<uint16>(
			deviceResources,
			indices,
			D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER,
			D3D11_USAGE::D3D11_USAGE_IMMUTABLE,
			0
		);

		this->bvertex = Buffer<DirectX::VertexPositionNormalTexture>(
			deviceResources,
			vertices,
			D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
			D3D11_USAGE::D3D11_USAGE_IMMUTABLE,
			0
		);

	}
	DirectX::IndexCollection indices;
	DirectX::VertexCollection vertices;
};