#pragma once
#include "Core.Minimal.h"
#include "DeviceResources.h"
#include "Camera.h"
#include "Scene.h"
#include "Model.h"
#include "MeshBasedPSO.h"
#include "StateDevice.h"

class SceneRenderer
{
public:
	SceneRenderer(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources);
	~SceneRenderer();

	void Initialize();
	void SetCamera(Camera* camera);
	void SetScene(Scene* scene);

	void StagePrepare();
	void StageSubmit();

	void AddDrawModel(Model* model);
	void StageDrawModels();

private:
	Camera* m_camera{};
	Scene* m_scene{};

	Mathf::xVColor4 m_clearColor{ 0.01f, 0.01f, 0.01f, 1.0f };

	std::vector<Model*> m_drawModels{};

    std::unique_ptr<StateDevice> m_stateDevice{};
	std::unique_ptr<MeshBasedPSO> m_pso{};
};
