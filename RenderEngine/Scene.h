#pragma once
#include "ImGuiRegister.h"
#include "InstancedModel.h"
#include "DataSystem.h"
#include "PointLight.h"
#include "InstancedBillboard.h"

struct Scene
{
	DirectX::XMFLOAT3 suncolor{ 1.f, 1.f, 0.96f };
	DirectX::XMFLOAT3 sunpos{ 3.f, 10.f, 3.f };
	DirectX::XMFLOAT3 iblcolor{ 1.f, 1.f, 1.f };
	float iblIntensity{ 0.22f };
	bool moreShadowSamples{ true };
	bool gaussianShadowBlur{ true };
	bool isEditorMode{ false };

	bool fxaa{ true };
	float bias{ 0.688f };
	float biasMin{ 0.021f };
	float spanMax{ 8.0f };

	std::unordered_map<std::string, uint32> instancedKeys;
	std::unordered_map<std::string, uint32> billboardInstancedKeys;
	std::vector<InstancedModel> ModelsData;
	std::vector<InstancedBillboard> billboards;
	uint32 numModels{ 0 };
	uint32 numBillboards{ 0 };
	PointLight pointLights[MAX_POINT_LIGHTS];
	uint32 numPointLights{ 0 };
	InstancedModel* m_pGround{};

	Scene()
	{
		ModelsData.reserve(1000);
		billboards.reserve(1000);
		//기본 지형 생성

		ImGui::ContextRegister("Scene Settings", [&]() 
		{
			ImGui::Text("dir light Setting");
			ImGui::ColorEdit3("Sun Color", &suncolor.x);
			ImGui::DragFloat3("Sun Position", &sunpos.x, 0.1f);
			ImGui::Separator();
			ImGui::Text("IBL Setting");
			ImGui::ColorEdit3("IBL Color", &iblcolor.x);
			ImGui::DragFloat("IBL Intensity", &iblIntensity, 0.01f, 0.0f, 100.0f);
			ImGui::Separator();
			ImGui::Text("blur Setting");
			ImGui::Checkbox("More Shadow Samples", &moreShadowSamples);
			ImGui::Checkbox("Gaussian Shadow Blur", &gaussianShadowBlur);
			ImGui::Checkbox("FXAA", &fxaa);
			ImGui::DragFloat("Bias", &bias, 0.001f, 0.0f, 1.0f);
			ImGui::DragFloat("Bias Min", &biasMin, 0.001f, 0.0f, 1.0f);
			ImGui::DragFloat("Span Max", &spanMax, 0.1f, 0.0f, 100.0f);
			ImGui::Separator();
			ImGui::Text("Point Light Setting");
			if(ImGui::Button("Add Point Light"))
			{
				PointLightBuffer light{};
				light.position = { 0.f, 0.f, 0.f };
				light.color = { 1.f, 1.f, 1.f };
				light.intensity = 1.f;
				light.range = 10.f;
				AddPointLight(light);
			}
			ImGui::SameLine();
			if (ImGui::Button("Remove Point Light"))
			{
				RemovePointLight(numPointLights - 1);
			}
			ImGui::Separator();
			ImGui::Text("plane Setting");

		}, /*ImGuiWindowFlags_NoMove | */ImGuiWindowFlags_AlwaysAutoResize);
	}

	~Scene();

	void AllDestroy()
	{
		instancedKeys.clear();
		billboardInstancedKeys.clear();
		ModelsData.clear();
		billboards.clear();

		if (isEditorMode)
		{
			m_pGround = AddModel
			(
				"plane",
				AssetsSystem->Models["plane"],
				Mathf::xVector{ 40.f, 1.f, 40.f, 1.f },
				Mathf::xVector{ DirectX::XMQuaternionIdentity() },
				Mathf::xVector{ 0.f, 0.f, 0.f, 1.f }
			);
			m_pGround->model->meshes[0].material->properties.metalness = 1.0f;
			m_pGround->model->meshes[0].material->properties.roughness = 1.0f;
		}
	}

