#pragma once
#include "ImGuiRegister.h"

struct Scene
{
	DirectX::XMFLOAT3 suncolor{ 1.f, 1.f, 0.96f };
	DirectX::XMFLOAT3 sunpos{ 3.f, 10.f, 3.f };
	DirectX::XMFLOAT3 iblcolor{ 1.f, 1.f, 1.f };
	float iblIntensity{ 0.1f };
	bool moreShadowSamples{ false };
	bool gaussianShadowBlur{ false };

	bool fxaa{ true };
	float bias{ 0.688f };
	float biasMin{ 0.021f };
	float spanMax{ 8.0f };

	Scene()
	{
		ImGui::ContextRegister("Scene Settings", [&]() 
		{
			ImGui::ColorEdit3("Sun Color", &suncolor.x);
			ImGui::DragFloat3("Sun Position", &sunpos.x, 0.1f);
			ImGui::ColorEdit3("IBL Color", &iblcolor.x);
			ImGui::DragFloat("IBL Intensity", &iblIntensity, 0.01f, 0.0f, 1.0f);
			ImGui::Checkbox("More Shadow Samples", &moreShadowSamples);
			ImGui::Checkbox("Gaussian Shadow Blur", &gaussianShadowBlur);
			ImGui::Separator();
			ImGui::Checkbox("FXAA", &fxaa);
			ImGui::DragFloat("Bias", &bias, 0.001f, 0.0f, 1.0f);
			ImGui::DragFloat("Bias Min", &biasMin, 0.001f, 0.0f, 1.0f);
			ImGui::DragFloat("Span Max", &spanMax, 0.1f, 0.0f, 100.0f);
		});
	}
};