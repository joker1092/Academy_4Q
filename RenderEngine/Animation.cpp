#include "Animation.h"
#include "Model.h"

Animation::Animation(const std::string& animationPath, AnimModel* model)
{
	Assimp::Importer importer;
	_scene = importer.ReadFile(animationPath, aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
	if (!_scene || _scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !_scene->mRootNode)
	{
		std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	_globalInverseTransform = DirectX::XMMatrixIdentity();
	_rootNode = new AnimationNode();
	_duration = static_cast<float>(_scene->mAnimations[0]->mDuration);
	_ticksPerSecond = _scene->mAnimations[0]->mTicksPerSecond;

	ReadHierarchyData(_rootNode, _scene->mRootNode);
	ReadAnimationData(_scene->mAnimations[0], model);
}

Animation::Animation(const aiScene* _scene, AnimModel* model, uint32 index)
{
	_globalInverseTransform = DirectX::XMMatrixIdentity();
	_rootNode = new AnimationNode();
	_duration = static_cast<float>(_scene->mAnimations[index]->mDuration);
	_ticksPerSecond = _scene->mAnimations[index]->mTicksPerSecond;

	ReadHierarchyData(_rootNode, _scene->mRootNode);
	ReadAnimationData(_scene->mAnimations[index], model);
}
void Animation::ReadHierarchyData(AnimationNode* dest, const aiNode* src)
{
	if (_boneInfoMap.find(src->mName.C_Str()) != _boneInfoMap.end())
	{
		dest->name = src->mName.C_Str();
		dest->transform = XMMatrixTranspose(XMMATRIX(&src->mTransformation.a1));
		dest->numChildren = src->mNumChildren;

		auto it = std::find_if(_bones.begin(), _bones.end(), [&](const BoneReference& bone) { return bone.GetBoneName() == dest->name; });

		if (it == _bones.end())
		{
			std::string boneName = dest->name;
			_bones.push_back(BoneReference{ boneName, _boneInfoMap[dest->name].id, dest->transform });
		}

		for (uint32 i = 0; i < src->mNumChildren; i++)
		{
			AnimationNode* child = new AnimationNode();
			dest->children.push_back(child);
			ReadHierarchyData(child, src->mChildren[i]);
		}
	}
	else
	{
		for (uint32 i = 0; i < src->mNumChildren; i++)
		{
			ReadHierarchyData(dest, src->mChildren[i]);
		}
	}
}

void Animation::ReadAnimationData(const aiAnimation* animation, AnimModel* model)
{
	int numChannaels = animation->mNumChannels;

	auto& boneInfoMap = model->_boneInfoMap;
	int& numBones = model->_numBones;

	for (int i = 0; i < numChannaels; i++)
	{
		auto channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.C_Str();
		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			boneInfoMap[boneName].id = numBones;
			numBones++;
		}

		_bones.push_back(BoneReference{ boneName, boneInfoMap[boneName].id, channel });
	}

	boneInfoMap = _boneInfoMap;
}
//
//void Animation::ReadHierarchyData(AnimationNode* dest, const aiNode* src)

//
//void Animation::ReadAnimationData(const aiAnimation* animation, AnimModel* model)
