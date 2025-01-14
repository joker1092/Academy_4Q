#include "DataSystem.h"
#include "ImGuiRegister.h"

DataSystem::~DataSystem()
{
	RemoveShaders();
}

void DataSystem::Initialize()
{
	m_DataThread = std::thread(&DataSystem::MonitorFiles, this);
	m_DataThread.detach();
	RenderForEditer();
}

void DataSystem::RenderForEditer()
{
	static std::string selectedModel{};
	// 타일의 크기와 간격 설정

	ImGui::ContextRegister("Models", [&]()
	{
		constexpr float tileSize = 64.0f;  // 타일 크기 (아이콘 크기)
		constexpr float padding = 10.0f;  // 타일 간 간격
		constexpr int tilesPerRow = 4;    // 한 줄에 표시할 타일 수
		int count = 0;

		for (const auto& [key, model] : Models)
		{
			ImGui::BeginGroup();

			if (ImGui::ImageButton(key.c_str(), icon.Get(), ImVec2(tileSize, tileSize)))
			{
				std::cout << "Selected Model: " << key << std::endl;
				selectedModel = key;
			}

			if (selectedModel == key)
			{
				ImGui::TextWrapped("[Selected]");
			}

			ImGui::TextWrapped("%s", key.c_str());

			ImGui::EndGroup();

			count++;
			if (count % tilesPerRow != 0)
			{
				ImGui::SameLine(0.0f, padding);
			}
		}
	}, ImGuiWindowFlags_NoMove);
	//이건 나중에 게임 오브젝트로 가던지 해야됨.
	ImGui::ContextRegister("Models Material properties", [&]()
		{
			for (const auto& [key, model] : Models)
			{
				if (selectedModel == key)
				{
					float* diffuse[3]
					{ 
						&model->meshes[0].material->properties.diffuse.x, 
						&model->meshes[0].material->properties.diffuse.y, 
						&model->meshes[0].material->properties.diffuse.z 
					};
					ImGui::ColorEdit3("Diffuse", diffuse[0]);
					ImGui::SliderFloat("Roughness", &model->meshes[0].material->properties.roughness, 0.3f, 1.0f);
					ImGui::SliderFloat("Metalness", &model->meshes[0].material->properties.metalness, 0.3f, 1.0f);
				}
			}
		});
}

void DataSystem::MonitorFiles()
{
	while (true)
	{
		uint32 modelcount = 0;
		uint32 shadercount = 0;
		try
		{
			file::path shaderpath = PathFinder::Relative("Shaders\\");
			for (auto& dir : file::recursive_directory_iterator(shaderpath))
			{
				if (dir.is_directory())
					continue;

				if (dir.path().extension() == ".hlsl")
				{
					shadercount++;
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
		try
		{
			file::path modelpath = PathFinder::Relative("Models\\");
			for (auto& dir : file::recursive_directory_iterator(modelpath))
			{
				if (dir.is_directory())
					continue;
				if (dir.path().extension() == ".dae" || dir.path().extension() == ".gltf" || dir.path().extension() == ".fbx")
				{
					modelcount++;
				}
			}
		}
		catch (const file::filesystem_error& e)
		{
			WARN("Could not load models" + e.what());
		}
		catch (const std::exception& e)
		{
			WARN("Error" + e.what());
		}
		if (modelcount != currModelFileCount)
		{
			std::cout << "Model count changed" << std::endl;
			LoadModels();
			currModelFileCount = modelcount;
		}
		if (shadercount != currShaderFileCount)
		{
			std::cout << "Shader count changed" << std::endl;
			LoadShaders();
			currShaderFileCount = shadercount;
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

void DataSystem::LoadShaders()
{
	icon = TextureLoader::LoadFromFile(PathFinder::Relative("Icon\\Model.png"));

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
				currShaderFileCount++;
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
			currModelFileCount++;
		}
	}
}

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
	std::string name = file::path(filepath.filename()).replace_extension().string();
	if(Models[name])
	{
		WARN("Model \"" + name + "\" already exists");
		return;
	}

	std::shared_ptr<Model>		model;
	std::shared_ptr<AnimModel>	animmodel;

	ModelLoader::LoadFromFile(filepath, dir, &model, &animmodel);
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
