#pragma once
#include "Core.Minimal.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h> 
#include <assimp/material.h>

constexpr uint32 MAX_JOINTS = 128;
constexpr int MAX_BONE_WEIGHTS = 4;

struct JointBuffer
{
	//joint
	DirectX::XMMATRIX transforms[MAX_JOINTS];
};

struct BoneInfo
{
	int id;
	DirectX::XMMATRIX offset;
};

struct KeyPosition 
{
	XMVECTOR position;
	float timeStamp;
};

struct KeyRotation 
{
	XMVECTOR orientation;
	float timeStamp;
};

struct KeyScale 
{
	XMVECTOR scale;
	float timeStamp;
};

class BoneReference 
{
	std::string _name{};
	std::vector<KeyPosition> _positions{};
	std::vector<KeyRotation> _rotations{};
	std::vector<KeyScale> _scales{};
	int _numPositions{};
	int _numRotations{};
	int _numScalings{};

	XMMATRIX _localTransform{};
	int _ID{};

public:
	BoneReference(const std::string& name, int ID, XMMATRIX localMatrix);
	BoneReference(const std::string& name, int ID, const aiNodeAnim* channel);

	void Update(float animationTime)
	{
		XMMATRIX translate = InterpolatePosition(animationTime);
		XMMATRIX rotation = InterpolateRotation(animationTime);
		XMMATRIX scaling = InterpolateScaling(animationTime);
		_localTransform = scaling * rotation * translate;
	}

	XMMATRIX GetLocalTransform() const { return _localTransform; }
	std::string GetBoneName() const { return _name; }
	int GetBoneID() const { _ID; }

	int GetPositionIndex(float animationTime) const
	{
		for (int index = 0; index < _numPositions - 1; ++index)
		{
			if (animationTime < _positions[index + 1].timeStamp)
				return index;
		}
		return -1;
	}

	int GetRotationIndex(float animationTime) const
	{
		for (int index = 0; index < _numRotations - 1; ++index)
		{
			if (animationTime < _rotations[index + 1].timeStamp)
				return index;
		}
		return -1;
	}

	int GetScaleIndex(float animationTime) const
	{
		for (int index = 0; index < _numScalings - 1; ++index)
		{
			if (animationTime < _scales[index + 1].timeStamp)
				return index;
		}
		return -1;
	}

private:
	float GetScaleFactor(float prevKeyframeTimeStamp, float nextKeyframeTimeStamp, float animationTime)
	{
		float scaleFactor{ 0.f };
		float midwayLength = animationTime - prevKeyframeTimeStamp;
		float keyframeLength = nextKeyframeTimeStamp - prevKeyframeTimeStamp;
		scaleFactor = midwayLength / keyframeLength;
		return scaleFactor;
	}

	XMMATRIX InterpolatePosition(float animationTime)
	{
		if (_positions.empty())
		{
			Mathf::Matrix mat = _localTransform;
			_positions.push_back(KeyPosition{ mat.Translation(), 0.f });
			_numPositions = static_cast<int>(_positions.size());
		}

		if (_numPositions == 1)
			return XMMatrixTranslationFromVector(_positions[0].position);

		int p0Index = GetPositionIndex(animationTime);
		int p1Index = p0Index + 1;

		float scaleFactor = GetScaleFactor(_positions[p0Index].timeStamp, _positions[p1Index].timeStamp, animationTime);

		XMVECTOR interpolatedPosition = XMVectorLerp(_positions[p0Index].position, _positions[p1Index].position, scaleFactor);

		return XMMatrixTranslationFromVector(interpolatedPosition);
	}

	XMMATRIX InterpolateRotation(float animationTime)
	{
		if (_rotations.empty())
		{
			Mathf::Matrix mat = _localTransform;
			_rotations.push_back(KeyRotation{ XMQuaternionRotationMatrix(mat), 0.f });
			_numRotations = static_cast<int>(_rotations.size());
		}

		if (_numRotations == 1)
		{
			XMVECTOR quatRotation = XMQuaternionNormalize(_rotations[0].orientation);
			return XMMatrixRotationQuaternion(quatRotation);
		}

		int p0Index = GetRotationIndex(animationTime);
		int p1Index = p0Index + 1;

		float scaleFactor = GetScaleFactor(_rotations[p0Index].timeStamp, _rotations[p1Index].timeStamp, animationTime);

		XMVECTOR interpolatedQuatRotation = XMQuaternionSlerp(
			XMQuaternionNormalize(_rotations[p0Index].orientation),
			XMQuaternionNormalize(_rotations[p1Index].orientation),
			scaleFactor
		);

		return XMMatrixRotationQuaternion(interpolatedQuatRotation);
	}

	XMMATRIX InterpolateScaling(float animationTime)
	{
		if (_scales.empty())
		{
			Mathf::Matrix mat = _localTransform;
			_scales.push_back(KeyScale{ Mathf::ExtractScale(mat), 0.f });
			_numScalings = static_cast<int>(_scales.size());
		}


		if (_numScalings == 1) {
			return XMMatrixScalingFromVector(_scales[0].scale);
		}

		int p0Index = GetScaleIndex(animationTime);
		int p1Index = p0Index + 1;

		float scaleFactor = GetScaleFactor(_scales[p0Index].timeStamp, _scales[p1Index].timeStamp, animationTime);
		XMVECTOR interpolatedScale = XMVectorLerp(_scales[p0Index].scale, _scales[p1Index].scale, scaleFactor);

		return XMMatrixScalingFromVector(interpolatedScale);
	}

};