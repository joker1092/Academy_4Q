#include "ModelLoader.h"
#include "Banchmark.hpp"

void ModelLoader::LoadFromFile(const file::path& path, const file::path& dir, std::shared_ptr<Model>* model, std::shared_ptr<AnimModel>* animmodel)
{
	Assimp::Importer importer = Assimp::Importer();
	unsigned int flags = 0;
	flags |= aiProcess_FlipUVs;
	
	std::string name = file::path(path.filename()).replace_extension().string();

	const aiScene* scene = importer.ReadFile(path.string(), flags);

	const char* error = importer.GetErrorString();

	if (scene->mNumMeshes > 0)
	{
		if (!scene->mMeshes[0]->HasTangentsAndBitangents())
		{
			unsigned int postflags = 0;
			postflags |= aiProcess_CalcTangentSpace;

			scene = importer.ApplyPostProcessing(postflags);
		}
	}


	if (scene->mNumAnimations == 0)
	{
		*model = LoadModel(name, scene, path, dir);
		*animmodel = nullptr;
	}
	else
	{
		*animmodel = LoadAnimatedModel(name, scene, path, dir);
		*model = nullptr;
	}

}

std::shared_ptr<Model> ModelLoader::LoadModel(const std::string& name, const aiScene* scene, const file::path& path, const file::path& dir)
{
	std::shared_ptr<Model> model = std::make_shared<Model>();
	model->name = name;
	LoadMeshes(scene, dir, &model->meshes);
	return model;
}

std::shared_ptr<AnimModel> ModelLoader::LoadAnimatedModel(const std::string& name, const aiScene* scene, const file::path& path, const file::path& dir)
{
	std::shared_ptr<AnimModel> model = std::make_shared<AnimModel>();
	model->name = name;

	// Load Animated mesh
	// To be implemented...
	// Load Animations
	// To be implemented...
	return model;
}

void ModelLoader::LoadMeshes(const aiScene* scene, const file::path& dir, std::vector<Mesh>* meshes)
{
	Banchmark banchmark{};
	for (UINT i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* aimesh = scene->mMeshes[i];

		std::vector<Vertex> vertices = std::vector<Vertex>(aimesh->mNumVertices);
		std::vector<Index> indices = std::vector<Index>(aimesh->mNumVertices);
		for (uint32 i = 0; i < vertices.size(); i++)
		{
			vertices[i].position = float3{
				aimesh->mVertices[i].x,
				aimesh->mVertices[i].y,
				aimesh->mVertices[i].z
			};

			vertices[i].normal = float3{
				aimesh->mNormals[i].x,
				aimesh->mNormals[i].y,
				aimesh->mNormals[i].z,
			};

			vertices[i].tangent = float3{
				aimesh->mTangents[i].x,
				aimesh->mTangents[i].y,
				aimesh->mTangents[i].z,
			};

			vertices[i].bitangent = float3{
				aimesh->mBitangents[i].x,
				aimesh->mBitangents[i].y,
				aimesh->mBitangents[i].z,
			};

			if (aimesh->mTextureCoords[0])
			{
				vertices[i].texcoord = float2{
					aimesh->mTextureCoords[0][i].x,  // Take first texture coord
					aimesh->mTextureCoords[0][i].y
				};
			}

		}
		for (uint32 u = 0; u < aimesh->mNumFaces; u++)
		{
			// Heavily assumes that mesh is triangulated
			indices.push_back(aimesh->mFaces[u].mIndices[0]);
			indices.push_back(aimesh->mFaces[u].mIndices[1]);
			indices.push_back(aimesh->mFaces[u].mIndices[2]);
		}

		// Add in materials at the same time

		std::string name = std::string(scene->mMeshes[i]->mName.C_Str());
		std::shared_ptr<Material> mat = CreateMaterial(dir, aimesh, scene);

		meshes->emplace_back(
			name,
			indices,
			vertices,
			mat
		);
	}

	for (UINT i = 0; i < meshes->size(); i++)
	{
		(*meshes)[i].CreateBuffers();
	}

	for (auto& mesh : *meshes)
	{
		mesh.CreateBuffers();
	}
	std::cout << "Meshes loaded in ";
}

