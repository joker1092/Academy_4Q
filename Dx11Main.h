#pragma once
#include "Utility_Framework/DeviceResources.h"
#include "Utility_Framework/TimeSystem.h"
#include "RenderEngine/DataSystem.h"
#include "RenderEngine/SceneRenderer.h"
#include "RenderEngine/Camera.h"
#include "RenderEngine/Scene.h"
#include "RenderEngine/ImGuiRenderer.h"
#include "RenderEngine/InstancedModel.h"
#include <memory>

namespace DirectX11
{
	class Dx11Main : public IDeviceNotify
	{
	public:
		Dx11Main(const std::shared_ptr<DeviceResources>& deviceResources);
		~Dx11Main();
		void SceneInitialize();
		void CreateWindowSizeDependentResources();
		void Update();
		bool Render();

		// IDeviceNotify
		virtual void OnDeviceLost() override;
		virtual void OnDeviceRestored() override;

	private:
		std::shared_ptr<DeviceResources> m_deviceResources;
		TimeSystem m_timeSystem;
		std::unique_ptr<SceneRenderer> m_sceneRenderer;
		std::unique_ptr<ImGuiRenderer> m_imguiRenderer;

		std::unique_ptr<Camera> m_camera;
		std::unique_ptr<Scene> m_scene;

		InstancedModel* m_pModel{};
		InstancedModel* m_pModel2{};
		InstancedModel* m_pGround{};
	};
}
