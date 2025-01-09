#pragma once
#include "DeviceResources.h"
#include "LogicalDevice.h"
#include "Buffers.h"
#include "MPSO.h"
#include "Scene.h"
#include "Primitives.h"

class SceneRenderer
{
public:
	SceneRenderer(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources);
	~SceneRenderer();

	void Initialize();
	void SetCamera(Camera* camera);
	inline void SetScene(Scene* scene) { _scene = scene; };
	// Stages
	void StagePrepare();
    void EndStage();

	void AddDrawModel(const std::shared_ptr<Model>& model);
	void StageDrawModels();
	
private:
    std::shared_ptr<DirectX11::DeviceResources> deviceResources;

	std::vector<std::shared_ptr<Model>> _drawmodels;
	uint32								_modelcount;

	Camera*							_camera;
	Scene*							_scene;

	std::unique_ptr<LogicalDevice>	_device;
	std::unique_ptr<MPSO>			_mpso;
};
