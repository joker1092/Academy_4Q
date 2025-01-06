#pragma once
#include "Core.Minimal.h"
#include "TextureLoader.h"
#include "Model.h"
#include "Material.h"

#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/pbrmaterial.h>

class ModelLoader
{
public:
    static void LoadFormFile(const file::path& path, const file::path& dir,
    _inout_opt Model** ppModel, _inout_opt AnimModel** ppAnimModel);

private:

    struct MatOverrides
    {
        MatOverrides() = default;
        MatOverrides(const std::string& _name, float _roughness, float _metalness) : name(_name), roughness(_roughness), metalness(_metalness), enabled(1) {}
        bool enabled{};
        std::string name{};
        float roughness{};
        float metalness{};
    };

    static Model* LoadModel(const std::string_view& name, const aiScene* scene,
        const file::path& path, const file::path& dir);

    static AnimModel* LoadAnimModel(const std::string_view& name, const aiScene* scene,
        const file::path& path, const file::path& dir);

    static void LoadMeshes(const aiScene* scene, const file::path& dir, std::vector<Mesh>* meshes);

    static Material* CreateMaterial(const file::path& fir, aiMesh* mesh, const aiScene* scene);
};
