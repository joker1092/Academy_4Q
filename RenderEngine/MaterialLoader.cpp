#include "MaterialLoader.h"
#include "TextureLoader.h"

void MaterialLoader::LoadFromPath(std::shared_ptr<Material>& mat, const std::string& key, const file::path& filepath)
{
	std::string matOverrideName = key + ".mat";
	file::path overridepath = filepath / matOverrideName;
	if (file::exists(overridepath))
	{
		json j;
		std::ifstream in(overridepath);
		in >> j;

		mat->name = j["Name"].get<std::string>();
		flag bit = j["Bitmask"].get<uint32>();
		BitFlag flag = bit;

		// Diffuse
		{
			std::string name;
			if (j.contains("BaseColor"))
			{
				name = j["BaseColor"].get<std::string>();
			}
			file::path path = filepath / name;
			if (1 < name.length() && file::exists(filepath))
			{
				mat->textures.diffuse = TextureLoader::LoadFromFile(path, sRGBSettings::NO_SRGB);
				mat->properties.bitmask |= DIFFUSE_BIT;
			}
		}

		{
			std::string name;
			if (j.contains("OcclusionRoughnessMetallic"))
			{
				name = j["OcclusionRoughnessMetallic"].get<std::string>();
			}

			file::path path = filepath / name;
			if (1 < name.length() && file::exists(filepath))
			{
				mat->textures.occlusionMetallicRoughness = TextureLoader::LoadFromFile(path, sRGBSettings::NO_SRGB);
				mat->properties.bitmask |= OCCLUSIONMETALLICROUGHNESS_BIT;
			}
			else if (j.contains("Metalness") && j.contains("Roughness"))
			{
				float metalness = j["Metalness"].get<float>();
				float roughness = j["Roughness"].get<float>();
				mat->properties.metalness = metalness;
				mat->properties.roughness = roughness;
			}

		}

		{
			std::string name;
			if (j.contains("Normal"))
			{
				name = j["Normal"].get<std::string>();
			}
			file::path path = filepath / name;
			if (1 < name.length() && file::exists(filepath))
			{
				mat->textures.normal = TextureLoader::LoadFromFile(path, sRGBSettings::NO_SRGB);
				mat->properties.bitmask |= NORMAL_BIT;
			}
		}
	}
	else
	{
		json j;
		std::ofstream out(overridepath);
		j["Name"] = mat->name;
		{
			file::path path{};
			std::string name = mat->name;
			name += "_BaseColor.png";
			path = filepath / name;
			if (file::exists(path))
			{
				mat->textures.diffuse = TextureLoader::LoadFromFile(path, sRGBSettings::NO_SRGB);
				j["BaseColor"] = name;
				mat->properties.bitmask |= DIFFUSE_BIT;
			}
		}

		{
			file::path path{};
			std::string name = mat->name;
			name += "_OcclusionRoughnessMetallic.png";
			path = filepath / name;
			if (file::exists(path))
			{
				mat->textures.occlusionMetallicRoughness = TextureLoader::LoadFromFile(path, sRGBSettings::NO_SRGB);
				j["OcclusionRoughnessMetallic"] = name;
				mat->properties.bitmask |= OCCLUSIONMETALLICROUGHNESS_BIT;
			}
		}

		{
			file::path path{};
			std::string name = mat->name;
			name += "_Normal.png";
			path = filepath / name;
			if (file::exists(path))
			{
				mat->textures.normal = TextureLoader::LoadFromFile(path, sRGBSettings::NO_SRGB);
				j["Normal"] = name;
				mat->properties.bitmask |= NORMAL_BIT;
			}
		}

		j["Bitmask"] = mat->properties.bitmask;

		out << j.dump(4);
	}
}
