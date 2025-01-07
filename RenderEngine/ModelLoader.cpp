#include "ModelLoader.h"
#include "DataSystem.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

std::shared_ptr<DirectX11::DeviceResources> ModelLoader::m_deviceResources{};

void ModelLoader::Initialize(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources)
{
	m_deviceResources = deviceResources;
}

void ModelLoader::LoadFormFile(const file::path& path, const file::path& dir, _inout_opt Model** ppModel, _inout_opt AnimModel** ppAnimModel)
{
	Assimp::Importer importer{};
	BitFlag flags{};

	flags |= aiProcess_FlipUVs;

	std::string name = file::path(path.filename()).replace_extension().string();

	const aiScene* scene = importer.ReadFile(path.string(), flags);
	if (!scene)
	{
		const char* errorMsg = importer.GetErrorString();
		Console.WriteLine("Error: {}", std::string(errorMsg));
		return;
	}

	const char* errorMsg = importer.GetErrorString();

	if (scene->mNumMeshes > 0)
	{
		if (!scene->mMeshes[0]->HasTangentsAndBitangents())
		{
			BitFlag postFlags{};
			postFlags |= aiProcess_CalcTangentSpace;

			scene = importer.ApplyPostProcessing(postFlags);
		}
	}

	if (scene->HasAnimations())
	{
		*ppAnimModel = LoadAnimModel(name, scene, path, dir);
	}
	else
	{
		*ppModel = LoadModel(name, scene, path, dir);
	}

}

Model* ModelLoader::LoadModel(const std::string_view& name, const aiScene* scene, const file::path& path, const file::path& dir)
{
	auto ptr = DataSystem::AllocateModel();
	ptr->name = name;
	LoadMeshes(scene, dir, &ptr->meshes);
	return ptr;
}

AnimModel* ModelLoader::LoadAnimModel(const std::string_view& name, const aiScene* scene, const file::path& path, const file::path& dir)
{
	return nullptr;
}

void ModelLoader::LoadMeshes(const aiScene* scene, const file::path& dir, std::vector<Mesh>* meshes)
{
	for (uint32 i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh* aimesh = scene->mMeshes[i];
		auto vertices = 
			std::views::iota(0U, aimesh->mNumVertices) |
			std::views::transform([aimesh](uint32 idx)
			{
				Vertex vertex{};
				vertex.position = 
				{ 
					aimesh->mVertices[idx].x, 
					aimesh->mVertices[idx].y, 
					aimesh->mVertices[idx].z 
				};

				if (aimesh->mNormals)
				{
					vertex.normal =
					{
						aimesh->mNormals[idx].x,
						aimesh->mNormals[idx].y,
						aimesh->mNormals[idx].z
					};
				}

				if (aimesh->mTangents)
				{
					vertex.tangent =
					{
						aimesh->mTangents[idx].x,
						aimesh->mTangents[idx].y,
						aimesh->mTangents[idx].z
					};
				}

				if (aimesh->mBitangents)
				{
					vertex.bitangent =
					{
						aimesh->mBitangents[idx].x,
						aimesh->mBitangents[idx].y,
						aimesh->mBitangents[idx].z
					};
				}

				if (aimesh->mTextureCoords[0])
				{
					vertex.texCoord =
					{
						aimesh->mTextureCoords[0][idx].x,
						aimesh->mTextureCoords[0][idx].y
					};
				}

				return vertex;
					
			}) | std::ranges::to<std::vector>();

		auto indices =
			std::views::iota(0U, aimesh->mNumFaces) |
			std::views::transform([aimesh](uint32 idx)
			{
				auto face = aimesh->mFaces[idx];
				return std::array<uint32, 3>
				{
					face.mIndices[0],
					face.mIndices[1],
					face.mIndices[2]
				};
			}) | std::ranges::to<std::vector>();

		std::vector<Index> flattenedIndices{};
		for (const auto& face : indices)
		{
			flattenedIndices.insert(flattenedIndices.end(), face.begin(), face.end());
		}

		std::string name = std::string(scene->mMeshes[i]->mName.C_Str());
		auto material = CreateMaterial(dir, aimesh, scene);

		meshes->emplace_back(name, flattenedIndices, vertices, material);
	}

	for (auto& mesh : *meshes)
	{
		mesh.CreateBuffer(m_deviceResources);
	}
}

