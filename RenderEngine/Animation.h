#pragma once
//#include "Model.h"
#include "AnimationInfo.h"

class AnimModel;
struct AnimationNode
{
	std::string name{};
	DirectX::XMMATRIX transform{};
	std::vector<AnimationNode*> children{};
	uint32 numChildren{};
};

class Animation
{
	float _duration{};
	double _ticksPerSecond{};

	std::vector<BoneReference> _bones{};
	AnimationNode* _rootNode{};
	std::unordered_map<std::string, BoneInfo> _boneInfoMap{};
	const aiScene* _scene{};

public:
	std::string _clipName{};
	XMMATRIX _globalInverseTransform{};

	Animation() = delete;
	Animation(const std::string& animationPath, AnimModel* model);
	Animation(const aiScene* _scene, AnimModel* model, uint32 index);
	~Animation() = default;

	BoneReference* FindBone(const std::string& name)
	{
		auto it = std::ranges::find_if(_bones, 
			[&](const BoneReference& bone) { return bone.GetBoneName() == name; });
		if (it == _bones.end())
			return nullptr;

		return &(*it);
	}
	float GetDurationSeconds() const { return _duration/ static_cast<float>(_ticksPerSecond); }
	float GetTicksPerSecond() const { return static_cast<float>(_ticksPerSecond); }
	float GetDuration() const { return _duration; }
	const AnimationNode& GetRootNode() { return *_rootNode; }
	const std::unordered_map<std::string, BoneInfo>& GetBoneInfoMap()
	{
		return _boneInfoMap;
	}

private:
	void ReadHierarchyData(AnimationNode* dest, const aiNode* src);
	void ReadAnimationData(const aiAnimation* animation, AnimModel* model);
};


class Animator
{
public:
    std::vector<XMMATRIX> _finalBoneTransforms{};
    Animation* _currentAnimation{};
    std::vector<Animation*> _animations{};
    float _currentTime{};
    float _deltaSeconds{};
	int _currIndex{};

	Animator() = default;
	~Animator() = default;

	void InitializedAnimation()
	{
		_currentAnimation = _animations[0];
		_currentTime = 0.f;
		_deltaSeconds = 0.f;
		_finalBoneTransforms.reserve(MAX_JOINTS);

		for (int i = 0; i < MAX_JOINTS; ++i)
		{
			_finalBoneTransforms.push_back(XMMatrixIdentity());
		}
	}

	void UpdateAnimation(float deltaSeconds, XMMATRIX proxyTransform)
	{
		_deltaSeconds = deltaSeconds;
		if (_currentAnimation)
		{
			_currentTime += _currentAnimation->GetTicksPerSecond() * deltaSeconds;
			_currentTime = fmod(_currentTime, _currentAnimation->GetDuration());
			CalculateBoneTransform(_currentAnimation->GetRootNode(), proxyTransform);
		}
	}

	void PlayAnimation(uint32 index)
	{
		if (index >= _animations.size())
		{
			Log::Warning("Invalid animation index");
			return;
		}
		else if (index == _currIndex)
		{
			return;
		}

		_currIndex = index;
		_currentAnimation = _animations[_currIndex];
		_currentTime = 0.f;
	}

	void CalculateBoneTransform(const AnimationNode& node, XMMATRIX parentTransform)
	{
		std::string nodeName = node.name;
		XMMATRIX nodeTransform = node.transform;

		BoneReference* bone = _currentAnimation->FindBone(nodeName);
		if (bone)
		{
			bone->Update(_currentTime);
			nodeTransform = bone->GetLocalTransform();
		}

		XMMATRIX globalTransform = nodeTransform * parentTransform;

		const auto& boneInfoMap = _currentAnimation->GetBoneInfoMap();
		auto it = boneInfoMap.find(nodeName);
		if (it != boneInfoMap.end())
		{
			const BoneInfo& boneInfo = it->second;
			int index = boneInfo.id;
			XMMATRIX offset = boneInfo.offset;
			_finalBoneTransforms[index] = XMMatrixTranspose(offset * globalTransform);
			//_finalBoneTransforms[index] = offset * globalTransform;
		}

		for (uint32 i = 0; i < node.numChildren; ++i)
		{
			CalculateBoneTransform(*node.children[i], globalTransform);
		}
	}

	const std::vector<XMMATRIX>& GetFinalBoneTransforms() const
	{
		return _finalBoneTransforms;
	}

};
