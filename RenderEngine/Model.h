#pragma once
#include "Mesh.h"

class Model
{
public:
	Model() = default;
	~Model() = default;

	std::string				name{};
	file::path				path{};
	std::vector<Mesh>		meshes{};

	bool					isLoaded = false;
};

class AnimModel
{
public:
	AnimModel() = default;
	~AnimModel() = default;

	std::string				name{};
	file::path				path{};
	std::vector<AnimMesh>	meshes;
	std::shared_ptr<Animator> animator;
	std::unordered_map<std::string, BoneInfo> _boneInfoMap;

	float					_distance{ 0.0f };
	int						_numBones{ 0 };
	bool					isLoaded = false;

    const Mathf::xMatrix* GetFinalBoneTransforms() const
    {
        return animator->GetFinalBoneTransforms().data();
    }
};
