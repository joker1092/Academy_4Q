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
	AddModel(PathFinder::Relative("Models\\plane\\plane.fbx"), PathFinder::Relative("Models\\plane"));
}

void DataSystem::RenderForEditer()
{
	static std::string selectedModel{};

	ImGui::ContextRegister("Models", [&]()
	{
		constexpr float tileSize = 64.0f;
		constexpr float padding = 10.0f;
		constexpr int tilesPerRow = 4;
		int count = 0;

		for (const auto& [key, model] : Models)
		{
			ImGui::BeginGroup();

			if (ImGui::ImageButton(key.c_str(), icon.Get(), ImVec2(tileSize, tileSize)))
			{
				std::cout << "Selected Model: " << key << std::endl;
				selectedModel = key;
			}

			// 호버 상태를 감지
			if (ImGui::IsItemHovered()) 
			{
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

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				//만약에 시간이 남아돌아서 Guizmo를 쓴다면 여기서 모델을 드래그앤드롭해서 씬으로 보낼 수 있게 해야함.
				//ImGui::SetDragDropPayload("MODEL_PAYLOAD", selectedModel.c_str(), selectedModel.length());
				ImGui::Text("Drag to Scene : %s", selectedModel.c_str());
				if (Models[selectedModel] != dragDropModel)
				{
					dragDropModel = Models[selectedModel];
				}
				ImGui::EndDragDropSource();
			}
		}


	}, ImGuiWindowFlags_NoMove);
	//?닿굔 ?섏쨷??寃뚯엫 ?ㅻ툕?앺듃濡?媛?섏? ?댁빞??
	ImGui::ContextRegister("Models Material properties", [&]()
	{
		for (const auto& [key, model] : Models)
		{
			if(!model)
				continue;

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
