#pragma once
#include "HLSLCompiler.h"
#include "Shader.h"
#include "ModelLoader.h"
#include "TextureLoader.h"

// Main system for storing runtime data
class DataSystem : public Singleton<DataSystem>
{
private:
    friend class Singleton;

	DataSystem() = default;
	~DataSystem();
public:
	void Initialize();
	void RenderForEditer();
	void MonitorFiles();
	void LoadShaders();
	void LoadModels();

	std::unordered_map<std::string, VertexShader>	VertexShaders;
	std::unordered_map<std::string, HullShader>		HullShaders;
	std::unordered_map<std::string, DomainShader>	DomainShaders;
	std::unordered_map<std::string, GeometryShader>	GeometryShaders;
	std::unordered_map<std::string, PixelShader>	PixelShaders;
	std::unordered_map<std::string, ComputeShader>	ComputeShaders;

	std::unordered_map<std::string, std::shared_ptr<Model>>		Models;
	std::unordered_map<std::string, std::shared_ptr<AnimModel>>	AnimatedModels;

private:
	void AddShaderFromPath(const file::path& filepath);
	void AddModel(const file::path& filepath, const file::path& dir);
	void AddShader(const std::string& name, const std::string& ext, const ComPtr<ID3DBlob>& blob);
	void RemoveShaders();

private:
	Texture2D icon{};
	uint32 currModelFileCount = 0;
	uint32 currShaderFileCount = 0;
	uint32 prevModelFileCount = 0;
	uint32 prevShaderFileCount = 0;
	std::thread m_DataThread{};
};

static inline auto& AssetsSystem = DataSystem::GetInstance();
