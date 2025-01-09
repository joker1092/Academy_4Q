#pragma once
#include "Mesh.h"

class Model
{
public:
	Model() = default;
	~Model() = default;

	std::string				name{};
	std::vector<Mesh>		meshes{};

	DirectX::XMVECTOR		position{ Mathf::xVectorZero };
	DirectX::XMVECTOR		rotation{ DirectX::XMQuaternionIdentity() };
	DirectX::XMVECTOR		scale{ 1.f, 1.f, 1.f, 0.f };

	DirectX::XMMATRIX GetMatrix() const
	{
		DirectX::XMMATRIX trans = DirectX::XMMatrixScalingFromVector(scale);
		trans *= DirectX::XMMatrixTranslationFromVector(position);
		trans *= DirectX::XMMatrixRotationQuaternion(rotation);
		return trans;
	}
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
