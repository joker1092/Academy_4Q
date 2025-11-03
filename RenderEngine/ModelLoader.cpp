#include "ModelLoader.h"
#include "DataSystem.h"
#include "Animation.h"

std::unordered_map<std::string, std::shared_ptr<Material>> ModelLoader::Materials{};

std::string ConvertToUtf8(const std::wstring& wideStr) 
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string utf8Str(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, &utf8Str[0], size_needed, nullptr, nullptr);
	return utf8Str;
}

void ModelLoader::LoadFromFile(const file::path& path, const file::path& dir, std::shared_ptr<Model>* model, std::shared_ptr<AnimModel>* animmodel)
{
    Assimp::Importer importer = Assimp::Importer();
    importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
    importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 4);

    unsigned int flags = 0;
    flags |= aiProcess_Triangulate
		| aiProcess_JoinIdenticalVertices
		| aiProcess_ImproveCacheLocality
		| aiProcess_ValidateDataStructure
		| aiProcess_PopulateArmatureData
		| aiProcess_LimitBoneWeights
		| aiProcess_SortByPType
		| aiProcess_FlipUVs;
	
	std::string name = file::path(path.filename()).replace_extension().string();
	std::string utf8Path = ConvertToUtf8(path);
 
	const aiScene* scene = importer.ReadFile(path.string().c_str(), flags);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

    if (scene->mNumAnimations == 0)
    {
        unsigned int postflags = 0;
        postflags |= aiProcess_PreTransformVertices;

        scene = importer.ApplyPostProcessing(postflags);
    }

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
		(*model)->isLoaded = true;
	}
	else
	{
		*animmodel = LoadAnimatedModel(name, scene->mRootNode, scene, path, dir);
		*model = nullptr;
		(*animmodel)->isLoaded = true;
	}
}

std::shared_ptr<Model> ModelLoader::LoadModel(const std::string& name, const aiScene* scene, const file::path& path, const file::path& dir)
{
	std::shared_ptr<Model> model = std::make_shared<Model>();
	model->path = path;
	model->name = name;
	LoadMeshes(model->name, scene, dir, &model->meshes);
	return model;
}

std::shared_ptr<AnimModel> ModelLoader::LoadAnimatedModel(const std::string& name, aiNode* node, const aiScene* scene, const file::path& path, const file::path& dir)
{
	std::shared_ptr<AnimModel> model = std::make_shared<AnimModel>();

	model->animator = std::make_shared<Animator>();
	model->path = path;
	model->name = name;
	LoadMeshes(&model, scene, dir, &model->meshes);
	//ProcessNode(&model, model->name, node, scene, dir, &model->meshes);
	LoadAnimations(&model, path);

	return model;
}

void ModelLoader::ProcessNode(std::shared_ptr<AnimModel>* model, const std::string_view& modelname, aiNode* node, const aiScene* scene, const file::path& dir, std::vector<AnimMesh>* meshes)
{
	AnimMesh* pAniMesh{ nullptr };
	aiMesh* pMesh{ nullptr };

	for (uint32 i = 0; i < node->mNumMeshes; i++)
	{
		pMesh = scene->mMeshes[node->mMeshes[i]];
		pAniMesh = new AnimMesh();
		LoadMeshes(model, modelname, node, pMesh, scene, dir, &(*model)->meshes);
	}

	for (uint32 i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(model, modelname, node->mChildren[i], scene, dir, meshes);
	}
}

