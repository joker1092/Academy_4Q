#pragma once
#include "TypeDefinition.h"
#include "Buffers.h"

struct PointLight
{
	PointLightBuffer buffer{};
	uint32 ID = 0;
};