Material* ModelLoader::CreateMaterial(const file::path& fir, aiMesh* mesh, const aiScene* scene)
{
	Material* material = DataSystem::AllocateMaterial();

	uint32 idx = mesh->mMaterialIndex;
	float fOut{};
	aiMaterial* aiMat = scene->mMaterials[idx];

	aiString name = aiMat->GetName();
	material->m_Name = std::string(name.C_Str());

	MatOverrides overrides{};

#pragma region BaseColor
	{
		aiReturn res = aiReturn_FAILURE;
		aiString path{};

		res = aiMat->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &path);
		if (res == aiReturn_SUCCESS)
		{
			file::path texPath = fir / path.C_Str();
			if (!file::exists(texPath))
			{
				texPath.replace_extension(".dds");
			}
			material->m_Textures.baseColor = TextureLoader::LoadTextureFromFile(texPath.string());
			material->m_Properties.bitMask |= BASE_COLOR_BIT;
		}
		else
		{
			aiReturn res = aiReturn_FAILURE;
			aiColor3D color{};
			res = aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
			if (res == aiReturn_SUCCESS)
			{
				material->m_Properties.baseColor = { color.r, color.g, color.b };
			}
		}
	}
#pragma endregion
#pragma region Metallic
	{
		aiReturn res = aiReturn_FAILURE;
		aiString path{};

		res = aiMat->GetTexture(aiTextureType::aiTextureType_METALNESS, 0, &path);
		if (res == aiReturn_SUCCESS)
		{
			file::path texPath = fir / path.C_Str();
			if (!file::exists(texPath))
			{
				texPath.replace_extension(".dds");
			}
			material->m_Textures.metallic = TextureLoader::LoadTextureFromFile(texPath.string());
			material->m_Properties.bitMask |= METALLIC_BIT;
		}
		else
		{
			aiReturn res = aiReturn_FAILURE;
			res = aiMat->Get(AI_MATKEY_METALLIC_FACTOR, fOut);
			if (res == aiReturn_SUCCESS)
			{
				material->m_Properties.metallic = fOut;
			}
		}
	}
#pragma endregion
#pragma region Roughness
	{
		aiReturn res = aiReturn_FAILURE;
		aiString path{};
		res = aiMat->GetTexture(aiTextureType::aiTextureType_DIFFUSE_ROUGHNESS, 0, &path);
		if (res == aiReturn_SUCCESS)
		{
			file::path texPath = fir / path.C_Str();
			if (!file::exists(texPath))
			{
				texPath.replace_extension(".dds");
			}
			material->m_Textures.roughness = TextureLoader::LoadTextureFromFile(texPath.string());
			material->m_Properties.bitMask |= ROUGHNESS_BIT;
		}
		else
		{
			aiReturn res = aiReturn_FAILURE;
			res = aiMat->Get(AI_MATKEY_ROUGHNESS_FACTOR, fOut);
			if (res == aiReturn_SUCCESS)
			{
				material->m_Properties.roughness = fOut;
			}
		}
	}
#pragma endregion
#pragma region Occulsion
	{
		aiReturn res = aiReturn_FAILURE;
		aiString path{};

		res = aiMat->GetTexture(aiTextureType::aiTextureType_AMBIENT_OCCLUSION, 0, &path);
		if (res == aiReturn_SUCCESS)
		{
			file::path texPath = fir / path.C_Str();
			if (!file::exists(texPath))
			{
				texPath.replace_extension(".dds");
			}
			material->m_Textures.occlusion = TextureLoader::LoadTextureFromFile(texPath.string());
			material->m_Properties.bitMask |= OCCLUSION_BIT;
		}
	}
#pragma endregion
#pragma region Normal
	{
		aiReturn res = aiReturn_FAILURE;
		aiString path{};
		res = aiMat->GetTexture(aiTextureType::aiTextureType_NORMALS, 0, &path);
		if (res == aiReturn_SUCCESS)
		{
			file::path texPath = fir / path.C_Str();
			if (!file::exists(texPath))
			{
				texPath.replace_extension(".dds");
			}
			material->m_Textures.normal = TextureLoader::LoadTextureFromFile(texPath.string());
			material->m_Properties.bitMask |= NORMAL_BIT;
		}
	}
#pragma endregion

	return material;
}
