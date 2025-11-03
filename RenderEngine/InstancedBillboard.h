#pragma once
#include "Billboards.h"

struct InstancedBillboard
{
	uint32 InstancedID{};
	std::shared_ptr<Billboard> _billboard;
	std::string _instancedName;
	bool _isVisible = true;
	bool _isDestroyMark = false;

	float3 Center{ 0.f, 0.f, 0.f };
	float Size = 10.f;
	uint32 currentTexture = 0;

	InstancedBillboard() = default;
	InstancedBillboard(const std::shared_ptr<Billboard>& billboard) : _billboard(billboard) {}
	~InstancedBillboard() = default;
};