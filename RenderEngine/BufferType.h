#pragma once
#include "Core.Minimal.h"

constexpr constant MAX_JOINTS = 24;

struct JointBuffer
{
    Mathf::xMatrix JointTransforms[MAX_JOINTS];
};

struct alignas(16) CameraBuffer
{
    Mathf::xMatrix VP{};
    alignas(16) Mathf::xMatrix position{};
    float          exposure{};
    alignas(16) Mathf::Vector3 viewDir{};
    float          placeholder{};
};

struct alignas(16) SceneBuffer
{
    alignas(16) Mathf::Vector3  ambientColor{};
    float           IBLIntensity{};
    alignas(16) Mathf::Vector3  SunPos{};
    uint32          preciseShadow{};
    alignas(16) Mathf::Vector3  SunColor{};
    float           placeholder{};
    alignas(16) Mathf::Color3   IBLColor{};
};

constexpr mask BASE_COLOR_BIT = 1U << 0;
constexpr mask METALLIC_BIT = 1U << 1;
constexpr mask ROUGHNESS_BIT = 1U << 2;
constexpr mask OCCLUSION_BIT = 1U << 3;
constexpr mask EMISSIVE_BIT = 1U << 4;
constexpr mask NORMAL_BIT = 1U << 5;

struct alignas(16) MaterialProperties
{
    alignas(16) Mathf::Color3 baseColor{};
    float         roughness{};
    alignas(16) Mathf::Color3 emissive{};
    float         metallic{};
    alignas(16) mask          bitMask {};
};

struct ModelBuffer
{
    Mathf::xMatrix modelMatrix{};
};

struct MVPBuffer
{
    Mathf::xMatrix MVP{};
};

struct LightSpaceBuffer
{
    Mathf::xMatrix lightSpaceMatrix{};
};

struct GridBuffer
{
    Mathf::xMatrix ModelWorldMatrix{};
};

struct Vertex
{
    Mathf::Vector3 position{};
    Mathf::Vector3 normal{};
    Mathf::Vector3 tangent{};
    Mathf::Vector3 bitangent{};
    Mathf::Vector2 texCoord{};
};

struct AnimationVertex
{
    Mathf::Vector3 position{};
    Mathf::Vector3 normal{};
    Mathf::Vector2 texCoord{};
    int3           jointID{};
    float3         jointWeight{};
};