std::shared_ptr<Material> ModelLoader::CreateMaterial(const file::path& dir, aiMesh* aimesh, const aiScene* scene)
{
	std::shared_ptr<Material> material = std::make_shared<Material>();


	UINT idx = aimesh->mMaterialIndex;
	float fOut = 0.0f;
	aiMaterial* aimat = scene->mMaterials[idx];


	aiString matname = aimat->GetName();
	material->name = std::string(matname.C_Str());

	MatOverrides overrides;

	// Diffuse
	{
		aiReturn res = aiReturn_FAILURE;
		aiString path;

		res = aimat->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &path);
		if (res == aiReturn_SUCCESS)
		{
			file::path filepath = dir / path.C_Str();

			if (!file::exists(filepath))
			{
				filepath.replace_extension(".dds");
			}
			material->textures.diffuse = TextureLoader::LoadFromFile(filepath);
			material->properties.bitmask |= DIFFUSE_BIT;
		}
		else
		{
			aiReturn ressec = aiReturn_FAILURE;
			aiColor3D pOut;

			ressec = aimat->Get(AI_MATKEY_COLOR_DIFFUSE, pOut);
			if (ressec == aiReturn_SUCCESS)
				material->properties.diffuse = DirectX::XMFLOAT3{ pOut.r, pOut.g, pOut.b };
		}
	}
	// Metallic
	{
		aiReturn res = aiReturn_FAILURE;
		aiString path;

		res = aimat->GetTexture(aiTextureType::aiTextureType_METALNESS, 0, &path);
		if (res == aiReturn_SUCCESS)
		{
			file::path filepath = dir / path.C_Str();

			if (!file::exists(filepath))
			{
				filepath.replace_extension(".dds");
			}
			material->textures.metallic = TextureLoader::LoadFromFile(filepath);
			material->properties.bitmask |= METALNESS_BIT;
		}
		else
		{
			aiReturn ressec = aiReturn_FAILURE;
			float pOut;

			ressec = aimat->Get(AI_MATKEY_METALLIC_FACTOR, pOut);
			if (ressec == aiReturn_SUCCESS)
				material->properties.metalness = pOut;
		}
	}
	// Roughness
	{
		aiReturn res = aiReturn_FAILURE;
		aiString path;

		res = aimat->GetTexture(aiTextureType::aiTextureType_DIFFUSE_ROUGHNESS, 0, &path);
		if (res == aiReturn_SUCCESS)
		{
			file::path filepath = dir / path.C_Str();

			if (!file::exists(filepath))
			{
				filepath.replace_extension(".dds");
			}
			material->textures.roughness = TextureLoader::LoadFromFile(filepath);
			material->properties.bitmask |= ROUGHNESS_BIT;
		}
		else
		{
			aiReturn ressec = aiReturn_FAILURE;
			float pOut;

			ressec = aimat->Get(AI_MATKEY_ROUGHNESS_FACTOR, pOut);
			if (ressec == aiReturn_SUCCESS)
				material->properties.roughness = pOut;
		}
	}
	// Occlusion
	{
		aiReturn res = aiReturn_FAILURE;
		aiString path;

		res = aimat->GetTexture(aiTextureType::aiTextureType_AMBIENT_OCCLUSION, 0, &path);
		if (res == aiReturn_SUCCESS)
		{
			file::path filepath = dir / path.C_Str();

			if (!file::exists(filepath))
			{
				filepath.replace_extension(".dds");
			}
			material->textures.occlusion= TextureLoader::LoadFromFile(filepath);
			material->properties.bitmask |= OCCLUSION_BIT;
		}
	}
	// Tangent Space Normal
	{
		aiReturn res = aiReturn_FAILURE;
		aiString path;

		res = aimat->GetTexture(aiTextureType::aiTextureType_NORMALS, 0, &path);
		if (res == aiReturn_SUCCESS)
		{
			file::path filepath = dir / path.C_Str();

			if (!file::exists(filepath))
			{
				filepath.replace_extension(".dds");
			}
			material->textures.normal = TextureLoader::LoadFromFile(filepath);
			material->properties.bitmask |= NORMAL_BIT;
		}
	}

	// A bit unecessary to parse every time

	//file::path overridepath = dir / "matoverride.json";
	//if (file::exists(overridepath))
	//{
	//	json j;
	//	std::ifstream in(overridepath);
	//	in >> j;

	//	for (auto& element : j)
	//	{
	//		std::string name = element["Name"];

	//		if (name == material->name)
	//		{
	//			float roughness = element["Roughness"].get<float>();
	//			float metalness = element["Metalness"].get<float>();

	//			// Will be ignored if proper maps are set
	//			material->properties.metalness = metalness;
	//			material->properties.roughness = roughness;
	//		}
	//	}

	//}

	return material;

}
