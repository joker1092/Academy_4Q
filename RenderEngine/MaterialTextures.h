#pragma once
#include "Texture.h"

struct MaterialTextures 
{
	Texture2D	diffuse;	// 1U << 0
	Texture2D	occlusionMetallicRoughness;	// 1U << 1
	Texture2D	emissive;	// 1U << 2
	Texture2D	normal;		// 1U << 3
};
