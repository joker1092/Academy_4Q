#include "DataSystem.h"

DataSystem::DataSystem()
{
}

DataSystem::~DataSystem()
{
	RemoveShaders();
}


void DataSystem::LoadShaders()
{
	try
	{
		file::path shaderpath = PathFinder::Relative("Shaders\\");
		for (auto& dir : file::recursive_directory_iterator(shaderpath))
		{
			if (dir.is_directory())
				continue;

			if (dir.path().extension() == ".hlsl")
			{
				AddShaderFromPath(dir.path());
			}
		}
	}
	catch (const file::filesystem_error& e)
	{
		WARN("Could not load shaders" + e.what());
	}
	catch (const std::exception& e)
	{
		WARN("Error" + e.what());
	}
}

void DataSystem::LoadModels()
{
	file::path shaderpath = PathFinder::Relative("Models\\");
	for (auto& dir : file::recursive_directory_iterator(shaderpath))
	{
		if (dir.is_directory())
			continue;

		if (dir.path().extension() == ".dae" || dir.path().extension() == ".gltf" || dir.path().extension() == ".fbx")
		{
			AddModel(dir.path(), dir.path().parent_path());
		}
	}
}

//void DataSystem::ReloadShaders()
//{
//	OnShadersReloadedEvent.Invoke();
//	RemoveShaders();
//	LoadShaders();
//	ShadersReloadedEvent.Invoke();
//}

void DataSystem::AddShaderFromPath(const file::path& filepath)
{
	ComPtr<ID3DBlob> blob = HLSLCompiler::LoadFormFile(filepath.string());
	file::path filename = filepath.filename();
	std::string ext = filename.replace_extension().extension().string();
	filename.replace_extension();
	ext.erase(0, 1);

	AddShader(filename.string(), ext, blob);
}

void DataSystem::AddModel(const file::path& filepath, const file::path& dir)
{
	std::shared_ptr<Model>		model;
	std::shared_ptr<AnimModel>	animmodel;

	ModelLoader::LoadFromFile(filepath,dir, &model, &animmodel);
	if (model)
	{
		Models[model->name] = model;
	}
}

void DataSystem::AddShader(const std::string& name, const std::string& ext, const ComPtr<ID3DBlob>& blob)
{
	if (ext == "vs")
	{
		VertexShader vs = VertexShader(name, blob);
		vs.Compile();
		VertexShaders[name] = vs;
	}
	else if (ext == "hs")
	{
		HullShader hs = HullShader(name, blob);
		hs.Compile();
		HullShaders[name] = hs;
	}
	else if (ext == "ds")
	{
		DomainShader ds = DomainShader(name, blob);
		ds.Compile();
		DomainShaders[name] = ds;
	}
	else if (ext == "gs")
	{
		GeometryShader gs = GeometryShader(name, blob);
		gs.Compile();
		GeometryShaders[name] = gs;
	}
	else if (ext == "ps")
	{
		PixelShader ps = PixelShader(name, blob);
		ps.Compile();
		PixelShaders[name] = ps;
	}
	else if (ext == "cs")
	{
		ComputeShader cs = ComputeShader(name, blob);
		cs.Compile();
		ComputeShaders[name] = cs;
	}
	else
	{
		WARN("Unrecognized shader extension \"" + ext + "\" for shader \"" + name + "\"");
	}
}

void DataSystem::RemoveShaders()
{
	VertexShaders.clear();
	HullShaders.clear();
	DomainShaders.clear();
	GeometryShaders.clear();
	PixelShaders.clear();
	ComputeShaders.clear();
}
