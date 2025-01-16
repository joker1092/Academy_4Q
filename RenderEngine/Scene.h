#pragma once
#include "ImGuiRegister.h"
#include "InstancedModel.h"
#include "DataSystem.h"
#include "../InputManager.h"

struct Scene
{
	DirectX::XMFLOAT3 suncolor{ 1.f, 1.f, 0.96f };
	DirectX::XMFLOAT3 sunpos{ 3.f, 10.f, 3.f };
	DirectX::XMFLOAT3 iblcolor{ 1.f, 1.f, 1.f };
	float iblIntensity{ 0.22f };
	bool moreShadowSamples{ false };
	bool gaussianShadowBlur{ false };

	bool fxaa{ true };
	float bias{ 0.688f };
	float biasMin{ 0.021f };
	float spanMax{ 8.0f };

	std::unordered_map<std::string, uint32> instancedKeys;
	std::vector<InstancedModel> ModelsData;
	std::string selectedModel;
	InstancedModel* m_pGround;

	Scene()
	{
		//기본 지형 생성
		m_pGround = AddModel
		(
			"plane",
			AssetsSystem->Models["plane"],
			Mathf::xVector{ 20.f, 1.f, 20.f, 1.f },
			Mathf::xVector{ DirectX::XMQuaternionIdentity() },
			Mathf::xVector{ 0.f, 0.f, 0.f, 1.f }
		);
		m_pGround->model->meshes[0].material->properties.metalness = 1.0f;
		m_pGround->model->meshes[0].material->properties.roughness = 0.3f;

		ImGui::ContextRegister("Scene Settings", [&]() 
		{
			ImGui::ColorEdit3("Sun Color", &suncolor.x);
			ImGui::DragFloat3("Sun Position", &sunpos.x, 0.1f);
			ImGui::Separator();
			ImGui::ColorEdit3("IBL Color", &iblcolor.x);
			ImGui::DragFloat("IBL Intensity", &iblIntensity, 0.01f, 0.0f, 1.0f);
			ImGui::Separator();
			ImGui::Checkbox("More Shadow Samples", &moreShadowSamples);
			ImGui::Checkbox("Gaussian Shadow Blur", &gaussianShadowBlur);
			ImGui::Separator();
			ImGui::Checkbox("FXAA", &fxaa);
			ImGui::DragFloat("Bias", &bias, 0.001f, 0.0f, 1.0f);
			ImGui::DragFloat("Bias Min", &biasMin, 0.001f, 0.0f, 1.0f);
			ImGui::DragFloat("Span Max", &spanMax, 0.1f, 0.0f, 100.0f);

		}, /*ImGuiWindowFlags_NoMove | */ImGuiWindowFlags_AlwaysAutoResize);

		ImGui::ContextRegister("Models", [&]()
		{
			auto model = AssetsSystem->GetPayloadModel();
			auto animModel = AssetsSystem->GetPayloadAnimModel();
			if (model && InputManagement->IsMouseButtonReleased(MouseKey::LEFT))
			{
				std::cout << "Dropped Model : " << model->name << std::endl;
				static uint32 id;

				std::string name = model->name + std::to_string(id);
				AddModel(model->name + std::to_string(++id), model);
				AssetsSystem->ClearPayloadModel();
			}

			if (animModel && InputManagement->IsMouseButtonReleased(MouseKey::LEFT))
			{
				std::cout << "Dropped Model : " << animModel->name << std::endl;
				static uint32 id;
				std::string name = animModel->name + std::to_string(id);
				AddModel(animModel->name + std::to_string(++id), animModel);
				AssetsSystem->ClearPayloadAnimModel();
			}

		}, ImGuiWindowFlags_NoMove);

		ImGui::ContextRegister("Scene Models List", [&]()
		{
			for (auto& [name, index] : instancedKeys)
			{
				if ("plane" == name)
					continue;

				if (ImGui::Selectable(name.c_str(), false))
				{
					std::cout << "Selected Model: " << name << std::endl;
					selectedModel = name.c_str();
				}
				ImGui::SameLine();
				ImGui::Text("ID : %d", index);
			}

			if (ImGui::BeginPopupContextItem("R_ClickMenu"))
			{
				if (ImGui::MenuItem("Delete"))
				{
					std::cout << "Delete Model" << std::endl;
					if (!selectedModel.empty())
					{
						RemoveModel(selectedModel);
						selectedModel.clear();
					}
				}
				ImGui::EndPopup();
			}

		}, ImGuiWindowFlags_None);

		ImGui::ContextRegister("Inspector", [&]()
		{
			if (selectedModel.empty())
				return;

			uint32 index = instancedKeys[selectedModel];
			auto model = std::ranges::find_if(ModelsData, [&](auto& model) 
			{
				return model.instancedID == index; 
			});

			ImGui::Text("Model Name : %s", selectedModel.c_str());

			Mathf::Vector3 pos = model->position;
			Mathf::Vector3 rot = { model->rotationX, model->rotationY, model->rotationZ };
			Mathf::Vector3 scale = model->scale;

			ImGui::DragFloat3("Position", &pos.x, 0.01f);
			ImGui::DragFloat3("Rotation", &rot.x, 0.01f);
			ImGui::DragFloat3("Scale", &scale.x, 0.01f);

			model->position = pos;
			model->SetRotation(rot.x, rot.y, rot.z);
			model->scale = scale;

		}, ImGuiWindowFlags_None);
	}
	~Scene() = default;
	//TODO : 끙... 지금은 인스턴스 아이디로 관리하는데 이게 인덱스값이 되는 것이 현명해보인다.
	InstancedModel* AddModel(const std::string_view& name, const std::shared_ptr<Model>& model)
	{
		ModelsData.emplace_back(model);
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
		instancedKeys[name.data()] = ModelsData.back().instancedID;
		return &ModelsData.back();
	}

	InstancedModel* AddModel(const std::string_view& name, const std::shared_ptr<AnimModel>& model)
	{
		ModelsData.emplace_back(model);
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
		instancedKeys[name.data()] = ModelsData.back().instancedID;
		return &ModelsData.back();
	}

	void RemoveModel(const std::string_view& name)
	{
		auto it = instancedKeys.find(name.data());
		if (it != instancedKeys.end())
		{
			std::erase_if(ModelsData, [&](auto& model) 
			{ 
				return model.instancedID == it->second;
			});
			instancedKeys.erase(it);
		}
	}

	InstancedModel& GetModel(const std::string_view& name)
	{
		return *std::ranges::find_if(ModelsData, [&](auto& model) 
		{ 
			return model.instancedID == instancedKeys[name.data()]; 
		});
	}
};