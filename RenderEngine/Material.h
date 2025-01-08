#pragma once
#include "Buffers.h"
#include "MaterialTextures.h"

struct Material
{
	inline Material(const std::string& _name = "") : name(_name) { };
	inline ~Material() {};

	// No copy
	Material(const Material&) = delete;
	Material& operator=(const Material&) = delete;

	std::string			name;
	MaterialProperties	properties;
	MaterialTextures	textures;

};