void ModelLoader::LoadMeshes(std::shared_ptr<AnimModel>* model, const std::string_view& modelname, aiNode* node, aiMesh* pMesh, const aiScene* scene, const file::path& dir, std::vector<AnimMesh>* meshes)
{
	std::vector<AnimVertex> vertices;
	std::vector<Index> indices;

	for (uint32 i = 0; i < pMesh->mNumVertices; i++)
	{
		AnimVertex vertex{};
		vertex.position = float3{
			pMesh->mVertices[i].x,
			pMesh->mVertices[i].y,
			pMesh->mVertices[i].z
		};
		vertex.normal = float3{
			pMesh->mNormals[i].x,
			pMesh->mNormals[i].y,
			pMesh->mNormals[i].z
		};

		if (pMesh->mTangents)
		{
			vertex.tangent = float3{
				pMesh->mTangents[i].x,
				pMesh->mTangents[i].y,
				pMesh->mTangents[i].z,
			};

			vertex.bitangent = float3{
				pMesh->mBitangents[i].x,
				pMesh->mBitangents[i].y,
				pMesh->mBitangents[i].z,
			};
		}
		if (pMesh->mTextureCoords[0])
		{
			vertex.texcoord = float2{
				pMesh->mTextureCoords[0][i].x,
				pMesh->mTextureCoords[0][i].y
			};
		}
		vertices.push_back(vertex);
	}

	auto faceSpan = std::span(pMesh->mFaces, pMesh->mNumFaces);
	foreach(faceSpan, [&](aiFace face)
	{
		auto indicesSpan = std::span(face.mIndices, face.mNumIndices);
		foreach(indicesSpan, [&](uint32 index)
		{
			indices.push_back(index);
		});
	});

	for (uint32 boneIndex = 0; boneIndex < pMesh->mNumBones; ++boneIndex)
	{
		aiBone* currBone = pMesh->mBones[boneIndex];

		int boneID = -1;
		std::string boneName{ currBone->mName.C_Str() };

		if ((*model)->_boneInfoMap.find(boneName) == (*model)->_boneInfoMap.end())
		{
			BoneInfo boneInfo{};
			boneInfo.id = (*model)->_numBones;
			boneInfo.offset = Mathf::aiToXMMATRIX(currBone->mOffsetMatrix);
			(*model)->_boneInfoMap[boneName] = boneInfo;
			boneID = (*model)->_numBones;
			(*model)->_numBones++;
		}
		else
		{
			boneID = (*model)->_boneInfoMap[boneName].id;
		}

		auto weights = currBone->mWeights;
		int numWeights = currBone->mNumWeights;

		//가중치를 정점에 할당
		for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
		{
			int vertexId = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			vertices[vertexId].SetBoneData(boneID, weight);
		}
	}

	std::string name{ pMesh->mName.C_Str() };
	std::shared_ptr<Material> mat{};

	file::path linkpath = dir.parent_path() / std::string((*model)->name + ".meta");
	if (file::exists(linkpath))
	{
		json j;
		std::ifstream in(linkpath);
		in >> j;
		std::string matname = j[name].get<std::string>();
		mat = AssetsSystem->Materials[matname];
	}
	else
	{
		mat = CreateMaterial((*model)->name, dir, pMesh, scene);
	}
	meshes->emplace_back(
		name,
		indices,
		vertices,
		mat
	);
	
	meshes->back().CreateBuffers();

}

