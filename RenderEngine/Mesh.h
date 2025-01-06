#pragma once
#include "Core.Minimal.h"
#include "Buffer.h"
#include "BufferType.h"
#include "Material.h"

using Index = uint32;

class Mesh
{
public:
	Mesh() = default;
	Mesh(const std::string_view& name, const std::vector<Index>& indices, const std::vector<Vertex>& vertices) :
		m_Name(name), m_Indices(indices), m_Vertices(vertices)
	{
	}
	Mesh(const std::string_view& name, 
		const std::vector<Index>& indices, 
		const std::vector<Vertex>& vertices, Material* material) :
		m_Name(name), m_Indices(indices), m_Vertices(vertices), m_Material(material)
	{
	}
	~Mesh() = default;

	Mesh(Mesh&& mesh) noexcept : 
		m_Name(std::move(mesh.m_Name)),
		m_Indices(std::move(mesh.m_Indices)),
		m_Vertices(std::move(mesh.m_Vertices)),
		m_Material(std::move(mesh.m_Material))
	{
	}
	Mesh& operator=(Mesh&&) noexcept = default;

	Mesh& operator=(const Mesh&) = delete;
	Mesh(const Mesh&) = delete;

	void CreateBuffer()
	{
		m_IndexBuffer = Buffer<Index>{
			m_Indices,
			D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER,
			D3D11_USAGE::D3D11_USAGE_IMMUTABLE,
			NULL
		};

		m_VertexBuffer = Buffer<Vertex>{
			m_Vertices,
			D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
			D3D11_USAGE::D3D11_USAGE_IMMUTABLE,
			NULL
		};
		m_IndexBuffer.SetName(m_Name + " Index Buffer");
		m_VertexBuffer.SetName(m_Name + " Vertex Buffer");
	}

	std::vector<Index> m_Indices;
	std::vector<Vertex> m_Vertices;

	Buffer<Index> m_IndexBuffer;
	Buffer<Vertex> m_VertexBuffer;

	Material* m_Material = nullptr;
	std::string m_Name;
};