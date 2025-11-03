#include "PXDebugSystem.h"
#include <physx/PxRigidActor.h>

void PXDebugSystem::CollectShapes()
{
	Physics->GetShapes(shapes);

	foreach(shapes, [this](auto&& shape)
	{
		_primitives.push_back(std::make_shared<InstancedDebugBox>(
			float3(shape.worldPosition),
			float3{ 1, 1, 1 },
			float4{ 1, 0, 0, 1},
			std::make_shared<Cube>(shape.halfSize[0] * 2.f)
		));
	});

}
