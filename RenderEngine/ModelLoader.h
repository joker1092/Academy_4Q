#pragma once
#include "Core.Minimal.h"
#include "Model.h"
#include "Mesh.h"
#include "Material.h"
#include "TextureLoader.h"
#include <unordered_map>

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
		std::string baseColor{};
		std::string normal{};
		std::string occlusionMetallicRoughness{};
		float roughness{};
		float metalness{};
		uint32 bitmask = 0;
	};
	static std::unordered_map<std::string, std::shared_ptr<Material>> Materials;

	static std::shared_ptr<Model> LoadModel(const std::string& name, const aiScene*	scene, const file::path& path, const file::path& dir);
	static std::shared_ptr<AnimModel> LoadAnimatedModel(const std::string& name, aiNode* node, const aiScene*	scene, const file::path& path, const file::path& dir);
	static void	LoadMeshes(const std::string_view& modelname, const aiScene* scene, const file::path&	dir, std::vector<Mesh>* meshes);
	static void ProcessNode(std::shared_ptr<AnimModel>* model, const std::string_view& modelname, aiNode* node, const aiScene* scene, const file::path& dir, std::vector<AnimMesh>* meshes);
	static void LoadMeshes(std::shared_ptr<AnimModel>* model, const std::string_view& modelname, aiNode* node, aiMesh* pMesh, const aiScene* scene, const file::path& dir, std::vector<AnimMesh>* meshes);
    static void LoadMeshes(std::shared_ptr<AnimModel>* model, const aiScene* scene, const file::path& dir, std::vector<AnimMesh>* meshes);
	static void LoadAnimations(std::shared_ptr<AnimModel>* model, const file::path& dir);
	static std::shared_ptr<Material> CreateMaterial(const std::string_view& modelname, const file::path& dir, aiMesh* aimesh, const aiScene* scene);
};