void ModelLoader::LoadMeshes(const std::string_view& modelname, const aiScene* scene, const file::path& dir, std::vector<Mesh>* meshes)
{
	for (UINT i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* aimesh = scene->mMeshes[i];

		std::vector<Vertex> vertices;
		std::vector<Index> indices;
		vertices.reserve(aimesh->mNumVertices);
		indices.reserve(aimesh->mNumFaces * 3);
		for (uint32 i = 0; i < aimesh->mNumVertices; i++)
		{
			Vertex vertex{};
			vertex.position = float3{
				aimesh->mVertices[i].x,
				aimesh->mVertices[i].y,
				aimesh->mVertices[i].z
			};

			vertex.normal = float3{
				aimesh->mNormals[i].x,
				aimesh->mNormals[i].y,
				aimesh->mNormals[i].z,
			};

			if(aimesh->mTangents)
			{
				vertex.tangent = float3{
					aimesh->mTangents[i].x,
					aimesh->mTangents[i].y,
					aimesh->mTangents[i].z,
				};

				vertex.bitangent = float3{
					aimesh->mBitangents[i].x,
					aimesh->mBitangents[i].y,
					aimesh->mBitangents[i].z,
				};
			}
			else
			{
				vertex.tangent = float3{ 0, 0, 0 };
				vertex.bitangent = float3{ 0, 0, 0 };
			}

			if (aimesh->mTextureCoords[0])
			{
				vertex.texcoord = float2{
					aimesh->mTextureCoords[0][i].x,  // Take first texture coord
					aimesh->mTextureCoords[0][i].y
				};
			}

			vertices.push_back(vertex);
		}

		auto faceSpan = std::span(aimesh->mFaces, aimesh->mNumFaces);
		foreach(faceSpan, [&](aiFace face)
		{
			auto indicesSpan = std::span(face.mIndices, face.mNumIndices);
			foreach(indicesSpan, [&](uint32 index)
			{
				indices.push_back(index);
			});
		});

		//여기서 tangent, bitangent 계산
		for (size_t i = 0; i < indices.size(); i += 3)
		{
			Vertex& v0 = vertices[indices[i]];
			Vertex& v1 = vertices[indices[i + 1]];
			Vertex& v2 = vertices[indices[i + 2]];

			Mathf::xVector pos0 = XMLoadFloat3(&v0.position);
			Mathf::xVector pos1 = XMLoadFloat3(&v1.position);
			Mathf::xVector pos2 = XMLoadFloat3(&v2.position);

			Mathf::xVector uv0 = XMLoadFloat2(&v0.texcoord);
			Mathf::xVector uv1 = XMLoadFloat2(&v1.texcoord);
			Mathf::xVector uv2 = XMLoadFloat2(&v2.texcoord);

			Mathf::xVector edge1 = XMVectorSubtract(pos1, pos0);
			Mathf::xVector edge2 = XMVectorSubtract(pos2, pos0);

			Mathf::xVector deltaUV1 = XMVectorSubtract(uv1, uv0);
			Mathf::xVector deltaUV2 = XMVectorSubtract(uv2, uv0);

			float2 dUV1, dUV2;
			XMStoreFloat2(&dUV1, deltaUV1);
			XMStoreFloat2(&dUV2, deltaUV2);

			float f = 1.0f / (dUV1.x * dUV2.y - dUV2.x * dUV1.y);

			Mathf::xVector tangent = XMVectorSet(
				f * (dUV2.y * XMVectorGetX(edge1) - dUV1.y * XMVectorGetX(edge2)),
				f * (dUV2.y * XMVectorGetY(edge1) - dUV1.y * XMVectorGetY(edge2)),
				f * (dUV2.y * XMVectorGetZ(edge1) - dUV1.y * XMVectorGetZ(edge2)),
				0.0f
			);

			float3 tan;
			XMStoreFloat3(&tan, tangent);

			v0.tangent = tan;
			v1.tangent = tan;
			v2.tangent = tan;
		}

		for (auto& vertex : vertices)
		{
			Mathf::xVector normal = XMLoadFloat3(&vertex.normal);
			Mathf::xVector tangent = XMLoadFloat3(&vertex.tangent);
			tangent = XMVector3Normalize(tangent);
			Mathf::xVector bi = XMVector3Cross(normal, tangent);
			XMStoreFloat3(&vertex.tangent, tangent);
			XMStoreFloat3(&vertex.bitangent, bi);
		}

		// Add in materials at the same time

		std::string name = std::string(scene->mMeshes[i]->mName.C_Str());
		std::shared_ptr<Material> mat{};
		file::path linkpath = dir / std::string(modelname.data() + std::string(".meta"));
		if (file::exists(linkpath))
		{
			json j;
			std::ifstream in(linkpath);
			in >> j;
			std::string matname = j[name].get<std::string>();
			if (matname != "")
			{
				mat = AssetsSystem->Materials[matname];
			}
			else
			{
				mat = CreateMaterial(modelname, dir, aimesh, scene);
			}
		}
		else
		{
			mat = CreateMaterial(modelname, dir, aimesh, scene);
		}

		meshes->emplace_back(
			name,
			indices,
			vertices,
			mat
		);
	}

    foreach((*meshes), [&](Mesh& mesh)
    {
        mesh.CreateBuffers();
    });
}

