#pragma once
#include "DeviceResources.h"
#include "LogicalDevice.h"
#include "Buffers.h"
#include "MPSO.h"
#include "Scene.h"
#include "Primitives.h"
#include "InstancedModel.h"
#include "InstancedBillboard.h"

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

	void AddDrawModel(const InstancedModel& model);
	void UpdateDrawModel();
	void StageDrawModels();

	void SetEditModel(const std::shared_ptr<Model>& model);
	const std::shared_ptr<Model>& GetEditModel() { return _meshEditModel; }
	const std::unique_ptr<RenderTarget>& GetMeshEditorTarget() const;
	void StageEditModel();

	void AddBillboard(const InstancedBillboard& billboard);
	void UpdateDrawBillboards();
	void StageBillboardsPrepare();
	void StageDrawBillboards();
	
private:
    std::shared_ptr<DirectX11::DeviceResources> deviceResources;

	std::vector<InstancedModel>		_drawmodels;
	std::shared_ptr<Model>			_meshEditModel{};
	std::vector<InstancedBillboard>	_billboards;
	uint32							_modelcount{};
	uint32 							_billboardcount{};

	Camera*							_camera{};
	Scene*							_scene{};

	std::unique_ptr<LogicalDevice>	_device{};
	std::unique_ptr<MPSO>			_mpso{};

	bool isEditor{ true };
};
