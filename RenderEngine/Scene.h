#pragma once
#include "Core.Minimal.h"

struct Scene
{
	Mathf::Vector3 SunColor{};
	Mathf::Vector3 SunPos{};
	Mathf::Vector3 IBLColor{};
	float IBLIntensity{};
	bool bMoreShadowSamplers{};
};