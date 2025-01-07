#pragma once
#include "Shader.h"
#include "ModelLoader.h"
#include "TextureLoader.h"
#include "DeviceResources.h"

class DataSystem : public Singleton<DataSystem>
{
private:
	friend class Singleton;

public:
	DataSystem();
	~DataSystem();
	
	void Initialize(const std::shared_ptr<DirectX11::DeviceResources>& deviceResource);
	void LoadShaders();
	void LoadModels();

	static Model* AllocateModel();
	static AnimModel* AllocateAnimModel();
	static Material* AllocateMaterial();

	static void ReleaseModel(Model* model);
	static void ReleaseAnimModel(AnimModel* model);
	static void ReleaseMaterial(Material* material);

	MemoryPool<Model> m_ModelPool;
	MemoryPool<AnimModel> m_AnimModelPool;
	MemoryPool<Material> m_MaterialPool;

	std::unordered_map<std::string, VertexShader> m_VertexShaders;
	std::unordered_map<std::string, HullShader> m_HullShaders;
	std::unordered_map<std::string, DomainShader> m_DomainShaders;
	std::unordered_map<std::string, GeometryShader> m_GeometryShaders;
	std::unordered_map<std::string, PixelShader> m_PixelShaders;
	std::unordered_map<std::string, ComputeShader> m_ComputeShaders;

	std::unordered_map<std::string, Model*> m_Models;
	std::unordered_map<std::string, AnimModel*> m_AnimModels;

private:
	std::shared_ptr<DirectX11::DeviceResources> m_deviceResources;

	void AddShaderFormPath(const file::path& path);
	void AddModel(const file::path& path, const file::path& dir);
	void AddShader(const std::string_view& name, const std::string_view& ext, ID3DBlob* blob);
	void ReleaseShaders();
	void ReleaseModels();
};

static auto AssetsSystem = DataSystem::GetInstance();