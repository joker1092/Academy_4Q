#pragma once
#include "Core.Minimal.h"
#include "Primitives.h"
#include "../Physics/Physx.h"

using namespace physx;

struct InstancedDebugBox
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT4 color;
	std::shared_ptr<Cube> cube;

	InstancedDebugBox(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& scl, const DirectX::XMFLOAT4& col, const std::shared_ptr<Cube>& pCube)
		: position(pos), scale(scl), color(col), cube(pCube)
	{
		cube->material->properties.diffuse = { col.x, col.y, col.z };
	}
};

class PXDebugSystem final : public Singleton<PXDebugSystem>
{
private:
	friend class Singleton;

	PXDebugSystem() = default;
	~PXDebugSystem() = default;

public:
	void CollectShapes();
	std::vector<std::shared_ptr<InstancedDebugBox>>& GetPrimitives() { return _primitives; }
	void ClearShapes() { shapes.clear(); }
	void ClearPrimitives() { _primitives.clear(); }

private:
	PxScene* physicsScene{};
	std::vector<BoxShape> shapes;
	std::vector<std::shared_ptr<InstancedDebugBox>> _primitives;
};

inline static auto& PXDebugSystems = PXDebugSystem::GetInstance();