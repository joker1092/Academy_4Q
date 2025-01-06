#pragma once
#include <filesystem>
#include <Windows.h>

namespace file = std::filesystem;

namespace InternalPath
{
    inline file::path ExecuteablePath{};
    inline file::path DataPath{};
    inline file::path ShaderSourcePath{};

    inline void Initialize()
    {
        HMODULE hModule = GetModuleHandleW(NULL);
        WCHAR path[MAX_PATH]{};

        GetModuleFileNameW(hModule, path, MAX_PATH);
        file::path p(path);

        ExecuteablePath = p.remove_filename();

        auto base = file::path(ExecuteablePath);
        DataPath = file::path(base).append("../Data/").lexically_normal();
        ShaderSourcePath = file::path(base).append("../Data/Shaders/").lexically_normal();
    }
};

static class PathFinder
{
public:
    inline PathFinder()
    {
        InternalPath::Initialize();
    }

    static inline file::path Relative(const std::string_view& path)
    {
        return file::path(InternalPath::DataPath) / path;
    }

    static inline file::path RelativeToExecutable(const std::string_view& path)
    {
        return file::path(InternalPath::ExecuteablePath) / path;
    }

    static inline file::path ShaderPath()
    {
        return InternalPath::ShaderSourcePath;
    }
};