void ModelLoader::LoadMeshes(std::shared_ptr<AnimModel>* model, const aiScene* scene, const file::path& dir, std::vector<AnimMesh>* meshes)
{
	for (uint32 i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* aimesh = scene->mMeshes[i];

		std::vector<AnimVertex> vertices;
		std::vector<Index> indices;
		vertices.reserve(aimesh->mNumVertices);
		indices.reserve(aimesh->mNumFaces * 3);
		for (uint32 i = 0; i < aimesh->mNumVertices; i++)
		{
			AnimVertex vertex{};
			vertex.position = float3{
				aimesh->mVertices[i].x,
				aimesh->mVertices[i].y,
				aimesh->mVertices[i].z
			};

			vertex.normal = float3{
				aimesh->mNormals[i].x,
				aimesh->mNormals[i].y,
				aimesh->mNormals[i].z,
			};

			if (aimesh->mTangents)
			{
				vertex.tangent = float3{
					aimesh->mTangents[i].x,
					aimesh->mTangents[i].y,
					aimesh->mTangents[i].z,
				};

				vertex.bitangent = float3{
					aimesh->mBitangents[i].x,
					aimesh->mBitangents[i].y,
					aimesh->mBitangents[i].z,
				};
			}

			if (aimesh->mTextureCoords[0])
			{
				vertex.texcoord = float2{
					aimesh->mTextureCoords[0][i].x,  // Take first texture coord
					aimesh->mTextureCoords[0][i].y
				};
			}

			vertices.push_back(vertex);
		}

		auto faceSpan = std::span(aimesh->mFaces, aimesh->mNumFaces);
		foreach(faceSpan, [&](aiFace face)
		{
			auto indicesSpan = std::span(face.mIndices, face.mNumIndices);
			foreach(indicesSpan, [&](uint32 index)
			{
				indices.push_back(index);
			});
		});

		for (uint32 boneIndex = 0; boneIndex < aimesh->mNumBones; ++boneIndex)
		{
			aiBone* currBone = aimesh->mBones[boneIndex];

			int boneID = -1;
			std::string boneName{ currBone->mName.C_Str() };

			if ((*model)->_boneInfoMap.find(boneName) == (*model)->_boneInfoMap.end())
			{
	            BoneInfo boneInfo{};
				boneInfo.id = (*model)->_numBones;
				boneInfo.offset = Mathf::aiToXMMATRIX(currBone->mOffsetMatrix);
	               (*model)->_boneInfoMap[boneName] = boneInfo;
				boneID = (*model)->_numBones;
	               (*model)->_numBones++;
			}
			else
			{
				boneID = (*model)->_boneInfoMap[boneName].id;
			}

			auto weights = currBone->mWeights;
			int numWeights = currBone->mNumWeights;

			//가중치를 정점에 할당
			for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
			{
				int vertexId = weights[weightIndex].mVertexId;
				float weight = weights[weightIndex].mWeight;
				vertices[vertexId].SetBoneData(boneID, weight);
			}
		}

		// Add in materials at the same time
	    std::string name{ scene->mMeshes[i]->mName.C_Str() };
		std::shared_ptr<Material> mat{}; 

		file::path linkpath = dir.parent_path() / std::string((*model)->name + ".meta");
		if (file::exists(linkpath))
		{
			json j;
			std::ifstream in(linkpath);
			in >> j;
			std::string matname = j[name].get<std::string>();
			mat = AssetsSystem->Materials[matname];
		}
		else
		{
			mat = CreateMaterial((*model)->name, dir, aimesh, scene);
		}
		meshes->emplace_back(
			name,
			indices,
			vertices,
			mat
		);
	}

	foreach((*meshes), [&](AnimMesh& mesh)
	{
	    mesh.CreateBuffers();
	});
}

void ModelLoader::LoadAnimations(std::shared_ptr<AnimModel>* model, const file::path& dir)
{
	Assimp::Importer importer;
    importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
    importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 4);

	const aiScene* _scene{};
    unsigned int flags = 0;
    flags |= aiProcess_FlipUVs;

	_scene = importer.ReadFile(dir.string().c_str(), flags);
	if (!_scene || _scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !_scene->mRootNode)
	{
		Log::Error("ERROR::ASSIMP::" + std::string(importer.GetErrorString()));
		return;
	}

	for (uint32 i = 0; i < _scene->mNumAnimations; i++)
	{
        (*model)->animator->_animations.push_back(new Animation(_scene, (*model).get(), i));
	}

    (*model)->animator->InitializedAnimation();
}

