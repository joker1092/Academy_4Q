#pragma once
#include "Mesh.h"

class Model
{
public:
	Model() = default;
	~Model() = default;

	std::string				name{};
	std::vector<Mesh>		meshes{};
	bool					isLoaded = false;
};

class AnimModel
{
public:
	std::string				name;
	std::vector<AnimMesh>	meshes;

	DirectX::XMMATRIX GetMatrix()
	{
		return DirectX::XMMatrixIdentity();
	}
};
