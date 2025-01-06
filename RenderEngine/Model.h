#pragma once
#include "Core.Minimal.h"
#include "Mesh.h"

class Model
{
public:
	Model() : position(Mathf::xVectorZero), rotation(Mathf::xVectorZero), scale(Mathf::xVectorOne) {}
	~Model() = default;

	std::string name{};
	std::vector<Mesh> meshes{};

	Mathf::xVector position{};
	Mathf::xVector rotation{};
	Mathf::xVector scale{};

	Mathf::xMatrix GetMatrix() const
	{
		return XMMatrixScalingFromVector(scale) * 
			XMMatrixRotationQuaternion(rotation) *
			XMMatrixTranslationFromVector(position);
	}
};