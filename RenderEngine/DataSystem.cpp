#include "DataSystem.h"
#include "HLSLCompiler.h"
#include "ModelLoader.h"
#include "TextureLoader.h"

DataSystem::DataSystem()
{
}

DataSystem::~DataSystem()
{
	ReleaseShaders();
	ReleaseModels();
}

void DataSystem::Initialize(const std::shared_ptr<DirectX11::DeviceResources>& deviceResource)
{
	m_deviceResources = deviceResource;
	ModelLoader::Initialize(m_deviceResources);
	TextureLoader::Initialize(m_deviceResources);
	LoadShaders();
	LoadModels();
}

void DataSystem::LoadShaders()
{
	try
	{
		file::path shaderpath = PathFinder::ShaderPath();
		for (auto& dir : file::recursive_directory_iterator(shaderpath))
		{
			if(dir.is_directory())
				continue;

			if (dir.path().extension() == ".hlsl")
			{
				AddShaderFormPath(dir.path());
			}
		}
	}
	catch (const file::filesystem_error& e)
	{
		OutputDebugStringA(e.what());
	}
	catch (const std::exception& e)
	{
		OutputDebugStringA(e.what());
	}
}

void DataSystem::LoadModels()
{
	file::path modelPath = PathFinder::Relative("Models/");
	for (auto& dir : file::recursive_directory_iterator(modelPath))
	{
		if (dir.is_directory())
			continue;

		if (dir.path().extension() == ".fbx" || dir.path().extension() == ".obj")
		{
			AddModel(dir.path(), dir.path().parent_path());
		}
	}
}

Model* DataSystem::AllocateModel()
{
	Model* model{ DataSystem::GetInstance()->m_ModelPool.allocate_element() };
	return model;
}

AnimModel* DataSystem::AllocateAnimModel()
{
	return nullptr;
}

Material* DataSystem::AllocateMaterial()
{
	Material* material{ DataSystem::GetInstance()->m_MaterialPool.allocate_element() };
	return material;
}

void DataSystem::ReleaseModel(Model* model)
{
	DataSystem::GetInstance()->m_ModelPool.destroy(model);
}

void DataSystem::ReleaseAnimModel(AnimModel* model)
{
	DataSystem::GetInstance()->m_AnimModelPool.destroy(model);
}

void DataSystem::ReleaseMaterial(Material* material)
{
	DataSystem::GetInstance()->m_MaterialPool.destroy(material);
}

void DataSystem::AddShaderFormPath(const file::path& path)
{
	ComPtr<ID3DBlob> blob = HLSLCompiler::LoadFormFile(path.string());
	file::path filename = path.filename();
	std::string ext = filename.replace_extension().extension().string();
	filename.replace_extension();
	ext.erase(0, 1);

	AddShader(filename.string(), ext, blob.Get());
}

void DataSystem::AddModel(const file::path& path, const file::path& dir)
{
	Model* model = AllocateModel();

	ModelLoader::LoadFormFile(path, dir, &model, nullptr);
	if (model)
	{
		m_Models[model->name] = model;
	}
}

void DataSystem::AddShader(const std::string_view& name, const std::string_view& ext, ID3DBlob* blob)
{
	if (ext == "vs")
	{
		VertexShader shader{ m_deviceResources, name, blob };
		shader.Compile();
		m_VertexShaders[std::string(name)] = shader;
	}
	else if (ext == "hs")
	{
		HullShader shader{ m_deviceResources, name, blob };
		shader.Compile();
		m_HullShaders[std::string(name)] = shader;
	}
	else if (ext == "ds")
	{
		DomainShader shader{ m_deviceResources, name, blob };
		shader.Compile();
		m_DomainShaders[std::string(name)] = shader;
	}
	else if (ext == "gs")
	{
		GeometryShader shader{ m_deviceResources, name, blob };
		shader.Compile();
		m_GeometryShaders[std::string(name)] = shader;
	}
	else if (ext == "ps")
	{
		PixelShader shader{ m_deviceResources, name, blob };
		shader.Compile();
		m_PixelShaders[std::string(name)] = shader;
	}
	else if (ext == "cs")
	{
		ComputeShader shader{ m_deviceResources, name, blob };
		shader.Compile();
		m_ComputeShaders[std::string(name)] = shader;
	}
	else
	{
		OutputDebugStringA("Invalid Shader Extension");
	}
}

void DataSystem::ReleaseShaders()
{
	m_VertexShaders.clear();
	m_HullShaders.clear();
	m_DomainShaders.clear();
	m_GeometryShaders.clear();
	m_PixelShaders.clear();
	m_ComputeShaders.clear();
}

void DataSystem::ReleaseModels()
{
	for (auto& [name, model] : m_Models)
	{
		ReleaseModel(model);
	}
	m_Models.clear();
}