	void SetEditorMode(bool mode)
	{
		isEditorMode = mode;
		if (isEditorMode)
		{
			m_pGround = AddModel
			(
				"plane",
				AssetsSystem->Models["plane"],
				Mathf::xVector{ 40.f, 1.f, 40.f, 1.f },
				Mathf::xVector{ DirectX::XMQuaternionIdentity() },
				Mathf::xVector{ 0.f, 0.f, 0.f, 1.f }
			);
			m_pGround->model->meshes[0].material->properties.metalness = 1.0f;
			m_pGround->model->meshes[0].material->properties.roughness = 1.0f;
		}
		else if (m_pGround)
		{
			RemoveModel("plane");
		}
	}

	InstancedModel* AddModel(const std::string_view& name, const std::shared_ptr<Model>& model)
	{
		ModelsData.emplace_back(model);
		ModelsData.back().instancedID = (uint32)ModelsData.size() - 1;
		ModelsData.back().instancedName = name;
		instancedKeys[name.data()] = ModelsData.back().instancedID;
		return &ModelsData.back();
	}

	InstancedModel* AddModel(
		const std::string_view& name, 
		const std::shared_ptr<Model>& model, 
		Mathf::xVector _scale,
		Mathf::xVector _rotation,
		Mathf::xVector _position)
	{
		ModelsData.emplace_back(model, _position, _rotation, _scale);
		ModelsData.back().instancedID = (uint32)ModelsData.size() - 1;
		ModelsData.back().instancedName = name;
		instancedKeys[name.data()] = ModelsData.back().instancedID;
		return &ModelsData.back();
	}

	InstancedModel* AddModel(const std::string_view& name, const std::shared_ptr<AnimModel>& model)
	{
		ModelsData.emplace_back(model);
		ModelsData.back().instancedID = (uint32)ModelsData.size() - 1;
		ModelsData.back().instancedName = name;
		instancedKeys[name.data()] = ModelsData.back().instancedID;
		return &ModelsData.back();
	}

	InstancedModel* AddModel(
		const std::string_view& name,
		const std::shared_ptr<AnimModel>& model,
		Mathf::xVector _scale,
		Mathf::xVector _rotation,
		Mathf::xVector _position)
	{
		ModelsData.emplace_back(model, _position, _rotation, _scale);
		ModelsData.back().instancedID = (uint32)ModelsData.size() - 1;
		ModelsData.back().instancedName = name;
		instancedKeys[name.data()] = ModelsData.back().instancedID;
		return &ModelsData.back();
	}

	InstancedBillboard* AddBillboard(const std::string_view& name, const std::shared_ptr<Billboard>& billboard)
	{
		billboards.emplace_back(billboard);
		billboards.back().InstancedID = (uint32)billboards.size() - 1;
		billboards.back()._instancedName = name;
		billboardInstancedKeys[name.data()] = billboards.back().InstancedID;

		return &billboards.back();
	}

	void RemoveModel(const std::string_view& name)
	{
		auto it = instancedKeys.find(name.data());
		if (it != instancedKeys.end())
		{
			ModelsData[it->second].isDestroyMark = true;
		}
	}

	void RemoveBillboard(const std::string_view& name)
	{
		auto it = billboardInstancedKeys.find(name.data());
		if (it != billboardInstancedKeys.end())
		{
			billboards[it->second]._isDestroyMark = true;
		}
	}

	InstancedModel& GetModel(const std::string_view& name)
	{
		return *std::ranges::find_if(ModelsData, [&](auto& model) 
		{ 
			return model.instancedID == instancedKeys[name.data()]; 
		});
	}

	InstancedModel* GetModelPtr(uint32 id)
	{
		if (id >= ModelsData.size())
		{
			return nullptr;
		}

		return &ModelsData[id];
	}

	InstancedModel& GetModel(uint32 id)
	{
		return *std::ranges::find_if(ModelsData, [&](auto& model)
		{
			return model.instancedID == id;
		});
	}