std::shared_ptr<Material> ModelLoader::CreateMaterial(const std::string_view& modelname, const file::path& dir, aiMesh* aimesh, const aiScene* scene)
{
	UINT idx = aimesh->mMaterialIndex;
	float fOut = 0.0f;
	aiMaterial* aimat = scene->mMaterials[idx];

	aiString matname = aimat->GetName();
	std::string key = modelname.data() + std::string("_") + std::string(matname.C_Str());

	if (Materials.find(key) != Materials.end())
	{
		return Materials[key];
	}

	std::shared_ptr<Material> material = std::make_shared<Material>();
	Materials[key] = material;
	material->name = key;

	std::string matOverrideName = material->name + ".mat";
	file::path overridepath = dir / matOverrideName;
	if (file::exists(overridepath))
	{
		json j;
		std::ifstream in(overridepath);
		in >> j;

		material->name = j["Name"].get<std::string>();
		flag bit = j["Bitmask"].get<uint32>();
		BitFlag flag = bit;
		// Diffuse
		{
			std::string name;
			if(j.contains("BaseColor"))
			{
				name = j["BaseColor"].get<std::string>();
			}
			file::path filepath = dir / name;
			if (1 < name.length() && file::exists(filepath))
			{
				material->textures.diffuse = TextureLoader::LoadFromFile(filepath, sRGBSettings::NO_SRGB);
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
			std::string name;
			if (j.contains("OcclusionRoughnessMetallic"))
			{
				name = j["OcclusionRoughnessMetallic"].get<std::string>();
			}
			else if (j.contains("Metalness") && j.contains("Roughness"))
			{
				float metalness = j["Metalness"].get<float>();
				float roughness = j["Roughness"].get<float>();
				material->properties.metalness = metalness;
				material->properties.roughness = roughness;
			}

			file::path filepath = dir / name;
			if (1 < name.length() && file::exists(filepath))
			{
				material->textures.occlusionMetallicRoughness = TextureLoader::LoadFromFile(filepath, sRGBSettings::NO_SRGB);
				material->properties.bitmask |= OCCLUSIONMETALLICROUGHNESS_BIT;
			}
			else
			{
				aiReturn ressec = aiReturn_FAILURE;
				float pOut;
				ressec = aimat->Get(AI_MATKEY_METALLIC_FACTOR, pOut);
				if (ressec == aiReturn_SUCCESS)
				{
					material->properties.metalness = pOut;
				}
				ressec = aimat->Get(AI_MATKEY_ROUGHNESS_FACTOR, pOut);
				if (ressec == aiReturn_SUCCESS)
				{
					material->properties.roughness = pOut;
				}
			}
		}

		// Tangent Space Normal
		{
			std::string name;
			if (j.contains("Normal"))
			{
				name = j["Normal"].get<std::string>();
			}

			file::path filepath = dir / name;
			if (1 < name.length() && file::exists(filepath))
			{
				material->textures.normal = TextureLoader::LoadFromFile(filepath, sRGBSettings::NO_SRGB);
				material->properties.bitmask |= NORMAL_BIT;
			}
		}
	}
	else
	{
		json j;
		std::ofstream out(overridepath);
		j["Name"] = material->name;
		// Diffuse
		{
			file::path filepath{};
			std::string name = material->name;
			name += "_BaseColor.png";
			filepath = dir / name;

			if (file::exists(filepath))
			{
				material->textures.diffuse = TextureLoader::LoadFromFile(filepath, sRGBSettings::NO_SRGB);
				j["BaseColor"] = name;
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
			file::path filepath{};
			std::string name = material->name;
			name += "_OcclusionRoughnessMetallic.png";
			filepath = dir / name;

			if (file::exists(filepath))
			{
				material->textures.occlusionMetallicRoughness = TextureLoader::LoadFromFile(filepath, sRGBSettings::NO_SRGB);
				j["OcclusionRoughnessMetallic"] = name;
				material->properties.bitmask |= OCCLUSIONMETALLICROUGHNESS_BIT;
			}
			else
			{
				aiReturn ressec = aiReturn_FAILURE;
				float pOut;

				ressec = aimat->Get(AI_MATKEY_METALLIC_FACTOR, pOut);
				if (ressec == aiReturn_SUCCESS)
				{
					material->properties.metalness = pOut;
					j["Metalness"] = pOut;
				}

				ressec = aimat->Get(AI_MATKEY_ROUGHNESS_FACTOR, pOut);
				if (ressec == aiReturn_SUCCESS)
				{
					material->properties.roughness = pOut;
					j["Roughness"] = pOut;
				}
			}
		}
		// Tangent Space Normal
		{
			file::path filepath{};
			std::string name = material->name;
			name += "_Normal.png";
			filepath = dir / name;

			if (file::exists(filepath))
			{
				material->textures.normal = TextureLoader::LoadFromFile(filepath, sRGBSettings::NO_SRGB);
				j["Normal"] = name;
				material->properties.bitmask |= NORMAL_BIT;
			}
		}

		j["Bitmask"] = material->properties.bitmask;

		out << j;
	}

	return material;

}
