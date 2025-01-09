#pragma once
#include "TypeDefinition.h"

constexpr uint32 MAX_JOINTS = 24;
typedef uint32 Index;

struct JointBuffer
{
	//joint
	DirectX::XMMATRIX transforms[MAX_JOINTS];
};

alstruct CameraBuffer
{
	DirectX::XMMATRIX vp;
	alfloat3 position;
	float	 exposure;
	alfloat3 viewdir;
	float	 _placeholder4 = 0;
};

alstruct SceneBuffer
{
	alfloat3	ambientcolor{};
	float		iblIntensity = 0;
	alfloat3	sunpos{};
	uint32		preciseShadows = 0;
	alfloat3	suncolor{};
	float		_placeholder3 = 0;
	alfloat3	iblColor{};
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

constexpr flag DIFFUSE_BIT	 = 1U << 0;
constexpr flag METALNESS_BIT = 1U << 1;
constexpr flag ROUGHNESS_BIT = 1U << 2;
constexpr flag OCCLUSION_BIT = 1U << 3;
constexpr flag EMISSIVE_BIT  = 1U << 4;
constexpr flag NORMAL_BIT    = 1U << 5;

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

struct Vertex
{
	float3	position;
	float3	normal;
	float3	tangent;
	float3	bitangent;
	float2	texcoord;
};

struct AnimVertex
{
	float3	position;
	float3	normal;
	float2	texcoord;
	int3	jointid;
	float3	weight;
};

