#pragma once
#include "Material.h"

class MaterialLoader
{
public:
	static void LoadFromPath(std::shared_ptr<Material>& mat, const std::string& key, const file::path& filepath);
};