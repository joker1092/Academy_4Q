#pragma once
#include "Model.h"
#include <queue>

namespace InstancedModelID
{
	static inline uint32 globalID = 0;
	static inline std::queue<uint32> freeIDs;
	static inline uint32 GetFreeID()
	{
		if (2500u == globalID)
		{
			WARN("InstancedModelID::GetFreeID() : globalID reached MAX(2500)");
			//std::terminate();
			if (freeIDs.empty())
			{
				return globalID++;
			}
		}
		else
		{
			if (freeIDs.empty())
			{
				return globalID++;
			}
		}

		uint32 id = freeIDs.front();
		freeIDs.pop();
		return id;
	}
}

struct InstancedModel
{
	uint32 instancedID{};
	std::shared_ptr<Model> model;
	std::shared_ptr<AnimModel> animModel;

	DirectX::XMVECTOR		position{ Mathf::xVectorZero };
	DirectX::XMVECTOR		rotation{ DirectX::XMQuaternionIdentity() };
	DirectX::XMVECTOR		scale{ 1.f, 1.f, 1.f, 0.f };

	float rotationX = 0.f;
	float rotationY = 0.f;
	float rotationZ = 0.f;

	void SetRotation(float x, float y, float z)
	{
		rotationX = x;
		rotationY = y;
		rotationZ = z;
		rotation = DirectX::XMQuaternionRotationRollPitchYaw(x, y, z);
	}

	DirectX::XMMATRIX GetMatrix() const
	{
		DirectX::XMMATRIX trans = DirectX::XMMatrixScalingFromVector(scale);
		trans *= DirectX::XMMatrixRotationQuaternion(rotation);
		trans *= DirectX::XMMatrixTranslationFromVector(position);
		return trans;
	}

	inline InstancedModel() : 
		instancedID(InstancedModelID::globalID++)
	{
	};
	inline InstancedModel(const std::shared_ptr<Model>& _model) : 
		instancedID(InstancedModelID::globalID++),
		model(_model) 
	{
	};
	inline InstancedModel(const std::shared_ptr<Model>& _model,
		DirectX::XMVECTOR _pos, DirectX::XMVECTOR _rot, DirectX::XMVECTOR _scale) :
		instancedID(InstancedModelID::globalID++),
		model(_model),
		position(_pos),
		rotation(_rot),
		scale(_scale)
	{
	};
	inline InstancedModel(const std::shared_ptr<AnimModel>& _model) :
		instancedID(InstancedModelID::globalID++),
		animModel(_model)
	{
	};
	inline InstancedModel(const std::shared_ptr<AnimModel>& _model,
		DirectX::XMVECTOR _pos, DirectX::XMVECTOR _rot, DirectX::XMVECTOR _scale) :
		instancedID(InstancedModelID::globalID++),
		animModel(_model),
		position(_pos),
		rotation(_rot),
		scale(_scale)
	{
	};
	~InstancedModel() { InstancedModelID::freeIDs.push(instancedID); }
};