	InstancedBillboard& GetBillboard(const std::string_view& name)
	{
		return *std::ranges::find_if(billboards, [&](auto& billboard)
		{
			return billboardInstancedKeys[name.data()] == billboardInstancedKeys[name.data()];
		});
	}

	InstancedBillboard* GetBillboardPtr(uint32 id)
	{
		return &billboards[id];
	}

	void AddPointLight(const PointLightBuffer& light)
	{
		if (numPointLights < MAX_POINT_LIGHTS)
		{
			pointLights[numPointLights++].buffer = light;
			pointLights[numPointLights - 1].ID = numPointLights - 1;
		}
	}

	void RemovePointLight(uint32 id)
	{
		if (id < numPointLights)
		{
			std::copy(pointLights + id + 1, pointLights + numPointLights, pointLights + id);
			numPointLights--;
		}
	}

    void Update(float deltaSeconds)
    {
        foreach(ModelsData, [&](auto& model)
        {
            if (model.animModel)
            {
                model.animModel->animator->UpdateAnimation(deltaSeconds, model.GetMatrix());
            }
        });
    }

	void EditorOutline()
	{
		foreach(ModelsData, [&](auto& model)
		{
			if (model.outline.bitmask & OUTLINE_BIT)
			{
				model.outline.bitmask &= ~OUTLINE_BIT;
			}
		});
	}

	void DestroyModels()
	{
		std::vector<uint32> indecesToRemove;

		for (auto& [key, value] : instancedKeys)
		{
			if (ModelsData[value].isDestroyMark)
			{
				indecesToRemove.push_back(value);
			}
		}

		std::ranges::sort(indecesToRemove, std::greater());
		for (uint32 index : indecesToRemove)
		{
			if (ModelsData.empty())
			{
				break;
			}

			uint32 lastIndex = (uint32)ModelsData.size() - 1;
	
			if (index != lastIndex)
			{
				std::swap(ModelsData[index], ModelsData[lastIndex]);

				for (auto& [key, value] : instancedKeys)
				{
					if (value == lastIndex)
					{
						value = index;
						break;
					}
				}
			}

			ModelsData.pop_back();
		}

		std::erase_if(instancedKeys, [&](const auto& pair)
		{
			return std::ranges::find(indecesToRemove, pair.second) != indecesToRemove.end();
		});

		//인덱스 재정렬
		for (uint32 i = 0; i < ModelsData.size(); i++)
		{
			if (ModelsData[i].model)
			{
				instancedKeys[ModelsData[i].instancedName] = i;
				ModelsData[i].instancedID = i;
			}
			else if (ModelsData[i].animModel)
			{
				instancedKeys[ModelsData[i].instancedName] = i;
				ModelsData[i].instancedID = i;
			}
		}

		std::vector<uint32> billboardIndecesToRemove;

		for (auto& [key, value] : billboardInstancedKeys)
		{
			if (billboards[value]._isDestroyMark)
			{
				billboardIndecesToRemove.push_back(value);
			}
		}

		std::ranges::sort(billboardIndecesToRemove, std::greater());

		for (uint32 index : billboardIndecesToRemove)
		{
			if (billboards.empty())
			{
				break;
			}

			uint32 lastIndex = (uint32)billboards.size() - 1;

			if (index != lastIndex)
			{
				std::swap(billboards[index], billboards[lastIndex]);
				for (auto& [key, value] : billboardInstancedKeys)
				{
					if (value == lastIndex)
					{
						value = index;
						break;
					}
				}
			}
			billboards.pop_back();
		}

		std::erase_if(billboardInstancedKeys, [&](const auto& pair)
		{
			return std::ranges::find(billboardIndecesToRemove, pair.second) != billboardIndecesToRemove.end();
		});

		//인덱스 재정렬
		for (uint32 i = 0; i < billboards.size(); i++)
		{
			billboardInstancedKeys[billboards[i]._instancedName] = i;
			billboards[i].InstancedID = i;
		}

	}
};
