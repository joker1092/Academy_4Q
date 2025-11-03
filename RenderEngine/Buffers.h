#pragma once
#include "TypeDefinition.h"
#include "Core.Mathf.h"
#include <algorithm>

typedef uint32 Index;
constexpr uint32 MAX_POINT_LIGHTS = 25;

alstruct CameraBuffer
{
	DirectX::XMMATRIX vp;
	alfloat3 position;
	float	 exposure;
	alfloat3 viewdir;
	float	 _placeholder4 = 0;
	alfloat3 up;
	float    _placeholder5 = 0;
	alfloat3 right;
	float	 _placeholder6 = 0;
};

alstruct PointLightBuffer
{
	alfloat3 position{};
	float	 range = 0;
	alfloat3 color{};
	float	 intensity = 0;
};

alstruct SceneBuffer
{
	alfloat3		 ambientcolor{};
	float			 iblIntensity = 0;
	alfloat3		 sunpos{};
	uint32			 preciseShadows = 0;
	alfloat3		 suncolor{};
	float			 _placeholder3 = 0;
	alfloat3		 iblColor{};
	uint32			 numPointLights = 0;
	PointLightBuffer pointLights[MAX_POINT_LIGHTS];
};


alstruct FxaaBuffer
{
	alignas(16) DirectX::XMINT2	textureSize;
	float			  bias = 0.1f;
	float			  biasMin = 0.05f;
	alignas(16) float spanMax = 8.0f;
	float			  _spacer1 = 0.0f;
	float			  _spacer2 = 0.0f;
	float			  _spacer3 = 0.0f;
};

struct lapacianFilterProperties
{
	uint32 width;
	uint32 height;
	uint32 _spacer1;
	uint32 _spacer2;
};

constexpr flag DIFFUSE_BIT	 = 1U << 0;
constexpr flag OCCLUSIONMETALLICROUGHNESS_BIT = 1U << 1;
constexpr flag EMISSIVE_BIT  = 1U << 2;
constexpr flag NORMAL_BIT    = 1U << 3;
constexpr flag OUTLINE_BIT	 = 1U << 4;

alstruct MaterialProperties
{
	alfloat3	diffuse = { 1.0f, 1.0f, 1.0f };
	float		roughness = 0.9f;
	alfloat3	emissive = { 0.0f, 0.0f, 0.0f };
	float		metalness = 0.0f;
	alflag		bitmask = 0;
};

struct ModelBuffer
{
	DirectX::XMMATRIX	modelmatrix;
};

struct MVPBuffer
{
	DirectX::XMMATRIX	MVP;
};

struct LightSpaceBuffer
{
	DirectX::XMMATRIX	lightSpaceMatrix;
};

struct GridBuffer
{
	DirectX::XMMATRIX	m;  // Model-World matrix
};

struct BlurParameters
{
	int2	textureSize;
	float	_spacer1 = 0.0f;
	float	_spacer2 = 0.0f;
};

alstruct OutlineProperties
{
	Mathf::Color4 color{ 0.0f, 0.0f, 0.0f, 1.0f };
	float oulineThickness = 0.008f;
	uint32 bitmask = 0;
};

struct Vertex
{
	float3	position{};
	float3	normal{};
	float3	tangent{};
	float3	bitangent{};
	float2	texcoord{};
};

struct AnimVertex
{
    float3	position{};
	float3	normal{};
	float3	tangent{};
	float3	bitangent{};
	float2	texcoord{};
	int		boneIndex[4]{ -1,-1,-1,-1 };
	float	boneWeight[4]{};

	void SetBoneData(int index, float weight)
	{
		for (int i = 0; i < 4; i++)
		{
			if (boneIndex[i] < 0)
			{
				boneIndex[i] = index;
				boneWeight[i] = weight;
				break;
			}
		}
	}
};

struct DebugVertex
{
	float3 position{};
	float4 color{};
};
