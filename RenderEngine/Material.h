#pragma once
#include "BufferType.h"
#include "MaterialTextures.h"

struct Material : public Noncopyable
{
    Material(const std::string_view& name = "BasicMaterial") : m_Name(name) {}
    ~Material() = default;

    std::string m_Name;
    MaterialProperties m_Properties{};
    MaterialTextures m_Textures{};
};
