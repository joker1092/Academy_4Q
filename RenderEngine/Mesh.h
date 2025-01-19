#pragma once
#include "Buffers.h"
#include "Buffer.h"
#include "Material.h"
#include "Animation.h"

class Mesh
{
public:
	Mesh() = default;
	Mesh(const std::string _name, const std::vector<Index>& _indices,const std::vector<Vertex>& _vertices)
		: name(_name), indices(_indices), vertices(_vertices)
	{};
	Mesh(const std::string _name, const std::vector<Index>& _indices, const std::vector<Vertex>& _vertices, const std::shared_ptr<Material>& _material)
		: name(_name), indices(_indices), vertices(_vertices), material(_material) 
	{};
	~Mesh() = default;

	// Move constructor
	Mesh(Mesh&& mesh) noexcept
		: name(mesh.name), indices(std::move(mesh.indices)), vertices(std::move(mesh.vertices)), material(mesh.material) {};
	Mesh& operator=(Mesh&& mesh) = default;
	Mesh& operator=(const Mesh&) = delete;
	Mesh(const Mesh&) = delete;

	void CreateBuffers() 
	{
		bindex = Buffer<Index>(
			indices,
			D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER,
			D3D11_USAGE::D3D11_USAGE_IMMUTABLE,
			NULL
		);
		bvertex = Buffer<Vertex>(
			vertices,
			D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
			D3D11_USAGE::D3D11_USAGE_IMMUTABLE,
			NULL
		);
		bindex.SetName(name + " Index Buffer");
		bvertex.SetName(name + " Vertex Buffer");
	}

	std::vector<Index>			indices;
	std::vector<Vertex>			vertices;

	Buffer<Index>				bindex;
	Buffer<Vertex>				bvertex;

	std::shared_ptr<Material>	material;
	std::string					name;
};

class AnimMesh
{
public:
	std::vector<Index>			indices;
	std::vector<AnimVertex>		vertices;

	Buffer<Index>				bindex;
	Buffer<AnimVertex>			bvertex;

	std::shared_ptr<Material>	material;
	std::shared_ptr<Animator>	animator;
	std::string				    name;

	AnimMesh() = default;
	AnimMesh(const std::string _name, const std::vector<Index>& _indices, const std::vector<AnimVertex>& _vertices)
		: name(_name), indices(_indices), vertices(_vertices)
	{
	};
	AnimMesh(const std::string _name, const std::vector<Index>& _indices, const std::vector<AnimVertex>& _vertices, const std::shared_ptr<Material>& _material, const std::shared_ptr<Animator>& animator)
		: name(_name), indices(_indices), vertices(_vertices), material(_material)
	{
	};

	// Move constructor
	AnimMesh(AnimMesh&& mesh) noexcept
		: name(mesh.name), indices(std::move(mesh.indices)), vertices(std::move(mesh.vertices)), material(mesh.material) {
	};
	AnimMesh& operator=(AnimMesh&& mesh) = default;
	AnimMesh& operator=(const AnimMesh&) = delete;
	AnimMesh(const AnimMesh&) = delete;

	void CreateBuffers()
	{
		bindex = Buffer<Index>(
			indices,
			D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER,
			D3D11_USAGE::D3D11_USAGE_IMMUTABLE,
			NULL
		);
		bvertex = Buffer<AnimVertex>(
			vertices,
			D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
			D3D11_USAGE::D3D11_USAGE_IMMUTABLE,
			NULL
		);
		bindex.SetName(name + " Index Buffer");
		bvertex.SetName(name + " Vertex Buffer");
	}
};
