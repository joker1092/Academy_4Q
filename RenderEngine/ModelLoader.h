#pragma once
#include "Core.Minimal.h"
#include "Mesh.h"
#include "Material.h"
#include "Model.h"
#include "TextureLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h> 
#include <assimp/material.h>

class ModelLoader
{
public:
	static void	LoadFromFile(const file::path& path, const file::path& dir, std::shared_ptr<Model>* model, std::shared_ptr<AnimModel>* animmodel);

private:
	struct MatOverrides
	{
		inline MatOverrides() {};
		inline MatOverrides(const std::string& _name, float _roughness, float _metalness) : name(_name), roughness(_roughness), metalness(_metalness), enabled(1) {}
		bool enabled = true;
		std::string name{};
		float roughness{};
		float metalness{};
	};

	static std::shared_ptr<Model> LoadModel(const std::string& name, const aiScene*	scene, const file::path& path, const file::path& dir);
	static std::shared_ptr<AnimModel> LoadAnimatedModel(const std::string& name, const aiScene*	scene, const file::path& path, const file::path& dir);
	static void	LoadMeshes(const aiScene* scene, const file::path&	dir, std::vector<Mesh>* meshes);
	static std::shared_ptr<Material> CreateMaterial(const file::path& dir, aiMesh* aimesh, const aiScene* scene);

};
